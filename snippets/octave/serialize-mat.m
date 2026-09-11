M1 = [33 34; 44 45]
M2 = [1 2 3 4 5]
save 'test.mat' M1 M2

clear M1 M2 P3
load 'test.mat'
M1
M2
