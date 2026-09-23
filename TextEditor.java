import javax.swing.*;
import javax.swing.filechooser.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import javax.swing.UIManager.*;

public class TextEditor implements ActionListener
{
    // the constants
    static final private String frame_title_str = "Text Editor";
    static final private String[] menu_str = { "File", "Edit" }; 
    static final private String[] file_menu_str = { "Open", "Save", 
                                      "Save as", "Exit" };
    static final private String[] edit_menu_str = { "Cut", "Copy", 
                                                    "Paste" };
    static final private String[] filter_ext_txt_str = { "txt" };
    static final private String[] button_str = { "Open", "Copy-and-Paste" };
    static final private int num_text_area = 2;

    // this is the gui stuff
    private JFrame frame;
    private JFileChooser file_chooser;
    private JMenuBar menu_bar;
    private JMenu[] menu;
    private JMenuItem[] file_menu_items;
    private JMenuItem[] edit_menu_items;
    private JTextArea[] text_area;
    private javax.swing.filechooser.FileFilter filter_extensions;
    private JButton[] button;
    private JPanel button_pane, text_pane, 
                   content_pane;
    private JScrollPane[] scroll_pane;

    // variables that is used in the program
    private String filename = "";

    public static void main(String[] args)
    {
        new TextEditor();
    }

    public void actionPerformed(ActionEvent e)
    {
        String action = e.getActionCommand();

        // user clicked the open button
        if (action.compareTo("Open") == 0)
        {
            // get the file to open
            int ret = file_chooser.showOpenDialog(frame);
            switch (ret)
            {
                case JFileChooser.APPROVE_OPTION:
                    text_area[0].setText(""); // clear text area first
                    String str = file_chooser.getSelectedFile()
                                             .getAbsolutePath();
                    Scanner ifp = openFile(str);
                    if (ifp != null)
                    {
                        // dump all the contents
                        // into the text area
                        while (ifp.hasNextLine())
                        {
                            String s = ifp.nextLine() + "\n";
                            text_area[0].append(s);
                        }
                    }

                    // if everything went well, 
                    // copy the filename into the internal buffer
                    // for later writing and set the title 
                    filename = str;
                    frame.setTitle(filename + " - " + frame_title_str);
                    ifp.close();
                    break;
            }
        }
        else if (action.compareTo("Save") == 0)
        {
            // save the file
            writeFile(filename);
        }
        else if (action.compareTo("Save as") == 0)
        {
            // get filename and then save the file
            int ret = file_chooser.showSaveDialog(frame);
            switch (ret)
            {
                case JFileChooser.APPROVE_OPTION:
                    String str = file_chooser.getSelectedFile()
                                             .getAbsolutePath();
                    String ext = file_chooser.getFileFilter()
                                             .getDescription();
                    
                    str = appendFileExtension(str, ext);
                    
                    if (writeFile(str) >= 0)
                    {
                        menuItemGrayed("Save", false);
                        filename = str;
                    }
                    break;
            }
        }
        else if (action.compareTo("Exit") == 0)
        {
            // user hit the exit menu item
            // so exit
            System.exit(0);
        }
        else if (action.compareTo("Cut") == 0)
        {
            // user hit the cut menu item, cutting
            // the text based on the selected text
            text_area[0].cut();
        }
        else if (action.compareTo("Copy") == 0)
        {
            // user hit copy menu item, copying
            // the text based on the selected text
            text_area[0].copy();
        }
        else if (action.compareTo("Paste") == 0)
        {
            // user hit the paste menu item, 
            // paste into the caret position
            text_area[0].paste();
        }
        else if (action.compareTo("Copy-and-Paste") == 0)
        {
            // copy and paste button
            // used to copy the text from one text area
            // into another one, where the other is non editable
            text_area[1].setText(text_area[0].getSelectedText());
        }
    }
    
    // open a file for reading
    private Scanner openFile(String str)
    {
        Scanner file;
        try
        {
            file = new Scanner(new File(str));
        }
        catch (FileNotFoundException e)
        {
            JOptionPane.showMessageDialog(frame,
                    "Error opening the file: " + e.getMessage(),
                    "Error", JOptionPane.ERROR_MESSAGE);
                    return null;
        }
        return file;
    }

    // write the text area contents into
    // a file
    private int writeFile(String str)
    {
        try 
        { 
            File ofp = new File(str);
            ofp.createNewFile();
            FileWriter writer = new FileWriter(ofp);
            BufferedWriter out = new BufferedWriter(writer);

            out.write(text_area[0].getText()); 
            out.close(); 
        } 
        catch (IOException e)
        { 
            JOptionPane.showMessageDialog(frame,
                    "Error saving the file " + str + ": " + e.getMessage(),
                    "Error", JOptionPane.ERROR_MESSAGE);
            return -1;
        } 
        return 0;
    }

    // gray out a menu item
    private void menuItemGrayed(String str, boolean b)
    {
        for (int i = 0; i != file_menu_str.length; ++i)
        {
            if (file_menu_str[i].compareTo(str) == 0)
            {
                file_menu_items[i].setEnabled(!b);
                return;
            }
        }
        
        for (int i = 0; i != edit_menu_str.length; ++i)
        {
            if (edit_menu_str[i].compareTo(str) == 0)
            {
                edit_menu_items[i].setEnabled(!b);
                return;
            }
        }
    }

    // add a file extension based on the filtering
    // ie, if the user save "a" and he chosed save 
    // as textfiles, it will be saved as "a.txt"
    private String appendFileExtension(String str, String ext)
    {
        String ret = new String(str);
        if (ext.compareTo("Text Files") == 0)
            return ret + ".txt";
        
        return ret;
    }

    // the constructor that inits everything
    TextEditor()
    {   
        content_pane = new JPanel(new BorderLayout());
        
        // make the button panes
        button_pane = new JPanel(); 
        button_pane.setLayout(new BoxLayout(button_pane, 
                                   BoxLayout.LINE_AXIS));
        button_pane.setBorder(BorderFactory.createEmptyBorder
                                    (10, 10, 10, 10));

        // text area panes
        text_pane = new JPanel();
        text_pane.setLayout(new BoxLayout(text_pane, 
                                   BoxLayout.LINE_AXIS));
        
        // the window itself
        frame = new JFrame(frame_title_str);
        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        frame.setLocation(200, 200);
        
        // allocate every other gui stuff we will use
        file_chooser = new JFileChooser();
        menu_bar = new JMenuBar();
        menu = new JMenu[menu_str.length];
        file_menu_items = new JMenuItem[file_menu_str.length];
        edit_menu_items = new JMenuItem[edit_menu_str.length];
        filter_extensions = new ExtensionFileFilter("Text Files", 
                                    filter_ext_txt_str);
        button = new JButton[button_str.length];

        // this is for the menu bar such as "file" and "edit"
        for (int i = 0; i != menu_str.length; ++i)
        {
            menu[i] = new JMenu(menu_str[i]);
            menu_bar.add(menu[i]);
        }
        
        // this sets up the file menu items into the menu bar
        for (int i = 0; i != file_menu_str.length; ++i)
        {
            file_menu_items[i] = new JMenuItem(file_menu_str[i]);
            file_menu_items[i].addActionListener(this);
            menu[0].add(file_menu_items[i]);
        }

        // sets up the edit menu items into the menu bar
        for (int i = 0; i != edit_menu_str.length; ++i)
        {
            edit_menu_items[i] = new JMenuItem(edit_menu_str[i]);
            edit_menu_items[i].addActionListener(this);
            menu[1].add(edit_menu_items[i]);
        }

        // use that menu bar
        frame.setJMenuBar(menu_bar);
        
        // setup the buttons
        for (int i = 0; i != button_str.length; ++i)
        {
            button[i] = new JButton(button_str[i]);
            button[i].addActionListener(this);
            button_pane.add(button[i]);
            // give buttons some whitespace between each other
            button_pane.add(Box.createRigidArea(new Dimension(10, 0)));
        }

        // this is the text area
        scroll_pane = new JScrollPane[num_text_area];
        text_area = new JTextArea[num_text_area];
        for (int i = 0; i != num_text_area; ++i)
        {
            text_area[i] = new JTextArea(30, 30);
            scroll_pane[i] = new JScrollPane(text_area[i]);
            scroll_pane[i].setVerticalScrollBarPolicy(
                        JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
            scroll_pane[i].setHorizontalScrollBarPolicy(
                        JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);

            text_pane.add(scroll_pane[i]);

            if (i > 0)
                text_area[i].setEditable(false);
        }
        
        // combine it all into the content pane
        // and display it.

        // The content pane will be 2 boxlayout panes 
        // into a borderlayout.
        content_pane.add(button_pane, BorderLayout.PAGE_START);
        content_pane.add(text_pane, BorderLayout.CENTER);

        // add it to the window
        frame.add(content_pane); 
         
        // setup the defaults and show the window
        menuItemGrayed("Save", true);
        file_chooser.setFileFilter(filter_extensions);
        frame.pack();
        
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

// this class is used for jfilechooser to filter
// out text only files
class ExtensionFileFilter extends javax.swing.filechooser.FileFilter 
{
    String description;
    String[] extensions;

    public ExtensionFileFilter(String desc, String[] ext) 
    {
        description = desc;
        extensions = ext;
    }

    public String getDescription() 
    {
        return description;
    }

    public boolean accept(File file) 
    {
        if (file.isDirectory()) 
            return true;
        else 
        {
            String ext = file.getAbsolutePath().toLowerCase();
            for (int i = 0; i != extensions.length; ++i)
            {
                if (ext.endsWith(extensions[i]))
                    return true;
            }
        }
        return false;
    }
}

