# PLEASE READ README FIRST!
# Please drop this code in SD_CARD/file directory that contains "GRKXX.txt"

"""
====================================
            EDIT HERE
====================================
"""

# Write your range for file, here
# Ex: you want to convert GRK1-GRK10, so please write start = 1, and finish = 10
start = 5000
finish = 5727

# Value for default
KANAN21 = 180
KANAN22 = 87
KANAN23 = 135
KANAN24 = 193
KANAN25 = 145
KANAN26 = 150
KIRI31 = 225
KIRI32 = 245
KIRI33 = 150
KIRI34 = 247
KIRI35 = 160
KIRI36 = 150

"""
====================================
"""

# Define a function to modify the file contents
def modify_file(file_name, operations):
    with open(file_name, "r+") as f:
        # Read all lines into a list
        lines = f.readlines()
        # Modify each line according to the corresponding operation
        for i in range(len(lines)):
            line = lines[i].strip().split(";")
            num = float(line[1][:-1])
            x, y, z = operations[i]
            result = int(num + x - (y / z))
            line[1] = f"{result}B\n"
            lines[i] = ";".join(line)
        # Move the file pointer to the beginning of the file
        f.seek(0)
        # Write the modified lines back to the file
        f.writelines(lines)
        # Truncate any remaining content (if any)
        f.truncate()

# Define the operations for each line
operations = [
    (2045, KANAN21, 0.088),
    (99,   KANAN22, 0.29),
    (465,  KANAN23, 0.29),
    (665,  KANAN24, 0.29),
    (500,  KANAN25, 0.29),
    (517,  KANAN26, 0.29),
    (2556, KIRI31,  0.088),
    (183,  KIRI32,  0.29),
    (517,  KIRI33,  0.29),
    (851,  KIRI34,  0.29),
    (551,  KIRI35,  0.29),
    (517,  KIRI36,  0.29),
]

# Call the function for each file
for i in range(start, finish+1):
    file_name = f"GRK{i}.txt"
    modify_file(file_name, operations)
