package comets;

import java.lang.*;

public abstract class SpaceObject extends java.lang.Object
{
    static double playfieldHeight;
    static double playfieldWidth;
    protected double xVelocity;
    protected double yVelocity;
    private double radius, xPos, yPos;

    public SpaceObject(double xPos, double yPos, double xVel, double yVel,
                       double radius) 
    {
        this.xVelocity = xVel;
        this.yVelocity = yVel;
        this.radius = radius;
        this.xPos = xPos;
        this.yPos = yPos;
    }

    double getRadius()
    {
        return radius;
    }

    double getXPosition()
    {
        return xPos;
    }

    double getYPosition()
    {
        return yPos;
    }

    void move()
    {
        xPos = (xPos + xVelocity); 
        yPos = (yPos + yVelocity); 
        if (xPos < 0)
            xPos = playfieldWidth;
        else if (xPos > playfieldWidth)
            xPos = 0;
        if (yPos < 0)
            yPos = playfieldHeight;
        else if (yPos > playfieldHeight)
            yPos = 0;
    }

    boolean overlapping(SpaceObject rhs)
    {
        double x = getXPosition() - rhs.getXPosition();
        double y = getYPosition() - rhs.getYPosition();
        if (Math.sqrt(x * x + y * y) < (getRadius() + rhs.getRadius()))
            return true;
        return false;
    }
}
