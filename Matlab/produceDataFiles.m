function produceDataFiles(setMatrices)

% load data of selected matrices
Data = csvread('LAPresults.csv');

% produce all text files by default
if nargin==0, setMatrices = 1:size(Data,1); end

% make Data directory
dataDir = [pwd '/../Data'];
if ~exist(dataDir,'dir'), mkdir(dataDir); end

for i = setMatrices
  
  % download matrix
  MatrixID = Data(i,1);
  prob = UFget(MatrixID);
  
  % generate file name
  filename = sprintf('UFmat_%04u.dat', MatrixID);
  fullfilename = [dataDir '/' filename];
  
  % produce text files
  if ~exist(fullfilename, 'file')
    fprintf('Writing a text file for outputing the Matrix %i in CSC form ...\n', MatrixID)
    sparseMatrix2CSC(prob, fullfilename);
  end
  
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
% Column pointer, C->p in CSparse
fprintf(fid, '%i ', [0 colPointer]);
% Row indices, C->i in CSparse
fprintf(fid, '%i ', i-1);

fclose(fid);
end