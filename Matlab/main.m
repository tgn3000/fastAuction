% Before running this file, please add UFget to Matlab's search path

clear; close all; clc

addpath( [pwd '/../FlorianFastAuction'] )

produceDataFiles(1:40);
solveSampleMatrices(1:40)
