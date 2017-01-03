clear; close all; clc

%% load sample matrices
Data = csvread('LAPresults.csv');
nMatrix = size(Data,1);

% mex -setup C++
delete([pwd '/*.mexmaci64'])
mex -largeArrayDims ../FlorianFastAuction/auctionAlgorithmSparseMex.cpp -lut

%% my filling pattern for experiment
pattern = [2 1 0 1 1 0 2]';
numelPattern = numel(pattern);

%% obtain sparsity pattern by UFget; see
% 1) https://www.cise.ufl.edu/research/sparse/matrices/
% 2) http://www.cise.ufl.edu/research/sparse/mat/UFget.html

for MATi = 1:nMatrix
  
  matID = Data(MATi,1);
  prob = UFget(matID);
  A = prob.A;
  n = size(A,1);
  [i,j] = find(A);
  nz = nnz(A);
  Aij = [ repmat(pattern,floor(nz/numelPattern),1);
    pattern(1:mod(nz,numelPattern)) ];
  
  % construct matrix
  M = sparse(i,j,Aij+1,n,n);
  fprintf('Matrix %i, n = %i, nnz = %i, ', matID, n, nnz(M));
  
  % call Florian's solver to solve the assignment problem
  verbosity = 0;
  doFeasibilityCheck = 1;
  [assignments, prices] = ...
    sparseAssignmentProblemAuctionAlgorithm( M, [], [], verbosity );
  
  % optimal value
  profit = full(sum(M((1:n)'+n*(assignments-1))))-n;
  fprintf('profit = %i\n', profit);
end