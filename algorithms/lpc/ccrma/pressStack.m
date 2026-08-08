%
% Renders an overlap-add stack into the original signal.
% It assumes the stacked signals are already windowed.
%
% X - a stacked overlap-add
%
% x - the rendered signal
%
function x = pressStack(X)

[nw, count] = size(X);
step = floor(nw*0.5);
n = (count-1)*step+nw;

x = zeros(n, 1);

for i = 1:count
   x( (1:nw) + step*(i-1) ) = x( (1:nw) + step*(i-1) ) + X(:, i);
end
