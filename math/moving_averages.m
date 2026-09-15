function moving_averages()
	typ = 0;
	num_points = 10;
	while true
		draw_graph(typ, num_points);
		
		switch kbhit()
		case {'q', 27}
			break;
		case '1'
			if num_points > 1
				num_points /= 2
			end
		case '2'
			num_points *= 2
		case 'a'
			typ -= 1
		case 's'
			typ += 1
		end
	end
end

function draw_graph(typ, num_points)
	clf();
	x = linspace(0, num_points-1, num_points);
	switch typ
	case 0
		y = ones(num_points, 1) * randi([-100, 100]);
	case 1
		y = randi([-100, 100], num_points, 1);
	case 2
		t = (0:1:num_points-1);
		y = sin(2*pi*(t/(num_points-1))) + 0.2*randn(size(t));
	otherwise
		assert(0);
	end

	% simple moving average of window size w
	hold;
	i = 1;
	while i <= num_points
		w = i;
		v = filter(ones(w, 1)/w, 1, y);
		plot(x, v);
		i += 1;
	end
	
	scatter(x, y, '*');
	legend(sprintf('Type: %d Points: %d', typ, num_points));
end
