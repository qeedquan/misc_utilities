%
% Finds the peak frequencies. 
% It does not perform quadratic interpolation.
%
% x - the input signal
% fs - the sampling frequency. default 1
% np - number of peaks to find. default 1
%
function pf = findPeakFreqs(x, fs, np)

if nargin < 2,
    fs = 1;
end

if nargin < 3,
    np = 1;
end

% get FFT
nFFT = 4*2^nextpow2(length(x));
X = abs(fft(x, nFFT));
f = ((1:nFFT)-1)/nFFT*fs;

% we only need lower half
X = X(1:nFFT/2);
X = X/max(X); % normalize

% find peak frequency
[~, locs] = findpeaks(X, 'npeaks', np, 'sortstr', 'descend');

pf = sort( f(locs) );