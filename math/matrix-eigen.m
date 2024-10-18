% various matrix eigenvectors/eigenvalues properties

% https://en.wikipedia.org/wiki/Diagonalizable_matrix#How_to_diagonalize_a_matrix
% If a matrix A is diagonalizable, it can be written as
% D = P'AP
% where the matrix P is the eigenvectors of the matrix A
% and D is the eigenvalues of the matrix A written in matrix form
% if we have the diagonal and P, we can figure out A from it also
% DP' = P'A(PP')
% PDP' = PP'A(PP')
% PDP' = (I)A(I) = A
function test_diagonalization(N)
	A = rand(N)
	[P, D] = eig(A)
	P*D*inv(P)
	inv(P)*A*P
end

% https://en.wikipedia.org/wiki/Rayleigh_quotient
% given a diagonalizable matrix A, and eigenvector of
% return the the eigenvalue of the associate eigenvector for A
function rayleigh_quotient(A, b)
	(transpose(b)*A*b) / (transpose(b)*b)
end

% https://en.wikipedia.org/wiki/Power_iteration
% given a diagonalizable matrix A, algorithm will find
% the largest absolute value of the eigenvector of A
function test_power_method(N, I)
	A = rand(N)
	b = rand(N, 1)
	for i = 1:I
		b = A*b;
		b = b/norm(b);
	end
	b
	rayleigh_quotient(A, b)
	[P, D] = eig(A)
end

test_diagonalization(2)
test_diagonalization(3)
test_power_method(3, 100)
test_power_method(4, 10)
