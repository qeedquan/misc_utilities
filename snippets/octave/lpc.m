%{

https://www.phon.ucl.ac.uk/courses/spsci/dsp/lpc.html

Finds the coefficients of a pth-order linear predictor, an FIR filter that predicts the current value of the real-valued time series x based on past samples.

Given a system of the following form

y[n] = -Sum[k=1, p] b[k]*y[n-k]

We want recover the estimate the coefficients b[i] where the error is quadratically minimized

%}

pkg load signal;

noise = randn(50000,1);
x = filter(1, [1 1/2 1/3 1/4],noise);
x = x(end-4096+1:end);

a = lpc(x, 50);
est_x = filter([0 -a(2:end)],1,x);

a
max(est_x-x)

plot(1:100,x(end-100+1:end),1:100,est_x(end-100+1:end),'--')
grid
xlabel('Sample Number')
ylabel('Amplitude')
legend('Original signal','LPC estimate')
pause
