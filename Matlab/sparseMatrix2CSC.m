function sparseMatrix2CSC(prob, filename, options)

if nargin<3, options='w'; end

A          = prob.A;
id         = prob.id;
n          = size(A,1);
sparseA    = A~=0;
sumRow     = full(sum(sparseA,1));
colPointer = cumsum(sumRow);
nz         = nnz(sparseA);

[i,~,~] = find(A);
fid = fopen(filename, options);

% matrixID, size, nnz
fprintf(fid, '%i ', id, n, nz);
% Column pointer, C->p
fprintf(fid, '%i ', [0 colPointer]);
% Row indices, C->i
fprintf(fid, '%i ', i-1);

fclose(fid);
end