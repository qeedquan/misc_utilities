%{

Show how gain/zero/poles affect the bode plot

%}

pkg load control;

function pure_gain(fig, file, K)
	G = tf([K], [1]);

	% For every magnitude increase/decrease, the magnitude response goes up by 20
	G1 = 10^0*G;
	G2 = 10^1*G;
	G3 = 10^2*G;
	G4 = 10^3*G;
	
	G5 = 10^-1*G;
	G6 = 10^-2*G;
	G7 = 10^-3*G;

	bode(G1, G2, G3, G4, G5, G6, G7);
	
	title('Pure gain with no root/poles');
	saveas(fig, file, 'png');
end

%{

Pure gain system no roots/poles
Magnitude response - 20*log10(K)
Phase response - 0 (+K) or -180 (-K)

%}

fig = figure('visible', 'off');
pure_gain(fig, 'bode_plot_pure_positive_gain', 1);
pure_gain(fig, 'bode_plot_pure_negative_gain', -1);

%{

First order system with 1 zero, no poles
Magnitude response - +20 db per decade (magnitude increasing, the gain K determines the where the graph starts but does not affect the slope)
Phase response - tan^-1(w*tau) (lhp); tan^-1(-w*tau) (rhp), 90 degree for zero at origin (derivative)

%}

tau = 1;
s = tf('s');
G1 = 1 + s*tau;
G2 = 1 - s*tau;
G3 = s*tau;

bode(G1, 10^1*G1, 10^2*G1, 10^3*G1, 10^4*G1, 10^-1*G1, 10^-2*G1, 10^-3*G1);
saveas(fig, 'lhp_zero.png', 'png');

bode(G2, 10^1*G2, 10^2*G2, 10^3*G2, 10^4*G2, 10^-1*G2, 10^-3*G2, 10^-4*G2);
saveas(fig, 'rhp_zero.png', 'png');

bode(G3, 10^1*G3, 10^2*G3, 10^3*G3, 10^4*G3, 10^-1*G3, 10^-3*G3, 10^-4*G3);
saveas(fig, 'root_zero.png', 'png');

%{

First order system with 1 pole, no zeros
Magnitude response - -20 db per decade (magnitude increasing, the gain K determines the where the graph starts but does not affect the slope)
Phase response - -tan^-1(w*tau) (lhp); tan^-1(w*tau) (rhp), -90 degree for zero at origin (integrator)

%}

tau = 1;
s = tf('s');
G1 = 1 / (1 + s*tau);
G2 = 1 / (1 - s*tau);
G3 = 1 / (s*tau);

bode(G1, 10^1*G1, 10^2*G1, 10^3*G1, 10^4*G1, 10^-1*G1, 10^-2*G1, 10^-3*G1);
saveas(fig, 'lhp_pole.png', 'png');

bode(G2, 10^1*G2, 10^2*G2, 10^3*G2, 10^4*G2, 10^-1*G2, 10^-3*G2, 10^-4*G2);
saveas(fig, 'rhp_pole.png', 'png');

bode(G3, 10^1*G3, 10^2*G3, 10^3*G3, 10^4*G3, 10^-1*G3, 10^-3*G3, 10^-4*G3);
saveas(fig, 'root_pole.png', 'png');
