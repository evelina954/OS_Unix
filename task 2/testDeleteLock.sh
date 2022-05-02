#!/bin/bash
rm -f somefile.lck stats results.txt
make -s lock
./lock &
sleep 1
rm -f somefile.lck
wait
exit
