% https://www.astro.rug.nl/~vdhulst/SignalProcessing/Hoorcolleges/college08.pdf

pkg load signal;

% Transfer function is
% H(z) = (-0.2 + 0.18*z^-1 + 0.4*z^-2 + z^-3) / (1 + 0.4*z^1 + 0.18*z^2 - 0.2*z^-3)
% Note that in the z-domain, we can just read off the coefficients as numerators/denominators
% All pass example
b = [-0.2 0.18 0.4 1];
a = [1 0.4 0.18 -0.2];
[H w]=freqz(b, a, 1024);

subplot(211)
plot(w/pi, abs(H));
title('Magnitude response of the all pass filter')
grid
subplot(212)
plot(w/pi, angle(H));
title('Phase response of the all pass filter')
grid
xlabel('Normalized angular frequency, \omega/\pi')
figure

zplane(b,a)

pause;

