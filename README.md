# WindRosePOC
CMPE275 : Project1 Submission Repository
```
-Actual data size: ~200GB not included in the folder.
-Sample data size: ~5GB included in the project Data folder.
-Performance tested on: 2.5GHz dual-core Intel Core i5 processor (Turbo Boost up to 3.1GHz) with 3MB L3 cache
-Instructions to Run the Project:
   1.Clone the repo
   2.cd /WindRosePOC
   3.cd src_MPI : To run various algorithms using MPI techniques.
   4.cd src_OpenMP : To run various algorithms using openMP techniques.
```
```   
Information about source files:
Source folder: src_MPI
   1.Serial.cpp : Procedural serial code with the usage of local containers to store intermediary data in vectors and 2Darrays. 
      * g++ -std=c++1y Serial.cpp
      * ./a.out
      * Observation : Takes the longest time to process the files. (~60 seconds)
   2.SerialModified.cpp : Procedural serial code without the usage of local containers to store intermediary data.
      * g++ -std=c++1y SerialModified.cpp
      * ./a.out
      * Observation : Takes considerably less time to process the files. (~15 seconds for two station ids)
   3.MPITaskDivision.cpp : Parallelized code using OpenMPI Send and receive prototypes that spawns two processes 
                           to process data for two station ids.
      * mpic++ -std=c++1y MPITaskDivision.cpp
      * mpirun -np 2 a.out
      * Observation : Gives 2x performance when compared to the Serial code. (~8 seconds for two station ids)
   4.MP+MPI.cpp : Hybrid parallelized code optimized using Open MPI send and receive prototypes and openMP parallel for 
                  pragma directives.
      * mpic++ -fopenmp -std=c++1y MP+MPI.cpp
      * mpirun -np 3 a.out
      * Observation : Gives 2x performance. (~7 seconds)

Instructions for compiling OpenMP code:
   1.g++ -fopenmp -std=c++11 Parallel01_01.cpp -o Parallel01_01
   2.To run : ./Parallel01_01
```
