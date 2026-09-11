% https://en.wikipedia.org/wiki/Additive_white_Gaussian_noise
% higher SNR means noise floor is very small so signal looks closer and closer to the original
% lower SNR means noise floor is large so the signal gets distorted
pkg load signal;
pkg load communications;

function graph(t, snr, f)
	x = arrayfun(@(t) f(t), t);
	y = awgn(x, snr, 'measured');
	plot(t, [x, y]);
	legend('Original Signal', 'Signal with AWGN');
end

args = argv();
if (length(args) < 1)
	printf("usage: snr\n");
	exit(1);
end

snr = str2num(args{1});
t = (0:0.1:10)';
n = 6;
subplot(n, 1, 1);
graph(t, snr, @sin);
subplot(n, 1, 2);
graph(t, snr, @(x) x.^2);
subplot(n, 1, 3);
graph(t, snr, @(x) x);
subplot(n, 1, 4);
graph(t, snr, @sawtooth);
subplot(n, 1, 5);
graph(t, snr, @(x) sin(x)*cos(x));
subplot(n, 1, 6);
graph(t, snr, @(x) exp(-x));

pause;
