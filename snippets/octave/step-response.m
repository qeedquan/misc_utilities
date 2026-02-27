pkg load control
pkg load symbolic

% define transfer function
n = rand(5, 1);
d = rand(5, 1);
sys = tf(n, d);
n
d

% simulate the unit step response
t0 = 0;
t1 = 100;
ts = 1000;
t = linspace(t0, t1, ts);
u = heaviside(t, 1);
y = lsim(sys, u, t);
figure(1);
subplot(1, 2, 1);
plot(t, y);
subplot(1, 2, 2);

% another way to do the same thing
step(sys, t1);

grid;
pause;
