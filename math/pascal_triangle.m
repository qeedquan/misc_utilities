function pascal_triangle()
	test_convolution()
end

% pascal triangle can be generated as a repeated convolution
% since convolution represents polynomial multiplication
function test_convolution()
	% this coefficient gives the next row for pascal triangle
	% under convolution
	A = [1 1];

	% second row of pascal
	B = [1];

	% order doesn't matter
	for i = 1:10
		X = conv(A, B);
		Y = conv(B, A);
		X
		Y
		B = X
	end

	% if we instead convolve it with
	% A = P_i-1
	% B = Pi
	% we step through the rows faster
	A = [1 1]
	B = [1 2 1]
	for i = 1:10
		B
		C = conv(A, B);
		A = B;
		B = C;
	end
end

