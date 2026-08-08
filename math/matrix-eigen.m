% various matrix eigenvectors/eigenvalues properties

function U = rand_unitary(n)
	X = (randn(n) + i*randn(n))/sqrt(2);
	[Q, R] = qr(X);
	R = diag(diag(R)./abs(diag(R)));
	U = Q*R;
end

%{

https://en.wikipedia.org/wiki/Diagonalizable_matrix#How_to_diagonalize_a_matrix
If a matrix A is diagonalizable, it can be written as
D = P'AP
where the matrix P is the eigenvectors of the matrix A
and D is the eigenvalues of the matrix A written in matrix form
if we have the diagonal and P, we can figure out A from it also
DP' = P'A(PP')
PDP' = PP'A(PP')
PDP' = (I)A(I) = A

%}

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

%{

https://en.wikipedia.org/wiki/Unitary_matrix
Unitary matrices has the following property

U' = complex conjugate transpose of U
U^-1 = U'

P = eigenvectors of U
P' = complex conjugate transpose of P
PP' = I

abs(D) = I

dot(U, U) = 1
dot(UP, UP) = 1
dot(D, D) = 1

Eigenvalues are always in the form of exp(2*pi*i*theta)

%}

function test_matrix_unitary(N)
	U = rand_unitary(N)
	[P, D] = eig(U)
	U * ctranspose(U)
	P * ctranspose(P)
	abs(D)
	dot(U, U)
	dot(U*P, U*P)
	dot(D, D)

	% equal
	U * P
	P * D

	% equal
	P' * ctranspose(U)
	ctranspose(D) * P'
end

test_diagonalization(2)
test_diagonalization(3)
test_power_method(3, 100)
test_power_method(4, 10)
test_matrix_unitary(3)
test_matrix_unitary(4)
