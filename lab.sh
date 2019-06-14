#!/bin/bash

make -j 4 > /dev/null
./optim > /dev/null
python3 plot.py
python3 plot.py -t
