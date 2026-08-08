namespace DemoCalculatorTest
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.demoCalculator1 = new DemoCalculatorLib.DemoCalculator();
            this.SuspendLayout();
            // 
            // demoCalculator1
            // 
            this.demoCalculator1.Location = new System.Drawing.Point(0, 0);
            this.demoCalculator1.Margin = new System.Windows.Forms.Padding(4);
            this.demoCalculator1.Name = "demoCalculator1";
            this.demoCalculator1.Size = new System.Drawing.Size(1102, 489);
            this.demoCalculator1.TabIndex = 0;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(955, 503);
            this.Controls.Add(this.demoCalculator1);
            this.Name = "Form1";
            this.Text = "Calculator";
            this.ResumeLayout(false);

        }

        #endregion

        private DemoCalculatorLib.DemoCalculator demoCalculator1;
    }
}

