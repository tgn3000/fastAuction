function solveSampleMatrices(setMatrices)

% load sample matrices
Data = csvread('LAPresults.csv');
if nargin==0, setMatrices = 1:size(Data,1); end

% mex -setup C++
delete([pwd '/*.mexmaci64'])
mex -largeArrayDims ../FlorianFastAuction/auctionAlgorithmSparseMex.cpp -lut

% experiment filling pattern to fill matrices
pattern = [2 1 0 1 1 0 2]';
numelPattern = numel(pattern);

for MATi = setMatrices
  
  % extract sparsity pattern from the SuiteSparse Matrix Collection
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
  [assignments, prices] = ...
    sparseAssignmentProblemAuctionAlgorithm( M, [], [], verbosity );
  
  % optimal value
  profit = full(sum(M((1:n)'+n*(assignments-1))));
  fprintf('profit = %i\n', profit);
  assert(profit==Data(MATi,4));
  
end
end