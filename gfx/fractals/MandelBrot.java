import java.lang.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.UIManager.*;

public class MandelBrot implements ActionListener, ComponentListener, 
                                   MouseListener
{
    // default frame stuff
    static private int window_width = 800, window_height = 600,
                  generate_button_width = 50, generate_button_height = 50,
                  canvas_width = 550, canvas_height = 600;

    // textfield stuff
    static private int seq_len;
    static private double real_center, imag_center, view_size;
    static private int palette[];

    // GUI stuff
    private Canvas canvas; //canvas to draw the set
    private JFrame frame; 
    private JPanel panel;
    private JButton[] button;
    private JLabel[] label;
    private JTextField[] textfield;
    private GridBagConstraints c;
 
    // strings for the textfields/labels
    private String[] button_str = { "Generate" };
    private String[] textfield_str = { "Real center",
                                       "Imaginary center", 
                                       "View size", 
                                       "Sequence Length" };
    private String[] default_text = { "-0.4", "0", "1.1", "50" };

    public static void main(String args[])
    {
        new MandelBrot();
    }
    // action performed when the button generate is clicked    
	public void actionPerformed(ActionEvent e)
    {
        String action = e.getActionCommand();

        if (action.compareTo("Generate") == 0)
        {
            try
            {
                // for some reason, a negative number throws 
                // an exception in parse* functions, so do this
                // to make sure no exception gets thrown if its just
                // a negative sign in front
                String[] str = new String[textfield_str.length];
                for (int i = 0; i != textfield_str.length; ++i)
                {
                    if (textfield[i].getText().charAt(0) == '-')
                        str[i] = new String(textfield[i].getText().
                                            substring(1));
                    else
                        str[i] = new String(textfield[i].getText());
                }
                
                real_center = Double.parseDouble(str[0]);
                imag_center = Double.parseDouble(str[1]);
                view_size   = Double.parseDouble(str[2]);
                seq_len     = Integer.parseInt(str[3]);
            }
            catch (NumberFormatException ex)
            {
                // for some reason it parses the negative sign properly
                // but it throws an exception, so don't bother handling the 
                // error if nothing really went wrong
                 
                JOptionPane.showMessageDialog(frame,
                    "Error parsing one of the fields: " + ex.getMessage(),
                    "Error", JOptionPane.ERROR_MESSAGE);
                    return;
            }
            Draw();
        }
    }

    // these empty fields need to be here to get it to compile
    public void componentHidden(ComponentEvent e) 
    {
    }

    public void componentMoved(ComponentEvent e) 
    {
    }

    // when the user resize
    public void componentResized(ComponentEvent e) 
    {
        Dimension d = e.getComponent().getSize(); 
        int w = d.width, h = d.height;
        canvas_width = w - panel.getSize().width - 5;
        canvas_height = (int) ((canvas_height / (window_height * 1.0)) * h);
        window_width = w;
        window_height = h;
    }

    public void componentShown(ComponentEvent e) 
    {

    }

    // when a mouse is press on the canvas surface, it gets
    // the complex coordinates of the mandelbrot set.
    public void mousePressed(MouseEvent e) 
    {
        double a0, b0;
        int x = e.getX(), y = e.getY();

        a0 = real_center;
        b0 = imag_center;
        
        Double real = (a0 - view_size) + (2 * ((view_size * x) 
                       / (canvas_width * 1.0)));

        Double imag = (b0 + view_size) - (2 * ((view_size * y) 
                       / (canvas_height * 1.0)));
                
        textfield[0].setText(real.toString());
        textfield[1].setText(imag.toString());
    }

    public void mouseReleased(MouseEvent e)
    {
    }

    public void mouseEntered(MouseEvent e) 
    {
    }

    public void mouseExited(MouseEvent e) 
    {
    }

    public void mouseClicked(MouseEvent e) 
    {
    }

    // draw the mandelbrot set
    public void Draw()
    {
        Graphics g = canvas.getGraphics();
                 
        double a0, b0, real, imag;
        Color color;
        ComplexNumber n;
        a0 = real_center;
        b0 = imag_center;
            
        palette = new int[seq_len];
        for (int i = 0; i != palette.length; ++i)
               palette[i] = (int) (Math.random() * 0xFFFFFF);
               
        for (int y = 0; y != canvas_height; ++y)
        {
            for (int x = 0; x != canvas_width; ++x)
            {
                real = (a0 - view_size) + (2 * ((view_size * x) 
                       / (canvas_width * 1.0)));

                imag = (b0 + view_size) - (2 * ((view_size * y) 
                       / (canvas_height * 1.0)));
                
                n = new ComplexNumber(real, imag);
                int steps = Divergent(n);
                if (steps > -1)
                    color = new Color(palette[steps]);
                else
                    color = new Color(0);

                g.setColor(color); 
                g.fillRect(x, y, 1, 1);
            }
        }
    }

    // does the number diverge? 
    public int Divergent(ComplexNumber z)
    {
        ComplexNumber z0 = new ComplexNumber(z.real(), z.imag());
        for (int i = 0; i < seq_len; i++)
        {
            if (z.magnitude() > 2)
                return i;
            z = z0.add(z.square());
        }
        return -1;
    }

    // constructor to init the window
    public MandelBrot()
    {
        //make the window
        frame = new JFrame("Mandelbrot Set");
        canvas = new Canvas();
        panel = new JPanel();
        c = new GridBagConstraints();

        canvas.addMouseListener(this);
        canvas.setBackground(Color.BLACK);
        canvas.setPreferredSize(new Dimension(canvas_width, canvas_height));
        panel.setLayout(new GridBagLayout());
        
        frame.setLayout(new BorderLayout());
        frame.setSize(window_width, window_height);
        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        frame.add(canvas, BorderLayout.CENTER);
        frame.add(panel, BorderLayout.EAST);
        frame.setLocation(200, 200);
        frame.addComponentListener(this);  

        //make the buttons
        button = new JButton[button_str.length];
        for (int i = 0; i != button_str.length; ++i)
        {
            button[i] = new JButton(button_str[i]);
            button[i].setActionCommand(button_str[i]);
            button[i].addActionListener(this);
        }

        // make the labels and textfield
        label     = new JLabel[textfield_str.length];
        textfield = new JTextField[textfield_str.length];
        for (int i = 0; i != textfield_str.length; ++i)
        {
            label[i] = new JLabel(textfield_str[i]);
            textfield[i] = new JTextField(default_text[i], 10);
            textfield[i].setActionCommand(textfield_str[i]);
            
            c.ipadx = 5;
            c.gridwidth = GridBagConstraints.RELATIVE; // next-to-last
            c.fill = GridBagConstraints.NONE;         // reset to default
            c.weightx = 0.0;                          // reset to default
            panel.add(label[i], c);
             
            c.gridwidth = GridBagConstraints.REMAINDER; //end row
            c.fill = GridBagConstraints.HORIZONTAL;
            c.weightx = 1.0;
            panel.add(textfield[i], c);
        }
        for (int i = 0; i != button_str.length; ++i)
        {
            c.gridx = 50; // move the button to the right for alignment
            c.fill = GridBagConstraints.HORIZONTAL;
            panel.add(button[0], c);
        }
				
        try
        {
            for (LookAndFeelInfo info : UIManager.getInstalledLookAndFeels())
            {
                if ("Nimbus".equals(info.getName()))
                {
                    UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
            SwingUtilities.updateComponentTreeUI(frame);
        }
        catch (Exception e)
        {
            System.out.println("can't set system look and feel: " + e.getMessage());
        }
		
        frame.setVisible(true);
    }
}

class ComplexNumber
{
    private final double r, i;

    public ComplexNumber(double real, double imag)
    {
        r = real;
        i = imag;
    }
    
    public double real()
    {
        return r;
    }

    public double imag()
    {
        return i;
    }

    // square the complex number
    public ComplexNumber square()
    {
        return new ComplexNumber( (r * r - i * i), (2 * r * i) );
    }
    // get the magnitude of the complex number
    public double magnitude()
    {
        return Math.sqrt(r * r + i * i); 
    }

    // add 2 complex number
    public ComplexNumber add(ComplexNumber n)
    {
        return new ComplexNumber(r + n.real(), i + n.imag());
    }
}
