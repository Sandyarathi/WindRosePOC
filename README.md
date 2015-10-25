# WindRosePOC
CMPE275 : Project1 Submission Repository

Instructions for compiling MPI codes:


* g++ -std=c++1y Serial.cpp
    * ./a.out
* mpic++ -std=c++1y MPI.cpp
   *  mpirun -np 3 a.out
* mpic++ -std=c++1y ParallelWindroseReduce.cpp
   * mpirun -np 3 a.out
