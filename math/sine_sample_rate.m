function sine_sample_rate()
	f = 1;
	h = 5;
	p = 0;
	while true
		clf();
		do_test(f, p, h);
		switch kbhit();
		case 27
			break
		case '1'
			if h > 1
				h -= 1;
			end
		case '2'
			h += 1;
		case 'q'
			if f > 1
				f -= 1;
			end
		case 'w'
			f += 1;
		case 'a'
			p -= 0.1;
			p
		case 's'
			p += 0.1;
		end
	end
end

% nyquist sampling rate states that
% sampling rate must be at least twice
% the the frequency as the highest frequency
% for perfect reconstruction of the wave
% this relates to the dft, since a dft will take a time series of N samples,
% and returns an array of size N with each frequency in N. If it is a real signal,
% the array will be symmetric at N/2, so if we want to use dft to detect if a frequency
% is in the signal, we need at N/2 array > freq to detect it in the conventional way.

% to get a smooth plot using 'plot', however, need to sample
% more than nyquist, or else get jagged line (though it does show all frequencies) 

% it is a convention to make a wave be a multiple of 2*pi so
% we multiply f with 2*pi to make it one cycle per second
% this means that the real frequency to a wave is actually 2*pi*f per second
% and we need to adjust the sample rate based on the constant factor 2*pi

% this function samples one full cycle of a wave
% the wave can be made of higher harmonics, given
% the highest harmonic h that is a integer frequency, one complete cycle will be delayed
% by h seconds or (1*h) seconds, phase does not affect the period since it shifts the whole wave
% the amplitude of each higher harmonic is A/h_i for each harmonic h_i (it decrease)

% a dft of an N sample would produce amplitude in the frequency array proportional to N, to normalize
% the conventional way is to divide by the length N.
% We also want to center the frequency around the base frequency
% so we need to swap the values from [0, N/2] with N/2 as the center point (fftshift)

% sum of cosines are the same as sum of sines, just with a 90 phase degree offset
% sum of sine and cosine retain the same frequency, but different amplitudes/phase
function do_test(f, p, h)
	A = 10.0;

	N = 2*pi*32*f;

	F1 = @(x) sum_sin(x, A, f, h, p);
	F2 = @(x) sum_cos(x, A, f, h, p);
	F3 = @(x) sum_sincos(x, A, f, h, p);
	x = linspace(0, 1*h, N);
	y1 = F1(x);
	y2 = F2(x);
	y3 = F3(x);
	
	subplot(4, 1, 1);
	hold;
	plot(x, y1);
	plot(x, y2);
	plot(x, y3);
	
	annotation(
		'textbox', 
		[0.5, 0.5, 0.1, 0.1],
		'String',
		sprintf('frequency: %d harmonics: %d phase: %f', f, h, p)
	)
	subplot(4, 1, 2);
	hold;
	plot(x, fft(y1));
	plot(x, fft(y2));
	plot(x, fft(y3));

	% fft of various sampling rates, the more sampling, the more bins you have to detect
	% the subtler frequencies present
	i = 1*pi*f;
	while i <= N
		subplot(4, 1, 3);
		hold;
		sx = linspace(0, 1*h, i);
		sy = F1(sx);
		fy = abs(fft(sy)) ./ N;
		plot(sx, fy);
		
		% center the transform to 0 by swapping the array values
		subplot(4, 1, 4);
		hold;
		plot(sx, fftshift(fy));

		i *= 2;
	end
end

function r = sum_sin(x, A, f, h, p)
	r = 0;
	for i = 1:h
		r += A/i*sin((2*pi*f)/i*x + p);
	end
	return;
end

function r = sum_cos(x, A, f, h, p)
	r = 0;
	for i = 1:h
		r += A/i*cos((2*pi*f)/i*x + p);
	end
	return;
end

function r = sum_sincos(x, A, f, h, p)
	r = 0;
	for i = 1:h
		r += A/i*sin((2*pi*f)/i*x + p);
		r += A/i*cos((2*pi*f)/i*x + p);
	end
	return;
end
