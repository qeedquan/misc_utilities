% https://dspfirst.gatech.edu/archives/lectures/DSPFirst-L02-pp4.pdf

% hz per second
fs = 8000;

% generate some samples from [-1, 5] for a a tone
t = -1 : 1/fs : 5.0;
x = 2.1*cos(2*pi*440*t + 0.4*pi) + 3.5*cos(2*pi*500*t + 0.1*pi);

% envelope for exponential decay of sound
y = exp(-abs(t)*1.2);
y = x.*y;

soundsc(y, fs);
