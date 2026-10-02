package comets;

public class Shot extends comets.SpaceObject
{
    private int age = 0;
    static private final double radius = 3.0;
    public Shot(double xPos, double yPos, double xVel, double yVel) 
    {
        super(xPos, yPos, xVel, yVel, radius);
    }

    int getAge()
    {
        return age;
    }

    void move()
    {
        super.move();
        if (++age < 0)
            age = 0;
    }
}
