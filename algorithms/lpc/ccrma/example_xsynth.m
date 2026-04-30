pkg load signal;
pkg load audio;

close all; clear all; clc;

%% load audio
ic = 1; % choose the carrier signal from filec
filec = {'bubbles' 'creak' 'racing'};
filem = 'speech';

[xc, fs_c] = audioread(['audio/' filec{ic} '.wav']);
xc = mean(xc, 2); % single channel
xc = 0.9*xc/max(abs(xc)); % normalize linearly

[xm, fs_m] = audioread(['audio/' filem '.wav']);
xm = mean(xm, 2); % single channel
xm = 0.9*xm/max(abs(xm)); % normalize linearly
xm = resample(xm, round(fs_m*1.1), fs_m); % slow down the speech for better effect


%% match signal sample rates and lengths
fs = fs_m; % choose another sample rate if you want
xc = resample(xc, fs, fs_c);
nc = length(xc);
xm = resample(xm, fs, fs_m);
nm = length(xm);

n = min(nc, nm);
xc = xc(1:n);
xm = xm(1:n);


%% LPC encode 
pc = 30; % using higher order LPC for carrier (non-speech signal)
pm = 24; 
w = hann(floor(0.03*fs), 'periodic'); % using 30ms Hann window

[Ac, Gc, Ec] = lpcEncode(xc, pc, w);
[Am, Gm, Em] = lpcEncode(xm, pm, w);


%% X-Synth (LPC decode)
x_synth = lpcDecode(Am, Ec, w);
x_synth = 0.9*x_synth/max(abs(x_synth)); % normalize linearly


%% listen to resynthesized signal
% uncomment the lines below to play the estimated signal
% apXsynth = audioplayer(x_synth, fs);
% play(apXsynth);


%% save result to file
audiowrite(['output/xsynth_' filec{ic} '_' filem '.wav'], x_synth, fs);
