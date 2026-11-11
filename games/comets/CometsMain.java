package comets;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import java.util.*;
import java.io.*;

// This class is primarily responsible for organizing the game of Comets
public class CometsMain implements KeyListener
{
	// GUI Data
	private JFrame frame; // The window itself
	private Canvas playArea;  // The area where the game takes place
	
	private final int playWidth = 500; // The width of the play area (in pixels)
	private final int playHeight = 500; // The height of the play area (in pixels)
	
	// Game Data
	private Ship ship; // The ship in play
	private Vector<Shot> shots; // The shots fired by the player
	private Vector<Comet> comets; // The comets floating around
	
	private boolean shipDead; // Whether or not the ship has been blown up
	private long shipTimeOfDeath; // The time at which the ship blew up
	
	// Keyboard data
	// Indicates whether the player is currently holding the accelerate, turn
	// left, or turn right buttons, respectively
	private boolean accelerateHeld = false;
	private boolean turnLeftHeld = false;
	private boolean turnRightHeld = false;
	
	// Indicates whether the player struck the fire key
	private boolean firing = false;
	
	// Set up the game and play!
	public CometsMain()
	{
		// Get everything set up
		configureGUI();
		configureGameData();
		
		// Display the window so play can begin
		frame.setVisible(true);
		
		// Start the gameplay
		playGame();
	}
	
	// Set up the initial positions of all space objects
	private void configureGameData()
	{
		// Configure the play area size
		SpaceObject.playfieldWidth = playWidth;
		SpaceObject.playfieldHeight = playHeight;
		
		// Create the ship
		ship = new Ship(playWidth/2, playHeight/2, 0, 0);
		
		// Create the shot vector (initially, there shouldn't be any shots on the screen)
		shots = new Vector<Shot>();
		
		// Read the comets from comets.cfg
		comets = new Vector<Comet>();
		
		try
		{
			Scanner fin = new Scanner(new File("comets.cfg"));
			
			// Loop through each line of the file to read a comet
			while(fin.hasNext())
			{
				String cometType = fin.next();
				double xpos = fin.nextDouble();
				double ypos = fin.nextDouble();
				double xvel = fin.nextDouble();
				double yvel = fin.nextDouble();
				
				if(cometType.equals("Large"))
					comets.add(new LargeComet(xpos, ypos, xvel, yvel));
				else if(cometType.equals("Medium"))
					comets.add(new MediumComet(xpos, ypos, xvel, yvel));
				else 
					comets.add(new SmallComet(xpos, ypos, xvel, yvel));
			}
		}
		// If the file could not be read correctly for whatever reason, abort
		// the program
		catch(FileNotFoundException e)
		{
			System.err.println("Unable to locate comets.cfg");
			System.exit(0);
		}
		catch(Exception e)
		{
			System.err.println("comets.cfg is not in a proper format");
			System.exit(0);
		}
	}
	
	// Set up the game window
	private void configureGUI()
	{
		// Create the window object
		frame = new JFrame("Comets");
		frame.setSize(playWidth+20, playHeight+35);
		frame.setResizable(false);
		
		// The program should end when the window is closed
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// Set the window's layout manager
		frame.setLayout(new FlowLayout());
		
		// Create the play area
		playArea = new Canvas();
		playArea.setSize(playWidth, playHeight);
		playArea.setBackground(Color.BLACK);
		playArea.setFocusable(false);
		frame.add(playArea);
		
		// Make the frame listen to keystrokes
		frame.addKeyListener(this);
	}
	
	// The main game loop. This method coordinates everything that happens in
	// the game
	private void playGame()
	{
		while(true)
		{
			// Measure the current time in an effort to keep up a consistent
			// frame rate
			long time = System.currentTimeMillis();
			
			// If the ship has been dead for more than 3 seconds, revive it
			if(shipDead && shipTimeOfDeath + 3000 < time)
			{
				shipDead = false;
				ship = new Ship(playWidth/2, playHeight/2, 0, 0);
			}
			
			// Process game events, move all the objects floating around,
			// and update the display
			if(!shipDead)
				handleKeyEntries();
			handleCollisions();
			moveSpaceObjects();
			
			// Sleep until it's time to draw the next frame 
			// (i.e. 32 ms after this frame started processing)
			try
			{
				long delay = Math.max(0, 32-(System.currentTimeMillis()-time));				
				Thread.sleep(delay);
			}
			catch(InterruptedException e)
			{
			}
			
		}
	}

	// Deal with objects hitting each other
	private void handleCollisions()
	{
		// Anything that is destroyed should be erased, so get ready
		// to erase stuff
		Graphics g = playArea.getGraphics();
		g.setColor(Color.BLACK);
		
		// Deal with shots blowing up comets
		for(int i = 0; i < shots.size(); i++)
		{
			Shot s = shots.elementAt(i);
			for(int j = 0; j < comets.size(); j++)
			{
				Comet c = comets.elementAt(j);
				
				// If a shot has hit a comet, destroy both the shot and comet
				if(s.overlapping(c))
				{
					// Erase the bullet
					shots.remove(i);
					i--;
					this.drawSpaceObject(g, s);
					
					// If the comet was actually destroyed, replace the comet
					// with the new comets it spawned (if any)
					Vector<Comet> newComets = c.explode();
					if(newComets != null)
					{
						this.drawSpaceObject(g, c);
						comets.remove(j);
						j--;
						comets.addAll(newComets);		
					}
					break;
				}
			}
		}
		
		// Deal with comets blowing up the ship
		if(!shipDead)
		{
			for(Comet c : comets)
			{
				// If the ship hit a comet, kill the ship and mark down the time 
				if(c.overlapping(ship))
				{
					shipTimeOfDeath = System.currentTimeMillis();
					shipDead = true;
					drawShip(g, ship);
				}
			}
		}
	}
	
	// Check which keys have been pressed and respond accordingly
	private void handleKeyEntries()
	{
		// Ship movement keys
		if(accelerateHeld)
			ship.accelerate();
	
		// Shooting the cannon
		if(firing)
		{
			firing = false;
			shots.add(ship.fire());
		}
	}
	
	// Deal with moving all the objects that are floating around
	private void moveSpaceObjects()
	{
		Graphics g = playArea.getGraphics();
		
		// Handle the movements of all objects in the field
		if(!shipDead)
			updateShip(g);
		updateShots(g);
		updateComets(g);		
	}
	
	// Move all comets and draw them to the screen
	private void updateComets(Graphics g)
	{
		for(Comet c : comets)
		{
			// Erase the comet at its old position
			g.setColor(Color.BLACK);
			drawSpaceObject(g, c);
			
			// Move the comet to its new position
			c.move();
			
			// Draw it at its new position
			g.setColor(Color.CYAN);
			drawSpaceObject(g, c);
			
		}
	}
	
	// Move all shots and draw them to the screen
	private void updateShots(Graphics g)
	{
		
		for(int i = 0; i < shots.size(); i++)
		{
			Shot s = shots.elementAt(i);
			
			// Erase the shot at its old position
			g.setColor(Color.BLACK);
			drawSpaceObject(g, s);
			
			// Move the shot to its new position
			s.move();
			
			// Remove the shot if it's too old
			if(s.getAge() > 180)
			{
				shots.remove(i);
				i--;
			}
			// Otherwise, draw it at its new position
			else
			{
				g.setColor(Color.RED);
				drawSpaceObject(g, s);
			}		
		}
	}
	
	// Draws the space object s to the the specified graphics context
	private void drawSpaceObject(Graphics g, SpaceObject s)
	{
		// Figure out where the object should be drawn
		int radius = (int)s.getRadius();
		int xCenter = (int)s.getXPosition();
		int yCenter = (int)s.getYPosition();
		
		// Draw the object
		g.drawOval(xCenter - radius, yCenter - radius, radius*2, radius*2);
	}
	
	// Moves the ship and draws it at its new position
	private void updateShip(Graphics g)
	{
		// Erase the ship at its old position
		g.setColor(Color.BLACK);
		drawShip(g, ship);

		// Ship rotation must be handled between erasing the ship at its old position
		// and drawing it at its new position so that artifacts aren't left on the screen
		if(turnLeftHeld)
			ship.rotateLeft();
		if(turnRightHeld)
			ship.rotateRight();
		ship.move();
		
		// Draw the ship at its new position
		g.setColor(Color.WHITE);
		drawShip(g, ship);
	}
	
	// Draws this ship s to the specified graphics context
	private void drawShip(Graphics g, Ship s)
	{
		// Figure out where the ship should be drawn
		int radius = (int)s.getRadius();
		int xCenter = (int)s.getXPosition();
		int yCenter = (int)s.getYPosition();
		
		// Draw the ship body
		g.drawOval(xCenter - radius, yCenter - radius, radius*2, radius*2);
		
		// Draw the gun turret
		int guntipXoffset = (int)(radius * 1.5 * Math.sin(s.getAngle()));
		int guntipYoffset = (int)(radius * 1.5 * Math.cos(s.getAngle()));
		g.drawLine(xCenter, yCenter, xCenter + guntipXoffset, yCenter + guntipYoffset);
	}
		
	// Deals with keyboard keys being pressed
	public void keyPressed(KeyEvent key)
	{
		// Mark down which important keys have been pressed
		if(key.getKeyCode() == KeyEvent.VK_UP)
			this.accelerateHeld = true;
		if(key.getKeyCode() == KeyEvent.VK_LEFT)
			this.turnLeftHeld = true;
		if(key.getKeyCode() == KeyEvent.VK_RIGHT)
			this.turnRightHeld = true;
		if(key.getKeyCode() == KeyEvent.VK_SPACE)
			this.firing = true;
	}

	// Deals with keyboard keys being released
	public void keyReleased(KeyEvent key)
	{
		// Mark down which important keys are no longer being pressed
		if(key.getKeyCode() == KeyEvent.VK_UP)
			this.accelerateHeld = false;
		if(key.getKeyCode() == KeyEvent.VK_LEFT)
			this.turnLeftHeld = false;
		if(key.getKeyCode() == KeyEvent.VK_RIGHT)
			this.turnRightHeld = false;
	}

	// This method is not actually used, but is required by the KeyListener interface
	public void keyTyped(KeyEvent arg0)
	{
	}
	
	
	public static void main(String[] args)
	{
		// A GUI program begins by creating an instance of the GUI
		// object. The program is event driven from that point on.
		new CometsMain();
	}

}
