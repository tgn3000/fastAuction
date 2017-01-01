// Copyright 2016 <Copyright Guangning Tan>

#ifndef MISC_H_
#define MISC_H_

#include <stdio.h>
#include <cassert>

void readFile( char* filename, int& id, int& n, int& nnz,  // input
               int*& ic, int*& jr, double*& Cx );          // output

void fillMatrix( const int p, const int pattern[], const int nnz, double* Cx );

double computeCost( int n, int* ic, int* jr, double* Cx, int* col_sol );

double getCPUwithETIME();

void convertIndices( int n, int arc_count, int* row_ind, int* col_ind );

template <typename T>
void printArray( int array_size, T* array ) {
    printf( "\n" );
    for ( int i = 0; i < array_size; i++ )
        printf( "%i  ", static_cast<int>( array[ i ] ) );
    printf( "\n" );
}

#endif  // MISC_H_
