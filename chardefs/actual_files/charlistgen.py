#!/usr/bin/env python3
import sys
argv = sys.argv
argc = len(argv)
if argc<5:
    print("error: not enough argument")
    exit(1)
if argv[1] == "--num":
    start = int(argv[2],0)
    end = int(argv[3],0)
elif argv[1] == "--char":
    start = ord(argv[2])
    end = ord(argv[3])
else:
    print("error: unknown option: "+argv[1])
    exit(1)
for i in range(start,end+1):
    print(argv[4]+' U+'+format(i,"04x")+" #'{char}'".format(char=chr(i)))
