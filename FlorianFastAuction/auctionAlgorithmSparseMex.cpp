/* 

Mex implementation of Bertsekas' auction algorithm [1] for a very fast
solution of the linear assignment problem.

The implementation is optimised for sparse matrices, e.g. solving a
sparse problem of size 950,000 by 950,000 with around 40,000,000 non-zero
elements takes less than 8 mins. The method is also efficient for dense 
matrices, e.g. it can solve a 20,000 by 20,000 problem in less than 3.5
mins.

[1]	Bertsekas, D.P. 1998. Network Optimization: Continuous and Discrete
    Models. Athena Scientific.

Implementation by Florian Bernard ( f.bernardpi [at] gmail [dot] com ).

The author would like to thank Gary Guangning Tan for helpful feedback.

Last modified on 15/12/2016

*/

#include <cmath>
#include <matrix.h>
#include <mex.h>
#include <float.h> 

#include <list>
#include <vector>
#include <numeric>
#include <limits>
#include <unordered_map>
#include <utility>
#include <set>

#if defined (_WIN32)
    #include <windows.h>
#elif defined (__linux__)
    #include <unistd.h>
#endif

#ifdef __cplusplus 
    extern "C" bool utIsInterruptPending();
#else
    extern bool utIsInterruptPending();
#endif

struct PersonData {
	int personIdx;
	int currentUnassignedIdxListIdx;
	std::list<int>::iterator currentUnassignedIdxListIterator;
};

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	// input data
	const mxArray* sparseA = prhs[0];
	double epsilon = *(mxGetPr(prhs[1]));
	double epsilonDecreaseFactor = *(mxGetPr(prhs[2]));
	double largeNumber = *(mxGetPr(prhs[3]));
	int verbosity = (int) *(mxGetPr(prhs[4]));
	

	mwIndex n = (mwIndex)mxGetN(sparseA);
	const mwIndex *ic = mxGetIr(sparseA);
	const mwIndex *jr = mxGetJc(sparseA);
	double *sparseAPr = mxGetPr(sparseA);

	if ( verbosity >= 1 ) {
	  mexPrintf("epsilon = %f, epsilonDec = %f, n = %d\n\n", epsilon, epsilonDecreaseFactor, n);
	}
  
	// output data
	plhs[0] = mxCreateDoubleMatrix(n, 1, mxREAL);
	plhs[1] = mxCreateDoubleMatrix(n, 1, mxREAL); 
	double *prices = (double*) mxGetData(plhs[1]);
	double *assignments = (double*) mxGetData(plhs[0]);

	for ( int i = 0 ; i < n ; ++i ) {
		prices[i] = 0;
		assignments[i] = -1;
	}

	std::list<int>::iterator listIterator;
	std::unordered_map<int, std::list<PersonData> >::iterator objectJIt;
	std::list<PersonData>::iterator personIIt;
	
	std::unordered_map<int,int> objectToAssignmentIndex; // map that knows for each object the index in the 
	objectToAssignmentIndex.reserve(n);

	int nIter = 0;
	bool solutionFeasible = 1;

	while ( epsilon >= 1 ) {
		std::list<int> unassignedIdxList;
		
		for ( int i = 0 ; i < n ; ++i ) { 
			unassignedIdxList.push_back(i); 
		} // reset unassigned idx

		while ( !unassignedIdxList.empty() && solutionFeasible ) {
			int idx = 0;
			std::vector<int> viIdx(unassignedIdxList.size()); // j_i
			std::vector<int> viIdxInSparseA(unassignedIdxList.size()); 
			std::vector<double> vi(unassignedIdxList.size(), NAN);
			std::vector<double> wi(unassignedIdxList.size(), -largeNumber*(nIter+1));
			std::vector<double> bids(unassignedIdxList.size());
			
			std::unordered_map<int, std::list<PersonData> > personsWhoGaveBidForJ;

			solutionFeasible = 1; // we always assume that we have a feasible solution as long as we have not proven the opposite

			// bidding phase
			for ( listIterator = unassignedIdxList.begin(); listIterator != unassignedIdxList.end(); ++listIterator, ++idx) {
				// *listIterator corresponds to i
				mwIndex colStart = jr[*listIterator]; 
				mwIndex colEnd   = jr[*listIterator+1]; 

				// find best and second best objects
				PersonData bestPersonData;

				bool firstIterationForIdx = 1;
				for (mwIndex ci = colStart; ci < colEnd; ++ci)  {
					mwIndex j = ic[ci]; // corresponds to j
					double aijMinusPj = sparseAPr[ci] - prices[j];
				
					if ( firstIterationForIdx || aijMinusPj > vi[idx] ) {
						if ( !firstIterationForIdx ) {
							wi[idx] = vi[idx]; // make previous best value the second best value
						} 
						viIdx[idx] = j;
						viIdxInSparseA[idx] = ci;
						vi[idx] = aijMinusPj;

						bestPersonData.personIdx = *listIterator;
						bestPersonData.currentUnassignedIdxListIdx = idx;
						bestPersonData.currentUnassignedIdxListIterator = listIterator;

						firstIterationForIdx = 0;
					} else if ( aijMinusPj > wi[idx] ) {
						wi[idx] = aijMinusPj;
					}
				}
				if ( std::isnan(vi[idx]) ) {
					// nan means that for one person there is no best object, i.e. this person is not allowed to get assigned any object
					solutionFeasible = 0;
					break;
				}

				personsWhoGaveBidForJ[viIdx[idx]].push_back(bestPersonData);

				// compute bids
				bids[idx] = sparseAPr[viIdxInSparseA[idx]] - wi[idx] + epsilon;
				

				if ( verbosity >= 3) {
					mexPrintf("BIDDING: person %d, viIdx = %d, vi = %f, wi = %f, bids[%d] = %f\n", 
						*listIterator, viIdx[idx], vi[idx], wi[idx], *listIterator, bids[idx]);
				}
			}

			// ... end bidding phase
	
			// assignment phase
			std::list<int> unassignedIdxListTmp;
			for ( objectJIt = personsWhoGaveBidForJ.begin(); objectJIt != personsWhoGaveBidForJ.end(); ++objectJIt) {
				int objectJ = objectJIt->first;

				// set prices[j] to highest bid
				int personWithHighestBid;
				std::list<int>::iterator itPersonWithHighestBid;

				bool firstIterationForObject = 1;
				for (  personIIt = (objectJIt->second).begin() ; personIIt != (objectJIt->second).end() ; ++personIIt ) {
					if ( firstIterationForObject || bids[personIIt->currentUnassignedIdxListIdx] >= prices[objectJ] ) {
						prices[objectJ] = bids[personIIt->currentUnassignedIdxListIdx];
						personWithHighestBid = personIIt->personIdx;
						itPersonWithHighestBid = personIIt->currentUnassignedIdxListIterator;

						firstIterationForObject = 0;
					}
				}

				// update assignments
				if ( objectToAssignmentIndex.count(objectJ) && ((int) assignments[objectToAssignmentIndex[objectJ]]) == objectJ ) {
					const int asgIdxOfObjectJ = objectToAssignmentIndex[objectJ];

					assignments[asgIdxOfObjectJ] = -1; // remove previous assignment
					if ( asgIdxOfObjectJ != personWithHighestBid ) {
						unassignedIdxListTmp.push_back(asgIdxOfObjectJ);
					}
				}

				assignments[personWithHighestBid] = objectJ;
				objectToAssignmentIndex[objectJ] = personWithHighestBid;

				unassignedIdxList.erase(itPersonWithHighestBid);

				if ( verbosity >= 3) {
					mexPrintf("ASSIGNMENT: object %d, personWithHighestBid = %d, prices[%d] = %f\n", 
						objectJ, personWithHighestBid, objectJ, prices[objectJ]);
				}
			}
			unassignedIdxList.splice(unassignedIdxList.end(), unassignedIdxListTmp);
			// ... end assignment phase

			if ( verbosity >= 2 ) {
				mexPrintf("nIter = %d, epsilon = %f, nUnassigned = %d\n", nIter, epsilon, unassignedIdxList.size());
				mexPrintf("\n");
			}
			nIter++;

			if ( utIsInterruptPending() ) {
		    	mexPrintf("CTRL-C detected END\n\n");
		    	return;
		    }
		}
		epsilon *= epsilonDecreaseFactor;
	}

	for ( int i = 0 ; i < n ; ++i ) { 
		if ( solutionFeasible ) {
			assignments[i]++; 
		} else {
			assignments[i] = -1;
		}
	} // convert c to matlab idx
}

