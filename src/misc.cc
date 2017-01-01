// Copyright 2016 <Copyright Guangning Tan>

// getCPUwithETIME
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

// readFile
#include <fstream>

// fillMatrix
//#define USE_RAND_NUM
#ifdef USE_RAND_NUM
#include <cstdlib>  // rand, srand
#include <ctime>
#endif

#include "misc.h"

void readFile( char* filename, int& id, int& n, int& nnz, /* input */
               int*& ic, int*& jr, double*& Cx ) {
    std::ifstream infile( filename );

    /* Read matrix ID, size, and number of elements */
    infile >> id;
    infile >> n;
    infile >> nnz;

    /* Read matrix in column compressed form */
    ic = new int[ nnz ];
    jr = new int[ n + 1 ];
    Cx = new double[ nnz ];

    for ( int k = 0; k < n + 1; k++ )
        infile >> jr[ k ];

    for ( int k = 0; k < nnz; k++ )
        infile >> ic[ k ];

    infile.close();

    printf( "n = %i, nnz = %i\n", n, nnz );
}

void fillMatrix( const int p, const int pattern[], const int nnz,  // input
                 double* Cx )                                      // output
{
#ifdef USE_RAND_NUM
    srand( (unsigned)time( 0 ) );
    for ( int i = 0; i < nnz; i++ )
        Cx[ i ] = ( rand() % 100 ) + 1.0;

#else
    for ( int i = 0; i < nnz; i++ )
        Cx[ i ] = pattern[ i % p ] + 1.0;
#endif
}

double computeCost( int matrix_size, int* ic, int* jr, double* Cx,
                    int* col_sol ) {
    double cost = 0.0;

    for ( int j = 0; j < matrix_size; j++ )
        for ( int C_ind = jr[ j ]; C_ind < jr[ j + 1 ]; C_ind++ )
            if ( ic[ C_ind ] == col_sol[ j ] )
                cost += Cx[ C_ind ];

    return cost;
}

double getCPUwithETIME() {
    struct rusage  usage;
    struct timeval time;

    getrusage( RUSAGE_SELF, &usage );
    time = usage.ru_utime;
    return time.tv_sec + time.tv_usec / 1e6;
}

void convertIndices( int matrix_size, int arc_count,  // input
                     int* row_ind, int* col_ind )     // output
{
    for ( int i = arc_count - 1; i >= 0; --i ) {
        row_ind[ i ]++;
        col_ind[ i ] += 1 + matrix_size;
    }
}
