%{

Given a transfer function in the z-domain

H(z) = B(z)/A(z)
B(z) = b0 + b1*z^-1 + b2*z^-2 ... + bm*z^-m
A(z) = 1 + a1*z^-1 + a2*z^-2 + ... an*z^-n

A partial fraction expansion finds the roots and the poles so the form becomes:

Sum[i=1, N] r[i] / (1 - p[i]*z^-1) (Note that that all terms becomes first or second order terms)

%}

pkg load signal;

B = [1 0 0 0.125]
A = [1 0 0 0 0 0.9^5]
[r, p, f] = residuez(B,A)

% residued delays the IIR part until after the FIR part.
B=[2 6 6 2]; A=[1 -2 1];
[r,p,f,m] = residued(B,A)
[r,p,f,m] = residuez(B,A)
