package comets;

import java.util.Vector;

public class SmallComet extends comets.Comet
{
    static private final double radius = 20.0;
    public SmallComet(double xPos, double yPos, double xVel, double yVel) 
    {
        super(xPos, yPos, xVel, yVel, radius);
    }

    public java.util.Vector<Comet> explode()
    {
        java.util.Vector<Comet> vec = new java.util.Vector(0);
        return vec;
    }
}
