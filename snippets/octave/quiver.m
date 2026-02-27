% quivers draws vectors of varying magnitude and direction in a vector field

[X,Y] = meshgrid(-pi:pi/8:pi, -pi:pi/8:pi);
U = sin(Y)*sin(X);
V = cos(X)*cos(Y);
quiver(X, Y, U, V, 'r');
waitforbuttonpress;

