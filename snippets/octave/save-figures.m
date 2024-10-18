% Example to save figures generated to a file

pkg load control;

hf = figure('visible', 'off');
plot(rand(5, 5));
saveas(hf, 'out.png', 'png');

clf;
s = tf('s');
G = 1/(s^2 + s + 1);
bode(G);
saveas(hf, 'bode.png', 'png');

