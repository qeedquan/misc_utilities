pkg load signal;

[Bb, Ab] = butter(4, 0.5) % order 4, cutoff at 0.5 * pi
Hb = freqz(Bb, Ab);
Db = grpdelay(Bb, Ab);

[Bc1, Ac1] = cheby1(4, 1, 0.5) % 1 dB passband ripple
Hc1 = freqz(Bc1, Ac1);
Dc1 = grpdelay(Bc1, Ac1);

[Bc2, Ac2] = cheby2(4, 20, 0.5) % 20 dB stopband attenuation
Hc2 = freqz(Bc2, Ac2);
Dc2 = grpdelay(Bc2, Ac2);

[Be, Ae] = ellip(4, 1, 20, 0.5)  % like cheby1 + cheby2
He = freqz(Be, Ae);
[De, w ] = grpdelay(Be, Ae);

figure(1);
plot(w, abs([Hb, Hc1, Hc2, He]));
grid('on');
xlabel('Frequency (rad/sample)');
ylabel('Gain');
legend('butter','cheby1','cheby2','ellip');
pause;

figure(2);
plot(w, [Db, Dc1, Dc2, De]);
grid('on');
xlabel('Frequency (rad/sample)'); ylabel('Delay (samples)');
legend('butter','cheby1','cheby2','ellip');
pause;
