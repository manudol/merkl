# generates a random sequence of 500 bases in seq0.txt
# run "python3 gen_seq.py" to generate the seq0.txt file
import random

bases = ['A', 'T', 'G', 'C']

def gen_seq():
    file = open("./seq0.txt", "w")

    for i in range(10):
        seq = ""
        for j in range(50):
            seq += bases[random.randint(0, 3)]
        seq += "\n"
        file.write(seq)


if __name__ == '__main__':
    gen_seq()