clear; close all; clc

%% load Gary's data
Data = xlsread('CSA_results.xlsx');
nMatrix = size(Data,1);

% mex -setup C++
delete([pwd '/*.mexmaci64'])
mex -largeArrayDims ../FlorianFastAuction/auctionAlgorithmSparseMex.cc -lut

%% my filling pattern for experiment
pattern = [2 1 0 1 1 0 2]';
numelPattern = numel(pattern);

%% obtain sparsity pattern by UFget; see
% 1) https://www.cise.ufl.edu/research/sparse/matrices/
% 2) http://www.cise.ufl.edu/research/sparse/mat/UFget.html

for MATi = 1:nMatrix
    %%
    matID = Data(MATi,1);
    CSAwalltime = Data(MATi,9);
    
    prob = UFget(matID);
    A = prob.A;
    n = size(A,1);
    [i,j] = find(A);
    nz = nnz(A);
    Aij = [ repmat(pattern,floor(nz/numelPattern),1)
        pattern(1:mod(nz,numelPattern)) ];
    
	M = sparse(i,j,Aij+1,n,n);
	fprintf('Matrix %i, n = %i, matrix density = %.1e, CSA walltime = %.1e, ',...
		matID, n, nnz(M)/numel(M), CSAwalltime);
    
    % construct matrix
    % call Florian's solver to solve the assignment problem
    verbosity = 0;
	doFeasibilityCheck = 1;
    tic
    [assignments, prices] = ...
        sparseAssignmentProblemAuctionAlgorithm( M, [], [], verbosity, doFeasibilityCheck);
    AuctionWalltime = toc;

    % optimal value
    cost = full(sum(M((1:n)'+n*(assignments-1))))-n;
    fprintf(['Auction walltime = %.1e time factor = %3.2f ', 'Cost = %i\n'], ...
		AuctionWalltime, AuctionWalltime/CSAwalltime, cost);
    
    assert( Data(MATi,4)==cost );
    
end