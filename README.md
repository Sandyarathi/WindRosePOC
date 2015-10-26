# WindRosePOC
CMPE275 : Project1 Submission Repository

Instructions for compiling MPI codes:


* g++ -std=c++1y Serial.cpp
    * ./a.out
* mpic++ -std=c++1y MPI.cpp
   *  mpirun -np 3 a.out
* mpic++ -std=c++1y ParallelWindroseReduce.cpp
   * mpirun -np 3 a.out


instructions for compiling OpenMP code:

* g++ -fopenmp -std=c++11 Parallel01_01.cpp -o Parallel01_01

To run,

 * ./Parallel01_01.o
