#!/bin/bash
rm -r -f tmp
rm -f main sqlite3.o status.txt log.txt
mkdir tmp
python download.py > log.txt
