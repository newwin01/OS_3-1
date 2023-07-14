#!/bin/bash

echo "===Compile source code==="
gcc cimin.c -o cimin

if [ $? -eq 0 ]
then
    echo "compile worked!"
    echo "cimin binary file created"
    echo "You can now test crash input minimizer with the following options"
    echo "i: crashing input | m: error looking for | o: file to store the result"
    echo "put binary file and options after previous options"
else
    echo "compile failed..."
    echo "please check the directory"
fi

echo "=============================="
