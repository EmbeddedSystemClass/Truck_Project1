namespace EpServerEngineSampleClient
{
    partial class DlgSetParams
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
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.UpDwnFanOnTemp = new System.Windows.Forms.NumericUpDown();
            this.UpDwnEnableBlrTemp = new System.Windows.Forms.NumericUpDown();
            this.UpDwnFanOffTemp = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.UpDwnBlr3On = new System.Windows.Forms.NumericUpDown();
            this.UpDwnBlr2On = new System.Windows.Forms.NumericUpDown();
            this.UpDwnBlr1On = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.UpDwnHighRev = new System.Windows.Forms.NumericUpDown();
            this.UpDwnLowRev = new System.Windows.Forms.NumericUpDown();
            this.UpDwnFPGAXmitRate = new System.Windows.Forms.NumericUpDown();
            this.UpDwnLightOn = new System.Windows.Forms.NumericUpDown();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnFanOnTemp)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnEnableBlrTemp)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnFanOffTemp)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnBlr3On)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnBlr2On)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnBlr1On)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnHighRev)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnLowRev)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnFPGAXmitRate)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnLightOn)).BeginInit();
            this.SuspendLayout();
            // 
            // btnOK
            // 
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Location = new System.Drawing.Point(498, 400);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 0;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.OKClicked);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(417, 400);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.CancelClick);
            // 
            // UpDwnFanOnTemp
            // 
            this.UpDwnFanOnTemp.Location = new System.Drawing.Point(149, 28);
            this.UpDwnFanOnTemp.Name = "UpDwnFanOnTemp";
            this.UpDwnFanOnTemp.Size = new System.Drawing.Size(76, 20);
            this.UpDwnFanOnTemp.TabIndex = 2;
            this.UpDwnFanOnTemp.ValueChanged += new System.EventHandler(this.FanOnValueChanged);
            // 
            // UpDwnEnableBlrTemp
            // 
            this.UpDwnEnableBlrTemp.Location = new System.Drawing.Point(149, 80);
            this.UpDwnEnableBlrTemp.Name = "UpDwnEnableBlrTemp";
            this.UpDwnEnableBlrTemp.Size = new System.Drawing.Size(76, 20);
            this.UpDwnEnableBlrTemp.TabIndex = 3;
            this.UpDwnEnableBlrTemp.ValueChanged += new System.EventHandler(this.UpDwnEnableBlrTemp_ValueChanged);
            // 
            // UpDwnFanOffTemp
            // 
            this.UpDwnFanOffTemp.Location = new System.Drawing.Point(149, 54);
            this.UpDwnFanOffTemp.Name = "UpDwnFanOffTemp";
            this.UpDwnFanOffTemp.Size = new System.Drawing.Size(76, 20);
            this.UpDwnFanOffTemp.TabIndex = 4;
            this.UpDwnFanOffTemp.ValueChanged += new System.EventHandler(this.FanOffValueChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(28, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(72, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Fan On Temp";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(26, 57);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(72, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Fan Off Temp";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(25, 83);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(120, 13);
            this.label3.TabIndex = 7;
            this.label3.Text = "Engine Temp for Blower";
            // 
            // UpDwnBlr3On
            // 
            this.UpDwnBlr3On.Location = new System.Drawing.Point(149, 106);
            this.UpDwnBlr3On.Name = "UpDwnBlr3On";
            this.UpDwnBlr3On.Size = new System.Drawing.Size(76, 20);
            this.UpDwnBlr3On.TabIndex = 8;
            this.UpDwnBlr3On.ValueChanged += new System.EventHandler(this.UpDwnBlr3On_ValueChanged);
            // 
            // UpDwnBlr2On
            // 
            this.UpDwnBlr2On.Location = new System.Drawing.Point(149, 132);
            this.UpDwnBlr2On.Name = "UpDwnBlr2On";
            this.UpDwnBlr2On.Size = new System.Drawing.Size(76, 20);
            this.UpDwnBlr2On.TabIndex = 9;
            this.UpDwnBlr2On.ValueChanged += new System.EventHandler(this.UpDwnBlr2On_ValueChanged);
            // 
            // UpDwnBlr1On
            // 
            this.UpDwnBlr1On.Location = new System.Drawing.Point(149, 158);
            this.UpDwnBlr1On.Name = "UpDwnBlr1On";
            this.UpDwnBlr1On.Size = new System.Drawing.Size(76, 20);
            this.UpDwnBlr1On.TabIndex = 10;
            this.UpDwnBlr1On.ValueChanged += new System.EventHandler(this.UpDwnBlr1On_ValueChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(25, 110);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(95, 13);
            this.label4.TabIndex = 11;
            this.label4.Text = "Blower 3 On Temp";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(26, 135);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(95, 13);
            this.label5.TabIndex = 12;
            this.label5.Text = "Blower 2 On Temp";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(25, 161);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(95, 13);
            this.label6.TabIndex = 13;
            this.label6.Text = "Blower 1 On Temp";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.UpDwnFanOnTemp);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.UpDwnEnableBlrTemp);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.UpDwnFanOffTemp);
            this.groupBox1.Controls.Add(this.UpDwnBlr1On);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.UpDwnBlr2On);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.UpDwnBlr3On);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(252, 199);
            this.groupBox1.TabIndex = 14;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Temperatures";
            // 
            // UpDwnHighRev
            // 
            this.UpDwnHighRev.Location = new System.Drawing.Point(457, 40);
            this.UpDwnHighRev.Name = "UpDwnHighRev";
            this.UpDwnHighRev.Size = new System.Drawing.Size(76, 20);
            this.UpDwnHighRev.TabIndex = 15;
            this.UpDwnHighRev.ValueChanged += new System.EventHandler(this.UpDwnHighRev_ValueChanged);
            // 
            // UpDwnLowRev
            // 
            this.UpDwnLowRev.Location = new System.Drawing.Point(457, 69);
            this.UpDwnLowRev.Name = "UpDwnLowRev";
            this.UpDwnLowRev.Size = new System.Drawing.Size(76, 20);
            this.UpDwnLowRev.TabIndex = 16;
            this.UpDwnLowRev.ValueChanged += new System.EventHandler(this.UpDwnLowRev_ValueChanged);
            // 
            // UpDwnFPGAXmitRate
            // 
            this.UpDwnFPGAXmitRate.Location = new System.Drawing.Point(457, 124);
            this.UpDwnFPGAXmitRate.Name = "UpDwnFPGAXmitRate";
            this.UpDwnFPGAXmitRate.Size = new System.Drawing.Size(76, 20);
            this.UpDwnFPGAXmitRate.TabIndex = 18;
            this.UpDwnFPGAXmitRate.ValueChanged += new System.EventHandler(this.UpDwnFPGAXmitRate_ValueChanged);
            // 
            // UpDwnLightOn
            // 
            this.UpDwnLightOn.Location = new System.Drawing.Point(457, 95);
            this.UpDwnLightOn.Name = "UpDwnLightOn";
            this.UpDwnLightOn.Size = new System.Drawing.Size(76, 20);
            this.UpDwnLightOn.TabIndex = 17;
            this.UpDwnLightOn.ValueChanged += new System.EventHandler(this.UpDwnLightOn_ValueChanged);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(357, 44);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(0, 13);
            this.label7.TabIndex = 19;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(379, 76);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(0, 13);
            this.label8.TabIndex = 20;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(345, 44);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(76, 13);
            this.label9.TabIndex = 21;
            this.label9.Text = "High Rev Limit";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(345, 73);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(74, 13);
            this.label10.TabIndex = 22;
            this.label10.Text = "Low Rev Limit";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(345, 98);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(82, 13);
            this.label11.TabIndex = 23;
            this.label11.Text = "Lights On Delay";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(345, 128);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(104, 13);
            this.label12.TabIndex = 24;
            this.label12.Text = "FPGA Transmit Rate";
            // 
            // DlgSetParams
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(585, 435);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.UpDwnFPGAXmitRate);
            this.Controls.Add(this.UpDwnLightOn);
            this.Controls.Add(this.UpDwnLowRev);
            this.Controls.Add(this.UpDwnHighRev);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Name = "DlgSetParams";
            this.Text = "DlgSetParams";
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnFanOnTemp)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnEnableBlrTemp)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnFanOffTemp)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnBlr3On)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnBlr2On)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnBlr1On)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnHighRev)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnLowRev)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnFPGAXmitRate)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.UpDwnLightOn)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.NumericUpDown UpDwnFanOnTemp;
        private System.Windows.Forms.NumericUpDown UpDwnEnableBlrTemp;
        private System.Windows.Forms.NumericUpDown UpDwnFanOffTemp;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown UpDwnBlr3On;
        private System.Windows.Forms.NumericUpDown UpDwnBlr2On;
        private System.Windows.Forms.NumericUpDown UpDwnBlr1On;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.NumericUpDown UpDwnHighRev;
        private System.Windows.Forms.NumericUpDown UpDwnLowRev;
        private System.Windows.Forms.NumericUpDown UpDwnFPGAXmitRate;
        private System.Windows.Forms.NumericUpDown UpDwnLightOn;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
    }
}