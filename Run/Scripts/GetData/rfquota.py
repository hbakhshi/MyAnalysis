#!/usr/bin/python
import sys
import os

def ndlist1 (dir):
#    f_0 = os.popen("rfdir " + dir + " | grep root")
    f_0 = os.popen("rfdir " + dir )
    n = 0
    for i in f_0.readlines():
        j = i.split()
        n = n + int(j[4])
    
    f_0 = os.popen("rfdir " + dir)
    d = []
    for i in f_0.readlines():
        if i[0] == "d":
            j = i.split()
            d1 = dir + "/" + j[-1]
            d.append(d1)

    for j in d:
        n1,d1 = ndlist1(j);
        n = n + n1;

    size = n / (1024*1024*1024);
    size = size + (n % (1024.0*1024*1024)) / (1024.0*1024*1024) ;
    print str(size) + " GB\t---" + dir
    return n, d

if __name__ == "__main__":
    dir = sys.argv[1]
    n,d = ndlist1(dir)

    size = n / (1024*1024*1024);
    size = size + (n % (1024.0*1024*1024)) / (1024.0*1024*1024) ;

    print str(size) + " GB"
