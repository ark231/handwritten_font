#!/usr/bin/env python3
import sys
argv = sys.argv
argc = len(argv)
if argc<4:
    print("error: not enough argument")
    exit(1)
for i in range(int(argv[1],0),int(argv[2],0)+1):
    print(argv[3]+' U+'+format(i,"04x")+" #'{char}'".format(char=chr(i)))
