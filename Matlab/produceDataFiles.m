function produceDataFiles(setMatrices)

% load data of selected matrices
Data = csvread('LAPresults.csv');
if nargin==0, setMatrices = 1:size(Data,1); end

currDir = pwd;
upDir = currDir(1:find(currDir=='/',1,'last')-1);
dataDir = [upDir '/Data'];

if ~exist(dataDir,'dir'), mkdir(dataDir); end

for i = setMatrices
  
  MatrixID = Data(i,1);
  filename = sprintf('UFmat_%04u.dat', MatrixID);
  fullfilename = [dataDir '/' filename];
  
  prob = UFget(MatrixID);
  A = prob.A;
  
  if ~exist(fullfilename, 'file')
    fprintf('Writing a text file for outputing the matrix in CSC form ...\n')
    sparseMatrix2CSC(prob, fullfilename);
    fprintf('Finished writing.\n')
  end
  
  fprintf('i=%3i, ID=%4i, n=%5i, nnz=%7i\n', i, MatrixID, size(A,1), nnz(A));
  fprintf('--------------------------------------\n');
  
end

end

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