# C++ implementation of auction algorithm for solving sparse linear assignment problems (LAPs)

This is the C++ interface of Florian Bernard's original MEX code [1], which implements Bertsekas's auction algorithm [2] for sparse LAPs.

We illustrate the usages of both C++ and MEX codes by solving the sparse matrices that we construct using the SuiteSparse Matrix Collection (formerly known as the University of Florida Sparse Matrix Collection). To solve these sample LAPs, please follow the following steps.

(1) Download UFget from the website of [the SuiteSparse Matrix Collection](http://www.cise.ufl.edu/research/sparse/matrices/) and add UFget to Matlab's search path.

(2) To solve our sample LAPs using Florian's MEX code, run 

    cd Matlab
    main

This file writes the sparsity pattern of each sample matrix into a text file 

    Data/UFmat_XXXX.dat 
    
(XXXX is the four-digit matrix ID), builds Florian's implementation auctionAlgorithmSparseMex.cpp into a MEX function, and calls it to solve our sample LAPs.

(3) Having produced the LAP text files in Data directory, we can run the C++ code by

	cd src
	make run
	
and solve these LAPs in Terminal.

To cite this code, please use [3].

### References

[1] Bernard, F. 2016. Fast Linear Assignment Problem using Auction Algorithm (MEX) v2.4. https://www.mathworks.com/matlabcentral/fileexchange/48448

[2] Bertsekas, D.P. 1998. Network Optimization: Continuous and Discrete Models. Athena Scientific.

[3] Bernard, F., Tan, G. 2017. A C++ implementation of Bertsekas's Auction Algorithm for solving sparse linear assignment problems (2017). C++ implementation at https://github.com/tgn3000/fastAuction. Bernard's original MEX implementation at http://www.mathworks.com/matlabcentral/fileexchange/48448
 

