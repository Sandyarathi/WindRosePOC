%module windrose


%{

#define SWIG_FILE_WITH_INIT
#include "myHeader.h"
#include "omp.h"


void callFunction();


%}

extern void callFunction();

