%
% An implementation of LPC.
% A detailed explanation can be found at
% https://ccrma.stanford.edu/~hskim08/lpc/
%
% x - a single channel audio signal
% p - the polynomial order of the all-pole filter
%
% a - the coefficients to the all-pole filter
% g - the variance(power) of the source
% e - the full error signal
%
% NOTE: This is not the most efficient implementation of LPC.
% Matlab's own implementation uses FFT to via the auto-correlation method
% which is noticeably faster. (O(n log(n)) vs O(n^2))
%
function [a, g, e] = myLPC(x, p)

N = length(x);

% form matrices
b = x(2:N);
xz = [x; zeros(p,1)];

A = zeros(N-1, p);
for i=1:p,
    temp = circshift(xz, i-1);
    A(:, i) = temp(1:(N-1));
end

% solve for a
a = A\b;

% calculate variance of errors
e = b - A*a;
g = var(e);

