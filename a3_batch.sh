#!/bin/sh -login
WAIT=5;
for n in 100 1000 10000 100000 1000000;
#for n in 100;
do
	qsub -l nodes=1:ppn=1 -F "-l -n $n $n" a3a.pbs;
	sleep $WAIT;
	for no_threads in 1 2 4 8 16 32;
	#for no_threads in 1 2 4;
	do
		qsub -l nodes=1:ppn=$no_threads -F "-p -n $n $n $no_threads" a3b.pbs;
		sleep $WAIT;
	done
	qsub -l nodes=1:ppn=1 -F "-q -n $n $n" a3a.pbs
	sleep $WAIT;
done
