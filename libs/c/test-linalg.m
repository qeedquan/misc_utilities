function test_transform
	disp("Testing Transform")
	v = [1 2 3 4]
	M = [[1 2 3 4]
		 [5 6 7 8]
		 [9 10 11 12]
		 [13 14 15 16]]
	M*v.'
	v*M
end

function test_matrix_multiplication
	A = [[50 38 19 49]
	     [94 28 20 58]
		 [96 10 86 1]
		 [-5 -2 -6 -4]]
	B = [[1 2 3 4]
	     [5 6 7 8]
		 [9 10 11 12]
		 [13 14 15 16]]
	C = [[4 1 6 1]
	     [19 58 7 -2]
		 [-6 2 -4 1]
		 [9 7 9 1]]
	D = [[3 5 6 2]
	     [29 659 1 5]
		 [-21 -76 -2 1]
		 [1 6 2 3]]
	A*B*C*D
end

test_transform()
test_matrix_multiplication()
