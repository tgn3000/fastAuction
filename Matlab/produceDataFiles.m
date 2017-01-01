clear; close all; clc

% load data of selected matrices
Data = xlsread('CSA_results.xlsx');
nMatrix = size(Data,1);

currDir = pwd;
upDir = currDir(1:find(currDir=='/',1,'last')-1);
dataDir = [upDir '/Data'];

if ~exist(dataDir,'dir'), mkdir(dataDir); end

numOfCoarseBlocks = [];
smallestNumberOfCoarseBlocks = 10;

for i=1:40%nMatrix
    
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
    
    fprintf('i=%3i, ID=%4i, n=%5i, nz=%7i\n', i, MatrixID, size(A,1), nnz(A));
    
    fprintf('------------------------------------------------------------\n');
end
