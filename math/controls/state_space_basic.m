% https://blog.wesleyac.com/posts/intro-to-control-part-four-state-space

% state space representation decomposes dynamics into 4 matrices of first order differential equations
% x' = Ax + Bu
% y  = Cx + Du
% where 
% x' is the input (how it will change over time with the state space variable dynamic)
% y is the output
% x is the state variables that determine the dynamics of the system
% A and B are matrices that describe how things will change over time if there are no external influences
% C and D are matrices that describe how the output will change over time

% in the elevator example
% x' is composed of velocity and acceleration
% x is composed of position and velocity
% y is the sensor output, the update position in this cases

% pkg install -forge control

% Setup for simulation
pkg load control
times = 0:0.1:20;

% Our state space matrices in matlab/octave format
A = [0 1;
     0 -10/100];
B = [0;
     1/100];
C = [1 0];
D = [0];

% We'll start at a position of 10 meters with at velocity of 0 m/s
startingPosition = [10; 0];

% You can think of this as the PID gains - the first value is the proportional
% value, and the second one is the derivative. We'll go into ways to find this
% automatically later, but for now, you can just manually twiddle with these :)
K = [30 70]

% This creates a model of our system, using our PID (or more accurately PD) gains
elevatorClosedLoop = ss(A-B*K,B,C,D);

% This simulates the system and plots it on the screen
lsim(elevatorClosedLoop, 0*ones(size(times)), times, startingPosition);
pause;
