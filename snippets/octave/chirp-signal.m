% https://octave.sourceforge.io/signal/function/chirp.html

pkg load signal

% Shows linear sweep of 100 Hz/sec starting at zero for 5 sec
% since the sample rate is 1000 Hz, this should be a diagonal
% from bottom left to top right.
t = 0:0.001:5;
y = chirp(t);
specgram(y, 256, 1000);
pause;

% Shows a quadratic chirp of 400 Hz at t=0 and 100 Hz at t=10
% Time goes from -2 to 15 seconds.
t = -2:0.001:15;
y = chirp (t, 400, 10, 100, "quadratic");
[S, f, t] = specgram (y, 256, 1000);
t = t - 2;
imagesc(t, f, 20 * log10 (abs (S)));
set (gca(), "ydir", "normal");
xlabel("Time");
ylabel("Frequency");
pause;

% Shows a logarithmic chirp of 200 Hz at t=0 and 500 Hz at t=2
% Time goes from 0 to 5 seconds at 8000 Hz.
t = 0:1/8000:5;
y = chirp(t, 200, 2, 500, "logarithmic");
specgram(y, 256, 8000);
pause;
