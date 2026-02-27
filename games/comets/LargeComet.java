package comets;

import java.util.Vector;
import java.lang.Math;

public class LargeComet extends comets.Comet
{
    static private final double radius = 40.0;
    static private final int number_of_comets = 2;
    public LargeComet(double xPos, double yPos, double xVel, double yVel) 
    {
        super(xPos, yPos, xVel, yVel, radius);
    }

    public java.util.Vector<Comet> explode()
    {
        java.util.Vector<Comet> vec = new java.util.Vector();
        for (int i = 0; i != number_of_comets; ++i)
        {
            double t = 1;
            if (Math.random() < 0.5)
                t = -1;
            MediumComet temp = new MediumComet(
                                getXPosition() + getRadius() * 2 * i, 
                                getYPosition(), 
                                xVelocity * t , yVelocity * -t); 
            vec.add(temp);
        }
        return vec;
    }
}
