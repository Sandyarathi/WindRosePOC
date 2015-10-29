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


Description of OpenMP Files:
src_OpenMP/Serial01.cpp : Algorithm# 1 Storing Filtered Raw Data in a Local Container 
src_OpenMP/Parallel01_01.cpp : Strategy# 1 Parallelising data processing using local containers
src_OpenMP/Parallel01_02.cpp : Strategy# 2 Parallelising data read and processing
src_OpenMP/Parallel01_03.cpp : Strategy# 3 Parallelising data processing by accessing elements according to the fixed strip width
src_OpenMP/Serial02.cpp : Algorithm# 2  Without Using a Local Container for Storing Input Raw Data
src_OpenMP/Parallel02_01: Strategy# 1 Parallelising data read and processing using 2-D local containers
src_OpenMP/Parallel02_02: Strategy# 2 Parallelising data read and processing using 3-D shared container

Instructions for compiling OpenMP code:

   1.g++ -fopenmp -std=c++11 Parallel01_01.cpp -o Parallel01_01.O
   2.To run : ./Parallel01_01.O
   3.Example station ids to give at run time : "E3467", "A28";
```
