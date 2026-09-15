pkg load control
pkg load symbolic

% define transfer function
n = [1]
d = [1 1]
sys = tf(n, d);
n
d

% simulate the response of various functions
% step, sin, ramp
t0 = 0;
t1 = 10;
ts = 1000;
t = linspace(t0, t1, ts);
u1 = heaviside(t, 1);
u2 = sin(t*20);
u3 = t;
y1 = lsim(sys, u1, t);
y2 = lsim(sys, u2, t);
y3 = lsim(sys, u3, t);
figure(1);
subplot(1, 3, 1);
plot(t, y1);
subplot(1, 3, 2);
plot(t, y2);
subplot(1, 3, 3);
plot(t, y3);
grid;
pause;
