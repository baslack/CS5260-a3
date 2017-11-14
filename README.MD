Benjamin A. Slack
CS5260
Parallel Count Sort (and Miscalena)
11.14.17

PROG: a33
DESCRIPTION:
The source code implments a command line interface to test
three sorting algorithms: serial count sort, parallel count
sort and the standard library quick sort. It generates arrays
of randomized integers and performs iterative sorts on copies
of the array which it times and reports averages of. The program
can be configured via several command line options:

USAGE:
a33 [-q|-p|-l] (default: -p, parallel count sort)
	-n <int: length of array>  (default: 10)
	-h <int: high value of array elements>  (default: 100)
	-t <int: number of threads> (default: 4)
	-i <int: number of iterations> (default: 10)
	-s <int: seed for random values> (default: time(NULL))

	-q perform a quicksort
	-p perform a parallel count sort
	-l perform a serial count sort

PROG: a32, a31
DESCRIPTION:
In addition to the a33 program, there are two scheduling tests
proscribed by the asssignment. a31 uses default scheduling for
the OMP FOR directive. a32 uses static scheduling with a chunk
size of 4.

USAGE:
a31, a32
	-t <int: number of threads>
	-i <int: number of iterations>

COMPILING:
All programs in this package were compiled using CMake 3.8 and
gcc. Users familar with CMake can configure a tool chain for
their respective platforms.

In addition, a simple custom make file (Makefile) is supplied
in the root directory of the package. Running "make" in that
directory will build all three applications as well.