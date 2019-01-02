#!/usr/bin/env python

import sys

BAR = '\n\n' + ('-' * 74) + '\n\n'

for f in sys.argv:
    inman = False
    for line in open(f):
        if inman:
            if line.startswith('**man-end'):
                inman = False
                sys.stdout.write(BAR)
            else:
                sys.stdout.write(line)
        elif line.startswith('/*man-start*'):
            inman = True
