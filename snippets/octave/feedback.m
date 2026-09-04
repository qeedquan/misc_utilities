pkg load control;

%{

Given a transfer function G and a feedback controller C the system becomes:

input -----> G ------> output
      ^----  C <---v


H = G/(1 + G*C) (negative feedback)
H = G/(1 - G*C) (positive feedback)

The equation above is not minimized however, the feedback() function minimizes it though (cancels out equal roots/poles)
We can use minreal() to minimize the system, it may not look exactly like feedback but the zpk data should be the same

%}

function analyze(G, C)
	G
	C
	% negative feedback
	H1 = feedback(G, C, -1)
	H2 = G/(1 + G*C)
	% minimized form, closer to what H1 is
	H3 = minreal(H2)
	[z, p, k] = zpkdata(H1)
	[z, p, k] = zpkdata(H2)
	[z, p, k] = zpkdata(H3)

	% positive feedback
	H1 = feedback(G, C, 1)
	H2 = G/(1 - G*C)
	% minimized form, looks closer to H1
	H3 = minreal(H2)
	[z, p, k] = zpkdata(H1)
	[z, p, k] = zpkdata(H2)
	[z, p, k] = zpkdata(H3)
end

G = tf([2 5 1], [1 2 3],'inputname',"torque",'outputname',"velocity");
C = tf([5 10], [1 10]); 
analyze(G, C);

% If we don't have a controller and only 1 transfer function
% The equation becomes H = G/(1 + G) or G/(1 - G) depending on the gain
[z, p, k] = zpkdata(feedback(G, -1))
[z, p, k] = zpkdata(minreal(G/(1 - G)))

[z, p, k] = zpkdata(feedback(G, 1))
[z, p, k] = zpkdata(minreal(G/(1 + G)))
