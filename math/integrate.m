function integrate()
	choice = 0;
	while true
		clf();
		switch choice
		case 0
			plot_integral(@(x) x.*x, -10:0.1:10);
		case 1
			% integral sin(2*pi*n*t)*sin(2*pi*m*t) is 0 when n != m
			% since they are orthogonal functions
			f = @(x) sin(2*pi*x).*sin(2*pi*30*x);
			plot_integral(f, 0:0.01:1);
		case 2
			% sin and cos are orthogonal to each other
			f = @(x) cos(2*pi*x).*sin(2*pi*x);
			plot_integral(f, 0:0.01:1);
		case 3
			f = @(x) sin(2*pi*x).*sin(2*pi*x);
			plot_integral(f, 0:0.01:1);
		case 4
			f = @(x) x;
			plot_integral(f, -10:0.1:10);
		case 5
			plot_square();
		case 6
			plot_power();
		end
		
		title(sprintf('choice: %d', choice));

		switch kbhit()
		case 'q'
			break
		case '1'
			choice = mod(choice - 1, 7);
		case '2'
			choice = mod(choice + 1, 7);
		end
	end
end

function plot_power()
	N = 10000;             % number of points
	T = 3.4;               % define time of interval, 3.4 seconds
	t = [0:N-1]/N;         % define time
	t = t*T;               % define time in seconds
	f = sin(2*pi*10*t);    % define function, 10 Hz sinewave
	p = abs(fft(f))/(N/2); % absolute value of thefft
	p = p(1:N/2).^2;       % take the power of positve freq. 
	freq = [0:N/2-1]/T;    % find the corresponding frequency in Hz
	semilogy(freq,p);      % plot on semilog scale
	axis([0 20 0 1]);      % zoomin
end

function plot_square()
	N = 2000;
	x = [0:100]/100;
	f = ones(1,101)*1/2;
	for i = 1:2:N
		a = 2/pi/i;
		f += a*sin(2*pi*i*x);
	end
	plot(x,f);
end

function plot_integral(f, x)
	y = f(x);
	q = cumtrapz(x, y);

	hold;
	plot(x, y);
	plot(x, q);
end

