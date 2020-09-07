import sys
import random

punc = ('.', '!')
count = 0
new_word = ""

inFile = ("dressed.txt")

with open(inFile, 'r') as shuf:
    for line in shuf.readlines():
        for word in line.split():
            if len(word) > 2:
                if word.endswith(punc):
                    word1 = word[1:-2]
                    word1 = random.sample(word1, len(word1))
                    word1.insert(0, word[0])
                    word1.append(word[-2])
                    word1.append(word[-1])
                else:
                    word1 = word[1:-1]
                    word1 = random.sample(word1, len(word1))
                    word1.insert(0, word[0])
                    word1.append(word[-1])
                    new_word = new_word + ''.join(word1) + " "
            else:
                new_word = new_word + word + " "
with open((inFile[:-4] + "Scrambled.txt"), "a") as fileOut:
    fileOut.write(new_word + "\n")

new_word = ''



