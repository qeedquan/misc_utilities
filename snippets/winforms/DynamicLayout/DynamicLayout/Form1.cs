using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DynamicLayout
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            InitializeDynamicControls();
        }

        void ParseLayout(FlowLayoutPanel panel, string layout, string[] table)
        {
            panel.SuspendLayout();

            Control control = null;
            var bigFont = new System.Drawing.Font("Comic Sans MS", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            var normalFont = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            var elementHeight = 30;
            var td = 0;
            foreach (var ch in layout)
            {
                switch (ch)
                {
                    case 'h':
                        var label = new Label();
                        label.Font = bigFont;
                        label.Text = table[td++];
                        label.AutoSize = true;
                        panel.Controls.Add(label);

                        control = label;
                        break;

                    case 'b':
                        var button = new Button();
                        button.Font = normalFont; 
                        button.Text = table[td++];
                        button.Size = new Size(230, elementHeight);
                        button.BackColor = Color.Azure;
                        panel.Controls.Add(button);

                        control = button;
                        break;

                    case 'n':
                        var labelButton = new Button();
                        labelButton.Font = normalFont;
                        labelButton.Text = "100";
                        labelButton.AutoSize = true;
                        labelButton.MinimumSize = new Size(0, elementHeight);
                        labelButton.Enabled = false;
                        labelButton.BackColor = Color.Azure;
                        panel.Controls.Add(labelButton);

                        label = new Label();
                        label.Font = normalFont;
                        label.Text = table[td++];
                        label.Padding = new Padding(0, elementHeight/2-5, 0, 0);
                        label.AutoSize = true;
                        panel.Controls.Add(label);

                        control = label;
                        break;

                    case 'p':
                        label = new Label();
                        label.MinimumSize = new Size(200, elementHeight);
                        panel.Controls.Add(label);

                        control = label;
                        break;

                    case 'r':
                        var radio = new RadioColorButton();
                        radio.Text = "";
                        radio.ButtonColor = Color.DarkGray;
                        radio.Enabled = false;
                        radio.FlatAppearance.BorderSize = 0;
                        radio.FlatStyle = FlatStyle.Flat;
                        radio.Size = new Size(elementHeight, elementHeight);
                        panel.Controls.Add(radio);

                        label = new Label();
                        label.Font = normalFont;
                        label.Padding = new Padding(0, elementHeight / 2 - 5, 0, 0);
                        label.Text = table[td++];
                        label.AutoSize = true;
                        panel.Controls.Add(label);

                        control = label;
                        break;

                    case '\n':
                        panel.SetFlowBreak(control, true);
                        continue;
                }
            }

            panel.ResumeLayout();
        }

        void InitializeDynamicControls()
        {
            var layout = "h\nbnpbbb\nr";
            string[] table =
            {
                "Form And Space",
                "Button 1", "Number of Devices", "Hello", "XYZ", "Lines and Washbasins",
                "Radiohead",
            };

            ParseLayout(flowLayoutPanel1, layout, table);
        }
    }
}
