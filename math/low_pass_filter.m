function low_pass_filter()
	filter = 0;
	f = 1;
	p = 0;
	while true
		clf();
		[A, B] = get_filter(filter);
		lpf1(32, A, B);
		lpf1s(f, p, A, B);
		switch kbhit()
		case 27
			break
		case '1'
			f -= 1;
		case '2'
			f += 1;
		case 'a'
			filter -= 1;
		case 's'
			filter += 1;
		end
		f
		filter
	end
end

% B - feedforward matrix
% A - feedback matrix
function [A, B] = get_filter(type)
	switch type
	case 0
		% y(n) = x(n) + x(n-1)
		A = 1;
		B = [1, 1];
	case 1
		% y(n) = x(n) - x(n-1);
		A = 1;
		B = [1, -1];
	case 2
		% y(n) = x(n-1) - x(n)
		A = 1;
		B = [-1, 1];
	case 3
		% y(n) = x(n) + x(n-2)
		A = 1;
		B = [1, 0, 1];
	case 4
		% y(n) = x(n) - x(n-2)
		A = 1;
		B = [1, 0, -1];
	end
	return
end

function lpf1(N, A, B)
	% integer ramp
	x = 1:N;

	y = filter(B, A, x);
	abs(y) ./ abs(x)
end

% calculate the frequency response of the filter
% this consists of the amplitude response (ratio of output/input absolute value of signal)
% and phase response (the difference between output and input phase/angle)
% bode plot is the official terminology for this sort of plots, but bode plots show the
% only shows up the response up to f/2 where f is the sampling frequency, because the fourier transform
% of the real signal is symmetric around f/2, we show up until f, so we see the symmetry.

% we use a sine wave input because sine wave is the only function with the property of not changing 
% frequency (only amplitude/phase is changed) under an LTI transformation (which is what FIR filters are)

% doing a fourier transform on the input and the output after the filter has been applied and comparing
% their magnitude ratio gives the amplitude response, and their phase difference is the phase response
% the gain in amplitude and phase are the same no matter what frequency we give the sine at.
% what it does is if we feed it a higher frequency, the amplitude gain/phase plot stretches out since
% we have more samples, but the plot looks the same. Adding linear combination functions of the sine wave
% also does not change the plot output.
function lpf1s(f, p, A, B)
	N = 16*2*f;

	t = linspace(0, 1, N);
	x = sin(2*pi*f*t + p);
	y = filter(B, A, x);
	
	X = fft(x);
	Y = fft(y);
 
	subplot(3, 1, 1);
	hold;
	plot(t, x);
	plot(t, y);

	subplot(3, 1, 2);
	hold;
	axis([0 1 0, 2], "autox") 
	HA = abs(Y) ./ abs(X);
	plot(HA);

	subplot(3, 1, 3);
	HP = angle(Y) - angle(X);
	plot(HP);
end
