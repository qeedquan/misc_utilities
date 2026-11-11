function sine_wave_analysis()
	global B A N fs T fmax df f tmax t;
	init();
	calc();
	while true
		clf();
		hold;

		[gains, phases] = swanalc(t, f, B, A);
		subplot(2, 2, 1);
		plot(gains);
		subplot(2, 2, 3);
		plot(phases);
	
		[gains, phases] = swanalfft(fs, length(f), B, A);
		subplot(2, 2, 2);
		plot(gains);
		subplot(2, 2, 4);
		plot(phases);
		
		switch kbhit()
		case {'q', 27}
			break
		case '1'
			fs -= 1;
		case '2'
			fs += 1;
		case 'a'
			N -= 1;
		case 's'
			N += 1;
		end
		
		calc();
		printf('fs %d N %d\n', fs, N);
	end
end

function init()
	global B A N fs T fmax df f tmax t;
	B = [1 1];
	A = 1;
	N = 10;
	
	fs = 1;
	T = 1/fs;
	fmax = fs/2;
	df = fmax/(N-1);
	f = 0:df:fmax;
	tmax = 1/df;
	t = 0:T:tmax;
end

function calc()
	global B A N fs T fmax df f tmax t;

	T = 1/fs;
	fmax = fs/2;
	df = fmax/(N-1);
	f = 0:df:fmax;
	tmax = 1/df;
	t = 0:T:tmax;
end

function [y] = mod2pi(x)
	y = x;
	while y >= pi, y -= 2*pi; end
	while y < -pi, y += 2*pi; end
end

% https://ccrma.stanford.edu/~jos/filters/Complex_Sine_Wave_Analysis.html
function [gains, phases] = swanalc(t, f, B, A)
	% input signal amplitude
	ampin = 1;
	% input signal phase
	phasein = 0;

	% number of test frequencies
	N = length(f);
	% pre-allocate amp-response array
	gains = zeros(1, N);
	% pre-allocate phase-response array
	phases = zeros(1, N);

	if length(A) == 1
		ntransient = length(B)-1;
	else
		error('need to set transient response duration here');
	end
	
	% loop over analysis frequencies
	for k = 1:length(f)
		% test sinusoid
		s = ampin*e.^(j*2*pi*f(k)*t + phasein);
		% run it through the filter
		y = filter(B, A, s);
		% chop off transient
		yss = y(ntransient+1:length(y));
		% avg instantaneous amplitude
		ampout = mean(abs(yss));
		% amplitude response sample
		gains(k) = ampout / ampin;
		% align with yss
		sss = s(ntransient+1:length(y));
		% get the phases
		phases(k) = mean(mod2pi(angle(yss.*conj(sss))));
	end
end

% https://ccrma.stanford.edu/~jos/filters/Practical_Frequency_Response_Analysis.html
function [gains, phases] = swanalfft(fs, N, B, A)
	Bzp = [B, zeros(1, N-length(B))];
	H = fft(Bzp);

	% non-negative frequency indices
	nnfi = (1:N/2+1);
	% lose negative-frequency samples
	Hnnf = H(nnfi);
	% corresponding bin numbers
	nnfb = nnfi-1;
	% frequency axis in HZ
	f = nnfb*fs/N;
	gains = abs(Hnnf);
	phases = angle(Hnnf);
end
