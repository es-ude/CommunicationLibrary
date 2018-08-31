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
            print(row[0])
            time_b = datetime.datetime.strptime(row[0], time_format)
        else:
            time_a = time_b
            time_b = datetime.datetime.strptime(row[0], time_format)
            delta_times.append(time_b - time_a)
    print(delta_times)
    number_of_delta_times = len(delta_times)
    sum_of_delta_times = lambda 
