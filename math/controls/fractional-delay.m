%{

https://en.wikipedia.org/wiki/Pad%C3%A9_approximant
https://en.wikipedia.org/wiki/Pad%C3%A9_table
https://www.mathworks.com/help/control/ref/dynamicsystem.pade.html

To create fractional delay in an LTI system:

H(s) = some transfer function
G(s) = exp(-delay*s) * H(s)

If we do not have access to the exp(s) we can use a series to approximate it
A naive way is to use the taylor expansion of exp(s), the problem with this is that it introduces more zeros than poles so the system will be thought of as unstable
A better way is to use the pade approximation to exp(s) since pade generate a rational function, making the zeros and poles roughly equal

This pade approximation introduces a time delay but also alot of ringing for lower order systems (1st order)
For higher order system the ringing is less since the dominant poles are from the transfer function and not from the exp decay term

%}

pkg load control;

function r = padeexp(x)
	p = 1 + 1/2*x + 1/9*x^2 + 1/72*x^3 + 1/1008*x^4 + 1/30240*x^5
	q = 1 - 1/2*x + 1/9*x^2 - 1/72*x^3 + 1/1008*x^4 - 1/30240*x^5
	r = p/q
end

s = tf('s');
G = 1/(s + 10)
G1 = padeexp(-2.5*s)*G
G2 = padeexp(-3.5*s)*G
G3 = padeexp(-4.5*s)*G
G4 = padeexp(-5.5*s)*G
G5 = padeexp(-6.5*s)*G
G6 = padeexp(-7.5*s)*G
step(G, G1, G2, G3, G4, G5, G6);
pause;

G = 1/((s+0.6)*(s+0.7))
G1 = padeexp(-2.5*s)*G
G2 = padeexp(-3.5*s)*G
G3 = padeexp(-4.5*s)*G
G4 = padeexp(-5.5*s)*G
G5 = padeexp(-6.5*s)*G
G6 = padeexp(-7.5*s)*G
step(G, G1, G2, G3, G4, G5, G6);
pause;

G = 1/((s+0.6)*(s+0.7)*(s+0.9)*(s+10))
G1 = padeexp(-2.5*s)*G
G2 = padeexp(-3.5*s)*G
G3 = padeexp(-4.5*s)*G
G4 = padeexp(-5.5*s)*G
G5 = padeexp(-6.5*s)*G
G6 = padeexp(-7.5*s)*G
step(G, G1, G2, G3, G4, G5, G6);
pause;
