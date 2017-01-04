% Before running this file, please add UFget to Matlab's search path
clear; close all; clc
addpath( [pwd '/../FlorianFastAuction'] )

% set default range of sample matrices
rangeSampleMatrices = 1:40;
% produce LAP text files
produceDataFiles(rangeSampleMatrices);
% call Florian's solver to solve these LAPs
solveSampleMatrices(rangeSampleMatrices)
