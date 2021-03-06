#!/bin/bash
set -eu
cd "$(dirname $0)"

make -sj16 clean
make -sj16 all

for DAY in build/day[0-9] build/day[0-9][0-9]; do
    INPUT="${DAY:6}.txt"
    echo "==== ${DAY:6} ===="
    $DAY < $INPUT | tee -a build/reference.txt
done

echo "==== DIFF ===="
diff -u build/reference.txt run_all.txt
