"""
This script evaluates the output of
XCTU at console. It is supposed to
calculate the average delta time
between the receivement of two messages.
As well as the variance of these values.
"""
import sys
import csv
import datetime

with open(sys.argv[1], 'r') as file:
    reader = csv.reader(file, delimiter=',')
    time_a = None
    time_b = None
    delta_times = []
    time_format = "%m-%d-%Y %H:%M:%S.%f"
    for row in reader:
        if time_a is None and time_b is None:
            time_b = datetime.datetime.strptime(row[0], time_format)
        else:
            time_a = time_b
            time_b = datetime.datetime.strptime(row[0], time_format)
            delta_times.append(time_b - time_a)
    delta_times = [x/datetime.timedelta(microseconds=1) for x in delta_times]
    number_of_delta_times = len(delta_times)
    average_delta_time = sum(delta_times) / number_of_delta_times
    print("number of delta times: {}".format(number_of_delta_times))
    print("average delta time in microsecond: {}".format(average_delta_time))
    variance = sum([(x - average_delta_time)*(x - average_delta_time) for x in delta_times]) / number_of_delta_times
    print("variance: {}".format(variance))
    print("variance / average delta time: {}".format(variance/average_delta_time))
