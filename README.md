# fastAuction
This is the C++ interface of Florian Bernard's original MEX implementation [1] of Bertsekas's auction algorithm [2] for sparse linear assignment problems (LAPs).

We illustrate the usage of both C++ and MEX codes on some sparse matrices we derived from the SuiteSparse Matrix Collection (formerly known as the University of Florida Sparse Matrix Collection). Please follow the following steps.

(1) Download UFget from its website

	http://www.cise.ufl.edu/research/sparse/matrices/
	
and add UFget to Matlab's search path.

(2) To solve our sample LAPs using Bernard's MEX code, run main.m in directory "Matlab". 

    This file writes the sparsity pattern of each sample matrix into a text file in directory "Data", with file name "UFmat_XXXX.dat" where XXXX is the four digit of the matrix ID.

(3) Having produced the LAP text files in directory "Data", we can run the C++ code by simply typing "make run" in directory "src".

To cite this code, please use [3].

References

[1] Bernard, F. 2016. Fast Linear Assignment Problem using Auction Algorithm (MEX) v2.4. https://www.mathworks.com/matlabcentral/fileexchange/48448

[2] Bertsekas, D.P. 1998. Network Optimization: Continuous and Discrete Models. Athena Scientific.

[3] Bernard, F., Tan, G. 2017. A C++ implementation of Bertsekas's Auction Algorithm for solving sparse linear assignment problems (2017). C++ implementation at https://github.com/tgn3000/fastAuction. Bernard's original MEX implementation at http://www.mathworks.com/matlabcentral/fileexchange/48448
 

