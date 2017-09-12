#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <unordered_set>

#include "mica/table/fixedtable.h"
#include "mica/util/hash.h"
#include "util/huge_alloc.h"

/*
 * 0: No prefetch
 * 1: Prefetch table
 */
#define USE_PREFETCH 1
#define VAL_SIZE 16

// Dummy registration and deregistration functions for HugeAlloc
#define DUMMY_MR_PTR (reinterpret_cast<void *>(0x3185))
#define DUMMY_LKEY (3186)
ERpc::Transport::MemRegInfo reg_mr_wrapper(void *, size_t) {
  return ERpc::Transport::MemRegInfo(DUMMY_MR_PTR, DUMMY_LKEY);
}
void dereg_mr_wrapper(ERpc::Transport::MemRegInfo) {}

using namespace std::placeholders;
typename ERpc::Transport::reg_mr_func_t reg_mr_func =
    std::bind(reg_mr_wrapper, _1, _2);
typename ERpc::Transport::dereg_mr_func_t dereg_mr_func =
    std::bind(dereg_mr_wrapper, _1);
// End dummy registration and deregistration functions for HugeAlloc

struct FixedTableConfig : public ::mica::table::BasicFixedTableConfig {
  static constexpr bool kFetchAddOnlyIfEven = false;
};
typedef ::mica::table::FixedTable<FixedTableConfig> FixedTable;

typedef ::mica::table::Result MicaResult; /* An enum */
typedef uint64_t test_key_t;
struct test_val_t {
  uint64_t buf[VAL_SIZE / sizeof(uint64_t)];
};

template <typename T>
static uint64_t mica_hash(const T *key, size_t key_length) {
  return ::mica::util::hash(key, key_length);
}

static inline uint32_t hrd_fastrand(uint64_t *seed) {
  *seed = *seed * 1103515245 + 12345;
  return static_cast<uint32_t>(*seed >> 32);
}

int num_iters = 100;

int main() {
  uint64_t seed = 0xdeadbeef;
  std::chrono::high_resolution_clock timer;

  auto config = ::mica::util::Config::load_file("test_table.json");

  size_t num_keys =
      static_cast<size_t>(config.get("test").get("num_keys").get_int64());
  assert(num_keys > 0);

  size_t batch_size =
      static_cast<size_t>(config.get("test").get("batch_size").get_int64());
  assert(batch_size > 0 && num_keys % batch_size == 0);

  auto *alloc = new ERpc::HugeAlloc(1024, 0, reg_mr_func, dereg_mr_func);

  FixedTable table(config.get("table"), VAL_SIZE, &alloc, true);

  auto *key_arr = reinterpret_cast<test_key_t *>(alloc->alloc_raw(
     num_keys * sizeof(test_key_t), 0));
  ERpc::rt_assert(key_arr != nullptr, "");

  auto *val_arr = reinterpret_cast<test_val_t *>(alloc->alloc_raw(
      num_keys * sizeof(test_val_t), 0));

  uint64_t *key_hash_arr = new uint64_t[batch_size];
  MicaResult out_result;
  std::unordered_set<uint64_t> S;

  /*
   * This is more of a correctness test: the keys are queried in the same
   * order as they are inserted, causing sequential pool memory accesses.
   */
  for (int iter = 0; iter < num_iters; iter++) {
    /* Populate @key_arr with unique keys */
    printf("Iteration %d: Generating keys\n", iter);
    for (size_t i = 0; i < num_keys; i++) {
      uint64_t key = hrd_fastrand(&seed);
      while (S.count(key) == 1) {
        key = hrd_fastrand(&seed);
      }

      S.insert(key);
      key_arr[i] = key;
      val_arr[i].buf[0] = key;
    }

    S.clear();

    /* Insert */
    printf("Iteration %d: Setting keys\n", iter);
    auto start = timer.now();
    for (size_t i = 0; i < num_keys; i += batch_size) {
      for (size_t j = 0; j < batch_size; j++) {
        key_hash_arr[j] = mica_hash(&key_arr[i + j], sizeof(test_key_t));
#if USE_PREFETCH == 1
        table.prefetch_table(key_hash_arr[j]);
#endif
      }

      for (size_t j = 0; j < batch_size; j++) {
        out_result =
            table.set(key_hash_arr[j], key_arr[i + j],
                      reinterpret_cast<char *>(&val_arr[i + j]));
        if (out_result != MicaResult::kSuccess) {
          printf("Inserting key %zu failed. Error = %s\n", i + j,
                 ::mica::table::ResultString(out_result).c_str());
          exit(-1);
        }
      }
    }

    auto end = timer.now();
    double us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    printf("Insert tput = %.2f M/s\n", num_keys / us);

    /* GET */
    start = timer.now();
    for (size_t i = 0; i < num_keys; i += batch_size) {
      for (size_t j = 0; j < batch_size; j++) {
        key_hash_arr[j] = mica_hash(&key_arr[i + j], sizeof(test_key_t));
#if USE_PREFETCH == 1
        table.prefetch_table(key_hash_arr[j]);
#endif
      }

      for (size_t j = 0; j < batch_size; j++) {
        test_val_t temp_val;

        out_result =
            table.get(key_hash_arr[j], key_arr[i + j],
                      reinterpret_cast<char *>(&temp_val));
        if (out_result != MicaResult::kSuccess) {
          printf("GET failed for key %zu. Error = %s\n", i + j,
                 ::mica::table::ResultString(out_result).c_str());
          exit(-1);
        }

        if (temp_val.buf[0] != key_arr[i + j]) {
          printf("Bad value for key %lu. Expected = %lu, got = %lu\n",
                 key_arr[i + j], key_arr[i + j], temp_val.buf[0]);
          exit(-1);
        }
      }
    }

    end = timer.now();
    us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    printf("Get tput = %.2f M/s\n", num_keys / us);

    /* Delete */
    for (size_t i = 0; i < num_keys; i++) {
      uint64_t key_hash = mica_hash(&key_arr[i], sizeof(test_key_t));
      out_result = table.del(key_hash, key_arr[i]);
      if (out_result != MicaResult::kSuccess) {
        printf("Delete failed for key %zu.\n", i);
        exit(-1);
      }
    }
  }

  printf("Done test\n");
  (void)out_result;

  return EXIT_SUCCESS;
}
