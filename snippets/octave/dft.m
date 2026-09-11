%{

https://www.gaussianwaves.com/2015/11/interpreting-fft-results-obtaining-magnitude-and-phase-information/

%}

A = 5;
fc = 10;
phase = 30;
fs = 32 * fc;
t = 0: 1/fs : 2-1/fs;
phi = rad2deg(phase);
x = A*cos(2*pi*fc*t + phi) + A*sin(2*pi*2*fc*t + phi) + A*0.8*cos(2*pi*fc/4*t + 2*phi);
figure;
subplot(3, 1, 1);
plot(t, x);

N = 256;
X = 1/N * fftshift(fft(x, N));
df = fs/N;
si = -N/2 : N/2-1;
f = si * df;
subplot(3, 1, 2);
stem(f, abs(X));
xlabel('f (Hz)');
ylabel('|X(k)|');

X2 = X;
threshold = max(abs(X))/10000;
X2(abs(X) < threshold) = 0;
phase = rad2deg(atan2(imag(X2), real(X2)));
subplot(3, 1, 3);
plot(f, phase);

pause
