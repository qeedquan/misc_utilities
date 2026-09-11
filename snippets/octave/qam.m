% https://en.wikipedia.org/wiki/Quadrature_amplitude_modulation
% a form of digital modulation

pkg load communications;

% the constellation points, we want them to be spaced at some minimal distance away
% so we can distinguish easier
c = [1+2i 3+4i 5+6i 7+8i 9+10i]

% our message, we will encode as points inside the constellation
% the message when encoded has to be valid symbols, ie, they have to be from 0..M-1,
% making them a lookup index into the constellation point array, ie
% 0 will be 1+2i and 1 will be 3+4i and so on
M = [0 1 2 3 4]
X = genqammod(M, c)
% add some noise
Xn1 = X + randn(1)
Xn2 = X + 50*randn(1)
Xn3 = X + sin(randn(1))

% demodulation converts the constellation points back into the vector, however
% this one is more general as in if we have add some noise to our vector, it will attempt
% to find the closest match to the original message for us
Y = genqamdemod(X, c)
Yn1 = genqamdemod(Xn1, c)
Yn2 = genqamdemod(Xn2, c)
Yn3 = genqamdemod(Xn3, c)
genqamdemod([0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20], c)
