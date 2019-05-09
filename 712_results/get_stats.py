import matplotlib.pyplot as plt
import sys
from datetime import datetime
import pandas as pd
import numpy as np

def get_dec_s(secs):
    base, exp = secs.split("-")
    return float("0.{}{}".format("0" * int(exp), base))

def reject_outliers(data):
    np_data = np.array(data)
    quartile_1, quartile_3 = np.percentile(np_data, [10, 90])
    iqr = quartile_3 - quartile_1
    lower_bound = quartile_1 - (iqr * 1.5)
    upper_bound = quartile_3 + (iqr * 1.5)
    lower_cond = (np_data > lower_bound)
    upper_cond = (np_data < upper_bound)
    if lower_bound == upper_bound:
        lower_cond = (np_data >= lower_bound)
        upper_cond = (np_data <= upper_bound)

    return np_data[lower_cond & upper_cond].tolist()

def graph_msg_size(primary_log, comm_method):
    sizes = []
    with open(primary_log, "r") as f:
        for line in f:
            if "sending message of size" in line:
                size = ((line.split("sending message of size"))[1].split())[0]
                sizes.append(int(size))
    sizes.sort()
    max_size = sizes[-1]
    print(max_size)
    n_bins = int(max_size / 1000) # 10KB
    print(n_bins)
    print("Total messages: {}".format(len(sizes)))
    # print("Min size: {}".format(sizes[0]))
    # print("Max size: {}".format(sizes[-1]))
    # print("Median size: {}".format(sizes[len(sizes) // 2]))
    # print("Mean size: {}".format(sum([size[1] for size in sizes]) / len(sizes)))
    plt.hist(sizes, bins=30)
    plt.xlabel("Message size (bytes)")
    plt.ylabel("Number of messages")
    plt.tight_layout()
    # plt.show()
    plt.savefig("{}_msg_size.png".format(comm_method))
    plt.clf()
    plt.close()

def graph_one_way_latency(standby_log, comm_method):
    latencies = []
    with open(standby_log, "r") as f:
        for line in f:
            if "sendtime" in line and "receipttime" in line:
                split_line = line.split("sendtime")[1].split()

                send_time = split_line[1]
                # send_time_t = pd.Timestamp(send_time.split(".")[0])
                # send_time = get_dec_s(send_time.split(".")[1])
                # send_time = pd.Timestamp(send_time_t + strsend_time_s)
                send_time = pd.Timestamp(send_time.split("-")[0])

                receipt_time = split_line[4]
                # receipt_time_t = pd.Timestamp(receipt_time.split(".")[0])
                # receipt_time = get_dec_s(receipt_time.split(".")[1])
                # receipt_time = pd.Timestamp(receipt_time_t + receipt_time_s)
                receipt_time = pd.Timestamp(receipt_time.split("-")[0])
                
                # item = ((receipt_time_t - send_time_t).total_seconds() + 
                #     (receipt_time - send_time)) * 1000000
                item = (receipt_time - send_time).total_seconds() * 1000
                latencies.append(item)
    
    print(max(latencies), min(latencies))
    latencies = reject_outliers(latencies)
    print(max(latencies), min(latencies))
    x = np.sort(latencies)
    y = np.arange(len(x))/float(len(x))
    plt.plot(x, y)
    plt.xlabel("Master to Standby Latency (ms)")
    plt.ylabel("Percentile")
    plt.tight_layout()
    # plt.show()
    plt.savefig("{}_one_way_latency.png".format(comm_method))
    plt.clf()
    plt.close()

def graph_rtt_latency(primary_log, standby_log, comm_method):
    ms_latencies = []
    with open(standby_log, "r") as f:
        for line in f:
            if "sendtime" in line and "receipttime" in line:
                split_line = line.split("sendtime")[1].split()

                send_time = split_line[1]
                # send_time_t = pd.Timestamp(send_time.split(".")[0])
                # send_time = get_dec_s(send_time.split(".")[1])
                # send_time = pd.Timestamp(send_time_t + strsend_time_s)
                send_time = pd.Timestamp(send_time.split("-")[0])

                receipt_time = split_line[4]
                # receipt_time_t = pd.Timestamp(receipt_time.split(".")[0])
                # receipt_time = get_dec_s(receipt_time.split(".")[1])
                # receipt_time = pd.Timestamp(receipt_time_t + receipt_time_s)
                receipt_time = pd.Timestamp(receipt_time.split("-")[0])
                
                # item = ((receipt_time_t - send_time_t).total_seconds() + 
                #     (receipt_time - send_time)) * 1000000
                item = (receipt_time - send_time).total_seconds() * 1000
                ms_latencies.append(item)
    
    sm_latencies = []
    alt = True
    with open(primary_log, "r") as f:
        for line in f:
            if "reply_time" in line and "current_time" in line:
                if alt:
                    split_line = line.split("reply_time")[1].split()

                    send_time = split_line[1]
                    # send_time_t = pd.Timestamp(send_time.split(".")[0])
                    # send_time = get_dec_s(send_time.split(".")[1])
                    # send_time = pd.Timestamp(send_time_t + strsend_time_s)
                    send_time = pd.Timestamp(send_time.split("-")[0])

                    receipt_time = split_line[4]
                    # receipt_time_t = pd.Timestamp(receipt_time.split(".")[0])
                    # receipt_time = get_dec_s(receipt_time.split(".")[1])
                    # receipt_time = pd.Timestamp(receipt_time_t + receipt_time_s)
                    receipt_time = pd.Timestamp(receipt_time.split("-")[0])
                    
                    # item = ((receipt_time_t - send_time_t).total_seconds() + 
                    #     (receipt_time - send_time)) * 1000000
                    item = (receipt_time - send_time).total_seconds() * 1000
                    sm_latencies.append(item)
                    
                    alt = False
                else:
                    alt = True
    
    if len(ms_latencies) > len(sm_latencies):
        ms_latencies = ms_latencies[0:len(sm_latencies)]
    elif len(ms_latencies) < len(sm_latencies):
        sm_latencies = sm_latencies[0:len(ms_latencies)]
    print(len(ms_latencies), len(sm_latencies))
    trips = list(zip(ms_latencies, sm_latencies))
    print(trips[0:10])
    latencies = [sum(trip) for trip in trips]
    
    print(max(latencies), min(latencies))
    latencies = reject_outliers(latencies)
    print(max(latencies), min(latencies))
    x = np.sort(latencies)
    y = np.arange(len(x))/float(len(x))
    plt.plot(x, y)
    plt.xlabel("RTT Latency (ms)")
    plt.ylabel("Percentile")
    plt.tight_layout()
    # plt.show()
    plt.savefig("{}_rtt.png".format(comm_method))
    plt.clf()
    plt.close()

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("usage: python get_stats.py <primary logfile> <standby logfile> <eRPC/normal>")
        sys.exit(1)
    
    primary_log = sys.argv[1]
    standby_log = sys.argv[2]

    graph_msg_size(primary_log, sys.argv[3])
    graph_one_way_latency(standby_log, sys.argv[3])
    graph_rtt_latency(primary_log, standby_log, sys.argv[3])
