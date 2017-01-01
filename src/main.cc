/* Copyright 2016 <Copyright Guangning Tan> */

#include <string.h>
#include <algorithm>

#include "auction.h"
#include "misc.h"

#define NUM_REPETITION 1

const int p         = 7;
const int pattern[] = {2, 1, 0, 1, 1, 0, 2};

int main( int argc, char *argv[] ) {
    int     id, matrix_size, nnz;
    int *   row_ind, *col_ind, *row_sol, *col_sol;
    double *sig_entry;
    double  cost_csa, cost_verify, cost_auction;

    /* read in matrix */
    int *   ic = NULL, *jr = NULL;
    double *Cx = NULL;
    readFile( argv[ 1 ], id, matrix_size, nnz, ic, jr, Cx );
    int arc_count = jr[ matrix_size ];

    /* allocate memory */
    row_ind   = (int *)malloc( arc_count * sizeof( int ) );
    col_ind   = (int *)malloc( arc_count * sizeof( int ) );
    sig_entry = (double *)malloc( arc_count * sizeof( double ) );
    row_sol   = (int *)malloc( matrix_size * sizeof( int ) );
    col_sol   = (int *)malloc( matrix_size * sizeof( int ) );

    /* timing */
    double auction_cputime, auction_cputime_total = 0.0;

    /* fill entries in matrix */
    fillMatrix( p, pattern, nnz, Cx );

    /* Auction algorithm */
    for ( int i = 0; i < NUM_REPETITION; i++ ) {
        auction_cputime = getCPUwithETIME();
        auction( matrix_size, ic, jr, Cx, sig_entry, col_sol );
        auction_cputime_total += getCPUwithETIME() - auction_cputime;

        cost_auction = computeCost( matrix_size, ic, jr, Cx, col_sol );
    }

    //    free(col_sol);
    //
    //    /* CSA without BTF */
    //    /* pre-process input */
    //    for( int i=0; i<matrix_size; i++ )
    //        std::fill( col_ind+jr[i], col_ind+jr[i+1], i+1+matrix_size );
    //    memmove( row_ind, ic, arc_count * sizeof(int) );
    //    for( int i=0; i<arc_count; i++ )
    //        row_ind[i]++;
    //    memmove( sig_entry, Cx, arc_count * sizeof(double) );
    //    for( int i=0; i<NUM_REPETITION; i++ )
    //    {
    //        /* solve lap with row and column SWAPPED */
    //        CSA_Q lap( matrix_size, arc_count, row_ind, col_ind, sig_entry );
    //        lap.Solve();
    //
    //        memmove( row_sol, lap.row_sol_, matrix_size * sizeof(int) );
    //        cost_csa = -lap.cost_;
    //        csa_cputime_total  += lap.cputime_ + lap.parse_time_;
    //
    //        for( int j=0; j<matrix_size; j++ )
    //            row_sol[j] -= matrix_size+1;
    //
    //        col_sol = cs_pinv( row_sol, matrix_size );
    //        cost_verify = ComputeCost( C, col_sol );
    //    }
    //    assert( cost_verify  == cost_csa );
    //    assert( cost_auction == cost_csa );
    //
    //    /* post-process timing results */

    //    csa_cputime  = csa_cputime_total/NUM_REPETITION;
    //
    auction_cputime = auction_cputime_total / NUM_REPETITION;
    FILE *outfile   = fopen( "AUCTION_result.txt", "a" );
    fprintf( outfile, "%i\t%i\t%i\t%i\t%e\n", id, matrix_size, arc_count,
             (int)cost_auction - matrix_size, auction_cputime );
    printf( "%i\t%i\t%i\t%i\t%e\n", id, matrix_size, arc_count,
            (int)cost_auction - matrix_size, auction_cputime );
    fclose( outfile );

    /* finalize, free memory */
    free( col_sol );
    free( row_sol );
    free( sig_entry );
    free( col_ind );
    free( row_ind );
    free( ic );
    free( jr );
    free( Cx );

    return 0;
}
