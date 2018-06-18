#!/bin/sh
PATH=/usr/local/bin:/usr/bin:/bin:
find . -type f -name \*.c | sed 's/.c$/.o/g'
