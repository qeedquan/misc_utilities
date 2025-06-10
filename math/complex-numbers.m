% https://www.nagwa.com/en/explainers/152196980513/

% a + bi = [a -b]
%          [b  a]
function [M] = cpm(z)
	a = real(z);
	b = imag(z);
	M = [[a -b]; [b a]];
endfunction

function [z] = mcp(M)
	z = M(1,1) + M(2,1)*j;
endfunction

function test_matrix()
	z1 = 5 + 7i;
	z2 = 1 - 8i;
	M1 = cpm(z1);
	M2 = cpm(z2);

	% z1 +- z2 = M1 +- M2
	z1 + z2
	mcp(M1 + M2)
	
	z1 - z2
	mcp(M1 - M2)

	% z1 * z2 = M1 * M2
	z1 * z2
	mcp(M1 * M2)

	% z1 / z2 = M1 * M2^-1
	z1 / z2
	mcp(M1 * M2**-1)

	% 1 / z = M^-1
	1 / z1
	mcp(M1**-1)

	1 / z2
	mcp(M2**-1)

	% conj(z) = tr(M)
	conj(z1)
	mcp(M1')
	
	conj(z2)
	mcp(M2')

	% |z^2| = det(M)
	abs(z1*z1)
	det(M1)

	abs(z2*z2)
	det(M2)

	abs(z1*z1)*abs(z2*z2)
	det(M1)*det(M2)

	% eig(M) = a +- bi
	% abs(eig(M)) = sqrt(a^2 + b^2) = sqrt(det(M1))
	eig(M1)
	abs(eig(M1))
	sqrt(det(M1))
	abs(eig(M1)) / sqrt(det(M1))

endfunction

test_matrix();
