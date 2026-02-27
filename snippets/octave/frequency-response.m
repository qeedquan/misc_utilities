%{

http://dspfirst.gatech.edu/chapters/DSP1st2eLabs/BPFnullingFilters.pdf

%}

%{

Given a set of FIR coefficients, freqz() returns the frequency response (magnitude and phase) of the FIR filter.

%}

pkg load signal;

f = [0.4, 0.6, 0.3, 0.2, 0.7, 0.8, 0.2];
w = -pi:(pi/100):pi;
H = freqz(f, 1, w);
subplot(2, 1, 1);
plot(w, abs(H));
subplot(2, 1, 2);
plot(w, angle(H));
xlabel('Normalized Radian Frequency');
pause;
