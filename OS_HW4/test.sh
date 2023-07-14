#!/bin/bash

for ((i=1; i<=10; i++))
do
    echo "Execution $i"

    ./single -m 10000 -t 4 ./test
    
    echo "Execution $i complete"
    echo
done
