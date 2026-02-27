args = argv();
if (length(args) < 3)
	printf("usage: length mu sigma\n");
	exit(1);
end

L = str2num(args{1});
mu = str2num(args{2});
sigma = str2num(args{3});
X = sigma*randn(L, 1) + mu;

figure();
subplot(2, 1, 1);
plot(X);
title(['White noise : L=', num2str(length(X)), ' \mu_x=', num2str(mu), ' \sigma^2=', num2str(sigma^2)]);
xlabel('Samples');
ylabel('Values');
grid on;
pause;
