/* Copyright 2017 <Copyright Guangning Tan, Florian Bernard>

 C++ interface of Florian Bernard's original MEX implementation [1] of Bertsekas' auction algorithm [2] for sparse linear assignment problems (LAPs).
 
 [1] Bernard, F. 2016. Fast Linear Assignment Problem using Auction Algorithm (MEX) v2.4. https://www.mathworks.com/matlabcentral/fileexchange/48448-fast-linear-assignment-problem-using-auction-algorithm--mex-
 [2] Bertsekas, D.P. 1998. Network Optimization: Continuous and Discrete Models. Athena Scientific.

 Original MEX implementation by Florian Bernard ( f.bernardpi [at] gmail [dot] com )
 Modification by Guangning (Gary) Tan ( tgn [at] mit [dot] edu )

 Created on 1 Jan 2017

 */

#include "auction.h"

void auction( int n, int *ic, int *jr, double *Cx,  // input
              double *prices, int *assignments,     // output
              int verbosity )                       // debug
{
    // input data
    double *sparseAPr = new double[ jr[ n ] ];
    // Use default setting in Florian's sparseAssignmentProblemAuctionAlgorithm.m
    double epsilon, epsilonDecreaseFactor;

    double maxAbsA = 0.0, n_plus_one = n + 1.0, scaledMaxAbsA;
    for ( int i = 0; i < jr[ n ]; i++ ) {
        sparseAPr[ i ] = Cx[ i ] * n_plus_one;
        maxAbsA        = std::max( maxAbsA, fabs( Cx[ i ] ) );
    }
    scaledMaxAbsA         = maxAbsA * n_plus_one;
    epsilon               = std::max( scaledMaxAbsA / 50.0, 1.0 );
    epsilonDecreaseFactor = 0.2;

    if ( verbosity >= 1 ) {
        printf(
            "UnscaledMaxAbsA = %f, MaxAbsA = %f, epsilon = %e, epsilonDec = "
            "%e, size = %i, nnz = %i\n",
            maxAbsA, scaledMaxAbsA, epsilon, epsilonDecreaseFactor, n,
            jr[ n ] );
    }

    // output
    for ( int i = 0; i < n; ++i ) {
        prices[ i ]      = 0;
        assignments[ i ] = -1;
    }

    std::list<int>::iterator listIterator;
    std::unordered_map<int, std::list<PersonData>>::iterator objectJIt;
    std::list<PersonData>::iterator personIIt;
    // map that knows for each object the index in the objectToAssignmentIndex
    std::unordered_map<int, int> objectToAssignmentIndex;
    objectToAssignmentIndex.reserve( n );

    int nIter = 0;

    while ( epsilon >= 1.0 ) {
        std::list<int> unassignedIdxList;
        for ( int i = 0; i < n; ++i ) {
            unassignedIdxList.push_back( i );  // reset unassigned idx
        }

        while ( !unassignedIdxList.empty() ) {
            int                 idx = 0;
            std::vector<int>    viIdx( unassignedIdxList.size() );  // j_i
            std::vector<int>    viIdxInSparseA( unassignedIdxList.size() );
            std::vector<double> vi( unassignedIdxList.size(), NAN );
            std::vector<double> wi( unassignedIdxList.size(),
                                    -scaledMaxAbsA * ( 2 * n * nIter + 1 ) );
            std::vector<double> bids( unassignedIdxList.size() );

            std::unordered_map<int, std::list<PersonData>>
                personsWhoGaveBidForJ;

            // bidding phase
            for ( listIterator = unassignedIdxList.begin();
                  listIterator != unassignedIdxList.end();
                  ++listIterator, ++idx ) {
                // *listIterator corresponds to i
                mwIndex colStart = jr[ *listIterator ];
                mwIndex colEnd   = jr[ *listIterator + 1 ];

                // find best and second best objects
                PersonData bestPersonData;

                bool firstIterationForIdx = 1;
                for ( mwIndex ci = colStart; ci < colEnd; ++ci ) {
                    mwIndex j          = ic[ ci ];  // corresponds to j
                    double  aijMinusPj = sparseAPr[ ci ] - prices[ j ];
                    if ( firstIterationForIdx || aijMinusPj > vi[ idx ] ) {
                        if ( !firstIterationForIdx ) {
                            wi[ idx ] = vi[ idx ];  // make previous best value
                            // the second best value
                        }
                        viIdx[ idx ]          = j;
                        viIdxInSparseA[ idx ] = ci;
                        vi[ idx ]             = aijMinusPj;

                        bestPersonData.personIdx = *listIterator;
                        bestPersonData.currentUnassignedIdxListIdx = idx;
                        bestPersonData.currentUnassignedIdxListIterator =
                            listIterator;

                        firstIterationForIdx = 0;
                    } else if ( aijMinusPj > wi[ idx ] ) {
                        wi[ idx ] = aijMinusPj;
                        assert( !std::isinf( wi[ idx ] ) );
                    }
                }
                if ( std::isnan( vi[ idx ] ) ) {  // || vi[idx] <
                    // -(2.0*n-1.0)*maxAbs -
                    // (n-1.0)*epsilon ) {
                    // nan means that for one person there is no best object,
                    // i.e. this person is not allowed to get assigned any
                    // object
                    break;
                }

                personsWhoGaveBidForJ[ viIdx[ idx ] ].push_back(
                    bestPersonData );

                // compute bids
                bids[ idx ] =
                    sparseAPr[ viIdxInSparseA[ idx ] ] - wi[ idx ] + epsilon;

                if ( verbosity >= 3 ) {
                    printf(
                        "BIDDING: person %d, viIdx = %d, vi = %f, wi = %f, "
                        "bids[%d] = %f\n",
                        *listIterator, viIdx[ idx ], vi[ idx ], wi[ idx ],
                        *listIterator, bids[ idx ] );
                }
                assert( !std::isinf( wi[ idx ] ) );
            }
            // ... end bidding phase

            // assignment phase
            std::list<int> unassignedIdxListTmp;
            for ( objectJIt = personsWhoGaveBidForJ.begin();
                  objectJIt != personsWhoGaveBidForJ.end(); ++objectJIt ) {
                int objectJ = objectJIt->first;
                // set prices[j] to highest bid
                int                      personWithHighestBid;
                std::list<int>::iterator itPersonWithHighestBid;

                // prices[objectJ] = -1; // reset price for current bet
                bool firstIterationForObject = 1;
                for ( personIIt = ( objectJIt->second ).begin();
                      personIIt != ( objectJIt->second ).end(); ++personIIt ) {
                    if ( firstIterationForObject ||
                         bids[ personIIt->currentUnassignedIdxListIdx ] >=
                             prices[ objectJ ] ) {
                        prices[ objectJ ] =
                            bids[ personIIt->currentUnassignedIdxListIdx ];
                        personWithHighestBid = personIIt->personIdx;
                        itPersonWithHighestBid =
                            personIIt->currentUnassignedIdxListIterator;

                        firstIterationForObject = 0;
                    }
                }

                // update assignments
                if ( objectToAssignmentIndex.count( objectJ ) &&
                     ( (int)assignments
                           [ objectToAssignmentIndex[ objectJ ] ] ) ==
                         objectJ ) {
                    const int asgIdxOfObjectJ =
                        objectToAssignmentIndex[ objectJ ];

                    assignments[ asgIdxOfObjectJ ] =
                        -1;  // remove previous assignment
                    if ( asgIdxOfObjectJ != personWithHighestBid ) {
                        unassignedIdxListTmp.push_back( asgIdxOfObjectJ );
                    }
                }

                assignments[ personWithHighestBid ] = objectJ;
                objectToAssignmentIndex[ objectJ ]  = personWithHighestBid;

                unassignedIdxList.erase( itPersonWithHighestBid );

                if ( verbosity >= 3 ) {
                    printf(
                        "ASSIGNMENT: object %d, personWithHighestBid = %d, "
                        "prices[%d] = %f\n",
                        objectJ, personWithHighestBid, objectJ,
                        prices[ objectJ ] );
                }
            }
            unassignedIdxList.splice( unassignedIdxList.end(),
                                      unassignedIdxListTmp );
            // ... end assignment phase

            if ( verbosity >= 2 ) {
                printf( "nIter = %d, epsilon = %f, nUnassigned = %d\n\n", nIter,
                        epsilon, (int)unassignedIdxList.size() );
            }
            nIter++;
        }
        epsilon *= epsilonDecreaseFactor;
    }
    delete[] sparseAPr;
}
