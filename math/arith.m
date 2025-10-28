function arith()
	test_transpose();
	test_division();
	test_exp();
end

function test_division()
	% for matrix A, B
	% A / B = (inverse(y')*x')' where ' is inverse
	for i = 0:10
		N = randi(50);
		A = randi(100, N, N);
		B = randi(20, N, N);
		C = inv(pinv(inv(B))*inv(A));
		D = A/B;
		abs(C - D) < 1e6*eps(min(abs(A), abs(B)))
	end

	% element-wise division
	for i = 0:5
		N = randi(3);
		A = randi(4, N, N)
		B = randi(4, N, N)
		A ./ B
	end

	% for 1xN vectors, it actually returns a scalar
	for i = 0:10
		N = 5
		A = randi(100, 1, N)
		B = randi(100, 1, N)
		A / B
		A \ B
	end
end

function test_transpose()
	% transpose a matrix
	A = [1 2 3]
	A'
	
	% complex conjugate tranpose
	B = 1 + 4i
	B'

	C = -45
	C'
end

function test_exp()
	% want to express a^n as e^(dn)
	% a^n = e^(dn)
	% take ln of both side
	% ln(a)/n = dn
	% ln(a) = d
	% a^n = e^(ln(a)*n)
	for i = 1:100
		a = randi(100);
		n = ones()*10*rand();
		d = log(a);
		printf('%f %f\n', a^n, e^(d*n));
	end
end
