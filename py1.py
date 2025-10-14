import random

bases = ['A', 'T', 'G', 'C']

file = open("./seq0.txt", "w")

for i in range(10):
    seq = ""
    for j in range(50):
        seq += bases[random.randint(0, 3)]
    seq += "\n"
    file.write(seq)