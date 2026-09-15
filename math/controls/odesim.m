pkg load control;
pkg load signal;

% discretize the continuous state space system using a zero order hold
% then simulate over time using the ABCD matrix
function [Y, T, X] = mysim(sys, u, t, x0)
	[urows, ucols] = size(u);
	dt = abs (t(end) - t(1)) / (urows - 1);
	t = vec (linspace (t(1), t(end), urows));
	sys = c2d(sys, dt, 'zoh'); 

	[A, B, C, D] = ssdata(sys);
	[p, m] = size(D);
	n = rows(A);

	yout = zeros(urows, p);
	xout = zeros(urows, n);

	if isempty(x0)
		x0 = zeros(n, 1);
	end
	x = vec(x0);

	T = t;
	for k = 1:urows
		Y(k, :) = C*x  +  D*u(k, :).';
		X(k, :) = x;
		x = A*x  +  B*u(k, :).';
	end
end

function analyze(fig, file, G, Tf)
	t = 0:0.05:Tf;
	u = t >= 0;

	[A, B, C, D] = tf2ss(G);
	sys = ss(A, B, C, D);

	% simulate with step response given discrete samples
	[yn, tn] = lsim(sys, u, t);
	plot(tn, yn);
	grid on;
	axis([0 Tf 0 ceil(max(yn))]);
	title('Linear Simulation Results');
	saveas(fig, sprintf("%s_lsim", file), 'png');

	% do our own state space simulation, should match lsim closely
	[yn, tn] = mysim(sys, u', t, []);
	plot(tn, yn);
	grid on;
	title('Linear Simulation Results');
	axis([0 Tf 0 ceil(max(yn))]);
	saveas(fig, sprintf('%s_mysim', file), 'png');
end

fig = figure('visible', 'off');
s = tf('s');

Kp = 10;
taup = 3;
Tf = 20;
G = Kp/(taup*s + 1);
analyze(fig, '1st_order', G, Tf);

Kp = 2.0;
tau = 1;
zeta = 0.5;
theta = 0.0;
du = 1.0;
Tf = 20;
G = tf([Kp], [tau^2, 2*zeta*tau, 1]);
analyze(fig, '2nd_order', G, Tf);
