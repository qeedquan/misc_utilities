%{

A continuous state space model needs to be discretized for it to be simulated

%}

pkg load control;

m = 0.5
dt = 0.1
A = [0.3, 1 ; 0.4, 0.5]
B = [0 ; 1]
C = [1/m, 0]
D = [3]

G_s = ss(A, B, C, D)
G_z_zoh = c2d(G_s, dt, 'zoh')
G_z_foh = c2d(G_s, dt, 'foh')
G_z_tustin = c2d(G_s, dt, 'tustin')

% The discretized version has different eigenvalues/eigenvectors than the continous version
eig(G_s)
eig(G_z_zoh)
eig(G_z_foh)
eig(G_z_tustin)
