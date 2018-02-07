#!usr/bin/env python2.7
import os
import string

os.system("curl -L https://twitter.com/i/streams/category/686639666779394057?lang=en | grep 'href=\"https' > test.txt")

with open("test.txt","r+") as stuff:
    urls = list()
    for line in stuff:
        temp = line.split("href=\"")
        temp1 = temp[1]
        temp2 = temp1.split("\"")
        url = temp2[0]
#        print(url)
        urls.append(url)
    i = 0
    while (i < 100):
        os.system("rm test.txt")
        command = "curl -L {} | grep 'href=\"https' > test.txt".format(urls[i])
        os.system(command)
        with open("test.txt","r+") as stuff:
            for line in stuff:
                temp = line.split("href=\"")
                temp1 = temp[1]
                temp2 = temp1.split("\"")
                url = temp2[0]
                if url not in urls:
#                    print(url)
                    urls.append(url)
        i=i+1
    for things in urls:
        print(things)
