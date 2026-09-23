% various matrix inverse properties

% in general if we extend a square matrix along it's row/colums with zeroes
% the new extended matrix will *not* have the same determinant/inverse
% in general, it's inverse is most likely singular
% however, if we make the newly extended column a identity column
% determinant and inverse are preserved, if we set the 1 in the identity
% column to be another number, the determinant gets multiplied by that constant
function test_det_extend()
	for i = 1:10
		for j = 1:256
			a = rand(i, i);
			p = a;
			q = a;
			printf('%dx%d\n', i, i)
			det(a)
			for k = 1:10
				b = resize(p, i+k, i+k);
				c = resize(q, i+k, i+k);
				printf('extended %dx%d %f', i+k, i+k, det(b));
				b(i+k, i+k) = 1;
				c(i+k, i+k) = 9;
				printf(' %f %f\n', det(b), det(c));
				p = b;
				q = c;
			endfor
			printf('\n')
		endfor
	endfor
end

% same applies to identity matrices
function test_det_extend_identity()
	for i = 1 : 10
		a = eye(i);
		b = resize(a, i+1, i+1);
		det(b)
	endfor
end

% inverse transpose of a matrix is the equal to transpose inverse of a matrix
function test_inv_transpose()
	for i = 1 : 10
		a = rand(i, i);
		b = transpose(a**-1)
		c = transpose(a)**-1
	endfor
end

% for a 3x3 matrix, one can construct the inverse
% rows and columns using dot and cross products
% A    = [u v w]
% A^-1 = [a b c]^T
% a = (v X w) / (u . (v X w))
% b = (w X u) / (u . (v X w))
% c = (u X v) / (u . (v X w))
function test_inv_3x3_dot_cross()
	for i = 1:1
		M = rand(3, 3)
		N = inv(M)
		
		% column vectors [u v w] give rise to
		% row vectors [a b c]^T that is the inverse
		% coefficient vector
		u = M([1;2;3])
		v = M([4;5;6])
		w = M([7;8;9])

		d = dot(u, cross(v, w));
		a = cross(v, w) / d
		b = cross(w, u) / d
		c = cross(u, v) / d

		% row vectors [u v w] will give rise to
		% column vectors [a b c]^T that is the inverse
		% coefficient vector
		u = M([1;4;7])
		v = M([2;5;8])
		w = M([3;6;9])
		d = dot(u, cross(v, w));
		a = cross(v, w) / d
	end
end

