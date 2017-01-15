#!/bin/bash
rm -r -f tmp
rm -f status.txt log.txt
mkdir tmp
python download.py > log.txt
