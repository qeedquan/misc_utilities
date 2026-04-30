% https://en.wikipedia.org/wiki/Phase-shift_keying
% https://www.mathworks.com/help/comm/ref/pskmod.html

% PSK can be viewed as a special case of QAM, instead of arbitrary constellation points
% the points generated are on the unit circle in the complex plane
% the constellation points equation is
% psk_coeff[n] = exp((freq + phase) * i);
% freq = 2*pi*(n-1)/order
% phase = user choice
% common orders (basically powers of 2):
% 2 (binary PSK)
% 4 
% 8/16 (usually the highest order constellation PSK deployed)

pkg load communications;

function genpsk(order, phase)
	printf("PSK order=%d phase=%f\n", order, phase);
	X = (0:1:order-1);
	M = pskmod(X, order, phase);
	for n = 1:length(X)
		freq = 2*pi*(n-1)/order;
		C = exp((freq+phase)*i);
		printf("%-32s | %-32s\n", num2str(M(n)), num2str(C));
		assert(eq(M(n), C));
	end
	printf("\n");
end

genpsk(4, pi/4);
genpsk(16, 304)
genpsk(2, 2*pi);
genpsk(40, 30);
genpsk(8, 100);
