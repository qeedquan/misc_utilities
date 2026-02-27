package comets;

import java.lang.Math;

public class MediumComet extends comets.Comet
{
    static private final double radius = 30.0;
    static private final int number_of_comets = 3;
    public MediumComet(double xPos, double yPos, double xVel, double yVel) 
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

            SmallComet temp = new SmallComet(
                                getXPosition() + getRadius() * 2 * i, 
                                getYPosition(), 
                                xVelocity * t , yVelocity * -t); 
            vec.add(temp);
        }
        return vec;
    }

}
