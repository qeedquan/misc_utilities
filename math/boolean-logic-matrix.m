%{

https://ozaner.github.io/boolean-logic-with-matrices/

Boolean elements can be represented using linear algebra operations.

%}

function R = TENSOR(A, B)
	R = [A(1)*B(1); A(1)*B(2); A(2)*B(1); A(2)*B(2)];
end

function R = ZERO
	R = [1; 0];
end

function R = ONE
	R = [0; 1];
end

function R = IDENTITY(E)
	R = [1 0; 0 1] * E;
end

function R = NOT(E)
	R = [0 1; 1 0] * E;
end

function R = CONSTANT_ZERO(E)
	R = [1 1; 0 0] * E;
end

function R = CONSTANT_ONE(E)
	R = [0 0; 1 1] * E;
end

function R = OR(A, B)
	R = [1 0 0 0; 0 1 1 1] * TENSOR(A, B);
end

function R = AND(A, B)
	R = [1 1 1 0; 0 0 0 1] * TENSOR(A, B);
end

function R = XOR(A, B)
	R = [1 0 0 1; 0 1 1 0] * TENSOR(A, B);
end

% Another name for this is IFF (Logical Equivalence)
function R = XNOR(A, B)
	R = [0 1 1 0; 1 0 0 1] * TENSOR(A, B);
end

function R = IF(A, B)
	R = [0 0 1 0; 1 1 0 1] * TENSOR(A, B);
end

display('*** BOOLEAN ELEMENTS ***');
display(ZERO);
display(ONE);

display('*** IDENTITY ***');
display(IDENTITY(ZERO));
display('');
display(IDENTITY(ONE));
display('');

display('*** NOT ***');
display(NOT(ZERO));
display('');
display(NOT(ONE));
display('');

display('*** CONSTANT ZERO ***');
display(CONSTANT_ZERO(ZERO));
display('');
display(CONSTANT_ZERO(ONE));
display('');

display('*** CONSTANT ONE ***');
display(CONSTANT_ONE(ZERO));
display('');
display(CONSTANT_ONE(ONE));
display('');

display('*** OR ***');
display(OR(ZERO, ZERO));
display('');
display(OR(ZERO, ONE));
display('');
display(OR(ONE, ZERO));
display('');
display(OR(ONE, ONE));
display('');

display('*** AND ***');
display(AND(ZERO, ZERO));
display('');
display(AND(ZERO, ONE));
display('');
display(AND(ONE, ZERO));
display('');
display(AND(ONE, ONE));
display('');

display('*** XOR ***');
display(XOR(ZERO, ZERO));
display('');
display(XOR(ZERO, ONE));
display('');
display(XOR(ONE, ZERO));
display('');
display(XOR(ONE, ONE));
display('');

display('*** XNOR ***');
display(XNOR(ZERO, ZERO));
display('');
display(XNOR(ZERO, ONE));
display('');
display(XNOR(ONE, ZERO));
display('');
display(XNOR(ONE, ONE));
display('');

display('*** IF ***');
display(IF(ZERO, ZERO));
display('');
display(IF(ZERO, ONE));
display('');
display(IF(ONE, ZERO));
display('');
display(IF(ONE, ONE));
display('');
