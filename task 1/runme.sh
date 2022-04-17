#!/bin/bash
make -s
python3 createA.py
./sparse fileA fileB
gzip -kf fileA
gzip -kf fileB
gzip -cd fileB.gz | ./sparse fileC
./sparse fileA fileD -b 100

blockSize=$(stat -c %B fileA)
fileBlocks=$(stat -c %b fileA)
echo fileA physical size = $((blockSize*fileBlocks))
fileBlocks=$(stat -c %b fileB)
echo fileB physical size = $((blockSize*fileBlocks))
fileBlocks=$(stat -c %b fileA.gz)
echo fileA.gz physical size = $((blockSize*fileBlocks))
fileBlocks=$(stat -c %b fileB.gz)
echo fileB.gz physical size = $((blockSize*fileBlocks))
fileBlocks=$(stat -c %b fileC)
echo fileC physical size = $((blockSize*fileBlocks))
fileBlocks=$(stat -c %b fileD)
echo fileD physical size = $((blockSize*fileBlocks))