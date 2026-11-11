%
% A simple implementation for noise/pitch detection for LPC encoding.
%
% x - the input signal
% w - the window function
% np - number of peaks to find for pitch detection. default 5
% fcut - the fundamental frequency limit in normalized frequency. default 0.125
% pcut - the power cutoff in dB. default -60
% hcut - the harmonics cutoff criteria. default 0.5
%        to find the harmonics, we divide the peak frequencies by the
%        lowest frequency, then look at how far from an integer the peaks
%        are. if it is very harmonic, the values should be very close to
%        integers.
%
% ff - the estimated fundamental freqeuncy. returns 0 for noise.
% p - the signal power in dB for each corresponding chunk
%
function [ff, p] = lpcFindPitch(x, w, np, fcut, pcut, hcut)

X = stackOLA(x, w); % stack the windowed signals
[~,n] = size(X);

% handle defaults
if nargin < 7,
    hcut = 0.5;
end

if nargin < 6,
    pcut = -30;
end

if nargin < 5,
    fcut = 0.125;
end

if nargin < 4,
   np = 5;
end

% get peak frequencies 
f = zeros(np, n); % frequencies
p = zeros(1, n); % power
for i = 1:n,
    xi = X(:,i);
    
    f(:,i) = findPeakFreqs(xi, 1, np);
    p(i) = 10*log10(mean(xi.^2)); % in dB
end

%% check for harmonics
f0 = f(1,:);
ff = f./( ones(np, 1) * f0 );
df = ff - (1:np)' * ones(1, n);
df = df(1:floor(np/2),:);

% clean up
ff = f0;
ddf = max(abs(df));
ff(ddf > hcut) = 0;
ff(ff > fcut) = 0;
ff(p < pcut) = 0;