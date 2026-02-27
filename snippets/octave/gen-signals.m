pkg load control;

fig = figure('visible', 'off');

% By default gensig generates 64 samples per period for 10 seconds
% Ts = tau/64
% Length of array generated = 10/Ts

% Signal with a period of tau seconds before repeating running for Tf seconds
tau = 2;
Tf = 10;

sig = {'square', 'sine', 'pulse'};
for i = 1:length(sig)
	[u, t] = gensig(sig{i}, tau, Tf);
	plot(t, u);
	axis([0 Tf+5 -2 2]);
	saveas(fig, sig{i}, 'png');
end

