#!/usr/bin/env bash
NAME="results.csv";
echo "Compiling results from batch";
cat a3*.pbs.o* >> $NAME;
echo "Cleaning up batch";
rm a3*.pbs.o*;