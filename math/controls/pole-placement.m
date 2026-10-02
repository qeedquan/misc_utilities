%{

Given a LTI system with feedback using the state space update method

xdot = A*x + Bu
y = Cx + Du

Our control feedback is

u = r*Kr - K*x

Plug in u to get
xdot = Ax + B*(r*Kr - K*x)

Expand and redistribute
xdot = (A - B*K)*x + B*r*Kr

So now the closed loop A matrix can be controlled by the K gain matrix

%}

pkg load control;

%{

An example of an unstable system with the A matrix having one of its eigenvalues greater than 0
We want to stabilize it using pole placement

%}

A = [0 1; 2 -1]
B = [1; 0]
C = [1 0]
D = [0]
sys = ss(A, B, C, D);
eig(A)

%{

We can use the K matrix to transform the A matrix using this equation
A' = (A - B*K)

Set the K such that it changes the eigenvalues, in this case we want the poles to move to -2, -1

To adjust the steady state to unity, use Kr to adjust the gain
Kr = 1/steady_state_value

%}

p = [-2 -1];
K = place(A,B,p);
Acl = A - B*K;
syscl = ss(Acl, B, C, D);
eig(syscl)

Kdc = dcgain(syscl);
Kr = 1/Kdc

step(sys, Kr*syscl);
axis([0 5 0 3])

pause;

