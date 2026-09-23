%
% Encodes the input signal into lpc coefficients using 50% OLA
%
% x - single channel input signal
% p - lpc order
% nw - window length
% 
% A - the coefficients
% G - the signal power
% E - the full source (error) signal
%
function [A, G, E] = lpcEncode(x, p, w)

X = stackOLA(x, w); % stack the windowed signals
[nw, n] = size(X);

% LPC encode
A = zeros(p, n);
G = zeros(1, n);
E = zeros(nw, n);
for i = 1:n,
    [a, g, e] = myLPC(X(:,i), p);
    
    A(:, i) = a;
    G(i) = g;
    E(2:nw, i) = e;
end