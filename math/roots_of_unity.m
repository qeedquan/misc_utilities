% from euler identity
% e^(2*pi*i) = 1
% e^(2*pi*i*k/M) = 1^(k/M)
% k = 0..M-1
% k/M corresponds to points on the complex unit circle boundary
% the magnitude of the points (x, yi) will be 1

% the points generated by iterating through k/M gives an even
% spaced points around the complex circle boundary, ie, M=4 will
% cut the unit circle into 4 pieces, each of the point is at
% (360/4)=90 degrees from each other

function roots_of_unity()
	M = 4; 
	while true
		clf();
		hold();
		draw_unit_circle(M);
		
		key = kbhit();
		switch key
		case 'a'
			M -= 1;
		case 'd'
			M += 1;
		case {'q', 27}
			break
		end
	end
end

function draw_unit_circle(M)
	t = linspace(0, 2*pi, 1024);
	z = e.^(i*t);
	plot(z);

	axis equal;
	axis([-1.5 1.5 -1.5 1.5]);

	for k = 0:M-1
		p = e.^(2*pi*i*k/M);
		plot(p, '*');
	end
	text(0, 0, sprintf('M: %d', M));
end
