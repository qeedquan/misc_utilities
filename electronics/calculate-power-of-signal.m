%{

https://www.mathworks.com/help/signal/ug/measure-the-power-of-a-signal.html

Average power of a signal is rms(signal)^2

To convert power to DB:
10*log10(power/ref)
ref is 1 usually

%}

pkg load signal;

N = 1200;
Fs = 1000;
t = (0:N-1)/Fs;

sigma = 0.01;

s = chirp(t,100,1,300)+sigma*randn(size(t));
rms(s)^2

s = 10*sin(2*pi*100*t);
rms(s)^2

% for constant signal, power is magnitude^2
s = 5*ones(size(t));
rms(s)^2

10*log10(1.5777)
10*log10(0.13141)
10*log10(0.04672)
