/* Copyright 2016 <Copyright Guangning Tan> */

#ifndef AUCTION_H_
#define AUCTION_H_

#include <float.h>
#include <cmath>

#ifdef MATLAB_MEX_FILE
#include <matrix.h>
#include <mex.h>
#endif

#include <limits>
#include <list>
#include <numeric>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

#include <algorithm>  // max
#include <cassert>    // assert

// Florian's macros
#ifdef MATLAB_MEX_FILE
#if defined( _WIN32 )
#include <windows.h>
#elif defined( __linux__ )
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" bool utIsInterruptPending();
#else
extern bool utIsInterruptPending();
#endif
#else
#include <stdio.h>
typedef int mwIndex;

void auction( int matrix_size, int* ic, int* jr, double* Cx, /* input  */
              double* prices, int* col_sol,                  /* output */
              int verbosity = 0 );                           /* debug  */
#endif

struct PersonData {
    int                      personIdx;
    int                      currentUnassignedIdxListIdx;
    std::list<int>::iterator currentUnassignedIdxListIterator;
};

#endif  // AUCTION_H_
