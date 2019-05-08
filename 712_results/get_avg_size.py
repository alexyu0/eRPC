import datetime
import matplotlib.pyplot as plt

path = "logfile_tpcc_benchmark"
file = open(path, "r")
sizes = []
for line in file:
    # print(line)
    if "11:5" in line and "Size of log msg" in line:
        line_split = line.strip().split()
        # time = datetime.datetime.strptime(line_split[0] + " " + line_split[1][:-4], "%Y-%m-%d %H:%M:%S")
        # sizes.append((time,int(line_split[-1])))
        sizes.append(int(line_split[-1]))
sizes = sizes[6:len(sizes)-9]
sizes.sort()

# # Aggregate per minute
# aggregate = {}
# aggregate_count = {}
# for size in sizes:
#     time = size[0]
#     if time in aggregate:
#         aggregate[time] += size[1]
#         aggregate_count[time] += 1
#     else:
#         aggregate[time] = size[1]
#         aggregate_count[time] = 1

# for time in aggregate:
#     aggregate[time] = aggregate[time] / aggregate_count[time]


# aggregate_list = []
# for x in aggregate: 
#     aggregate_list.append((x, aggregate[x]))



# print(aggregate_list[0])
# aggregate_list.sort()
# times = [agg[0] for agg in aggregate_list]
# start = times[0]
# times = [(time - start).seconds for time in times]
# sizes = [agg[1] for agg in aggregate_list]
max_size = sizes[-1]
print(max_size)
n_bins = int(max_size / 1000) # 10KB
print(n_bins)

print("Total messages: {}".format(len(sizes)))
# print("Min size: {}".format(sizes[0]))
# print("Max size: {}".format(sizes[-1]))
# print("Median size: {}".format(sizes[len(sizes) // 2]))
# print("Mean size: {}".format(sum([size[1] for size in sizes]) / len(sizes)))

plt.hist(sizes, bins = 30)
plt.xlabel("Message size (bytes)")
plt.ylabel("Number of messages")


plt.show()

