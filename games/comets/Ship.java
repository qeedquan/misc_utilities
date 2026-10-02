package comets;

import java.lang.Math;

public class Ship extends comets.SpaceObject
{
    static private double angle = 0.0;
    static private final double radius = 10.0;
    static private final double turn_rate = 0.1;

    public Ship(double xPos, double yPos, double xVel, double yVel) 
    {
        super(xPos, yPos, xVel, yVel, radius);
    }

    void accelerate()
    {
        double s;
        xVelocity += 0.1 * Math.sin(angle);
        yVelocity += 0.1 * Math.cos(angle);
        s = Math.sqrt(xVelocity * xVelocity + yVelocity * yVelocity);
        if (s > 10)
        {
            double t = 10.0/s;
            xVelocity *= t;
            yVelocity *= t; 
        }
    }
    
    Shot fire()
    {
        Shot shot = new Shot(getXPosition(), getYPosition(), 
                             3 * Math.sin(angle) + xVelocity, 
                             3 * Math.cos(angle) + yVelocity);
        return shot;
    }

    double getAngle()
    {
        return angle;
    }

    void rotateLeft()
    {
        angle -= turn_rate;
        if (angle < 0)
            angle = 2 * Math.PI;
    }

    void rotateRight()
    {
        angle += turn_rate;
        if (angle > 2 * Math.PI)
            angle = 0.0;
    }
}
