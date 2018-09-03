import sys
with open(sys.argv[1], 'r') as file:
    last_number = 0
    skipped = 0
    doubles = 0
    total_lines = 0
    first_number = 0
    for line in file:
        current_number = int(line)
        if last_number is 0:
            last_number = current_number
            first_number = current_number
        if current_number > last_number + 1:
            skipped += current_number - (last_number + 1)
        elif current_number <= last_number:
            doubles += 1
        last_number = current_number
    total_lines = last_number - first_number
    print("skipped: {}".format(float(skipped)/(total_lines)))
    print("wrong order or doubled: {}".format(float(doubles)/(total_lines)))
    print("total lines: {}".format(total_lines))
