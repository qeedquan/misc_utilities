using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ScrollTest
{
    public partial class Form1 : Form
    {
        int counter = 0;

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            for (var i = 0; i < 100; i++)
            {
                var label = new Label();
                label.Text = String.Format("Flow Counter {0}", ++counter);

                var button = new Button();
                button.Text = "Button";

                var textbar = new TextBox();

                flowLayoutPanel1.Controls.Add(label);
                flowLayoutPanel1.Controls.Add(button);
                flowLayoutPanel1.Controls.Add(textbar);
                flowLayoutPanel1.SetFlowBreak(textbar, true);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            flowLayoutPanel1.Controls.Clear();
        }
    }
}
