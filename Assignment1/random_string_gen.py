import random
import string

lst = list(string.ascii_lowercase)
lst1 = list(string.ascii_uppercase)
file = open("bigRandomShit.txt","w")
lst+=lst1
for i in range(100000000):
    random.shuffle(lst)
    st = ''.join(lst)
    file.write(st+" ")

file.close()
