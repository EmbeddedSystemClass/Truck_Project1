namespace EpServerEngineSampleServer
{
	partial class ParamsForm
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
			this.label15 = new System.Windows.Forms.Label();
			this.cbTestBank = new System.Windows.Forms.ComboBox();
			this.cbMPHUpdateRate = new System.Windows.Forms.ComboBox();
			this.cbRPMUpdateRate = new System.Windows.Forms.ComboBox();
			this.cbFPGAXmitRate = new System.Windows.Forms.ComboBox();
			this.cbLightsOnDelay = new System.Windows.Forms.ComboBox();
			this.cbLowRevLimit = new System.Windows.Forms.ComboBox();
			this.cbHighRevLimit = new System.Windows.Forms.ComboBox();
			this.label14 = new System.Windows.Forms.Label();
			this.label13 = new System.Windows.Forms.Label();
			this.label12 = new System.Windows.Forms.Label();
			this.label11 = new System.Windows.Forms.Label();
			this.label10 = new System.Windows.Forms.Label();
			this.label9 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.cbTempLimit = new System.Windows.Forms.ComboBox();
			this.cbBlower1 = new System.Windows.Forms.ComboBox();
			this.label17 = new System.Windows.Forms.Label();
			this.cbBlower2 = new System.Windows.Forms.ComboBox();
			this.cbBlower3 = new System.Windows.Forms.ComboBox();
			this.cbBlowerEnabled = new System.Windows.Forms.ComboBox();
			this.cbFanOff = new System.Windows.Forms.ComboBox();
			this.cbFanOn = new System.Windows.Forms.ComboBox();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.label16 = new System.Windows.Forms.Label();
			this.label18 = new System.Windows.Forms.Label();
			this.label19 = new System.Windows.Forms.Label();
			this.label20 = new System.Windows.Forms.Label();
			this.btnOK = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.tbDebug = new System.Windows.Forms.TextBox();
			this.btnTest = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// label15
			// 
			this.label15.AutoSize = true;
			this.label15.Location = new System.Drawing.Point(345, 212);
			this.label15.Name = "label15";
			this.label15.Size = new System.Drawing.Size(56, 13);
			this.label15.TabIndex = 72;
			this.label15.Text = "Test Bank";
			// 
			// cbTestBank
			// 
			this.cbTestBank.FormattingEnabled = true;
			this.cbTestBank.Items.AddRange(new object[] {
            "Bank 0",
            "Bank 1",
            "Bank 2",
            "Bank 3",
            "Bank 4"});
			this.cbTestBank.Location = new System.Drawing.Point(457, 209);
			this.cbTestBank.Name = "cbTestBank";
			this.cbTestBank.Size = new System.Drawing.Size(94, 21);
			this.cbTestBank.TabIndex = 71;
			this.cbTestBank.SelectedIndexChanged += new System.EventHandler(this.cbTestBank_SelectedIndexChanged);
			// 
			// cbMPHUpdateRate
			// 
			this.cbMPHUpdateRate.FormattingEnabled = true;
			this.cbMPHUpdateRate.Items.AddRange(new object[] {
            "1000",
            "2000",
            "3000"});
			this.cbMPHUpdateRate.Location = new System.Drawing.Point(457, 180);
			this.cbMPHUpdateRate.Name = "cbMPHUpdateRate";
			this.cbMPHUpdateRate.Size = new System.Drawing.Size(94, 21);
			this.cbMPHUpdateRate.TabIndex = 70;
			this.cbMPHUpdateRate.SelectedIndexChanged += new System.EventHandler(this.cbMPHUpdateRate_SelectedIndexChanged);
			// 
			// cbRPMUpdateRate
			// 
			this.cbRPMUpdateRate.FormattingEnabled = true;
			this.cbRPMUpdateRate.Items.AddRange(new object[] {
            "1000",
            "2000",
            "3000"});
			this.cbRPMUpdateRate.Location = new System.Drawing.Point(457, 151);
			this.cbRPMUpdateRate.Name = "cbRPMUpdateRate";
			this.cbRPMUpdateRate.Size = new System.Drawing.Size(94, 21);
			this.cbRPMUpdateRate.TabIndex = 69;
			this.cbRPMUpdateRate.SelectedIndexChanged += new System.EventHandler(this.cbMPHUpdateRate_SelectedIndexChanged);
			// 
			// cbFPGAXmitRate
			// 
			this.cbFPGAXmitRate.FormattingEnabled = true;
			this.cbFPGAXmitRate.Items.AddRange(new object[] {
            "1000",
            "2000",
            "3000"});
			this.cbFPGAXmitRate.Location = new System.Drawing.Point(457, 124);
			this.cbFPGAXmitRate.Name = "cbFPGAXmitRate";
			this.cbFPGAXmitRate.Size = new System.Drawing.Size(94, 21);
			this.cbFPGAXmitRate.TabIndex = 68;
			this.cbFPGAXmitRate.SelectedIndexChanged += new System.EventHandler(this.cbFPGAXmitRate_SelectedIndexChanged);
			// 
			// cbLightsOnDelay
			// 
			this.cbLightsOnDelay.FormattingEnabled = true;
			this.cbLightsOnDelay.Items.AddRange(new object[] {
            "1 second",
            "2 seconds",
            "3 seconds",
            "5 seconds",
            "10 seconds",
            "15 seconds",
            "30 seconds",
            "1 minute",
            "2 minutes",
            "5 minutes",
            "10 minutes",
            "30 minutes",
            "1 hour"});
			this.cbLightsOnDelay.Location = new System.Drawing.Point(457, 95);
			this.cbLightsOnDelay.Name = "cbLightsOnDelay";
			this.cbLightsOnDelay.Size = new System.Drawing.Size(94, 21);
			this.cbLightsOnDelay.TabIndex = 67;
			this.cbLightsOnDelay.SelectedIndexChanged += new System.EventHandler(this.cbLightsOnDelay_SelectedIndexChanged);
			// 
			// cbLowRevLimit
			// 
			this.cbLowRevLimit.FormattingEnabled = true;
			this.cbLowRevLimit.Items.AddRange(new object[] {
            "4500",
            "4400",
            "4300",
            "4200",
            "4100",
            "4000",
            "3900",
            "3800",
            "3700"});
			this.cbLowRevLimit.Location = new System.Drawing.Point(457, 69);
			this.cbLowRevLimit.Name = "cbLowRevLimit";
			this.cbLowRevLimit.Size = new System.Drawing.Size(94, 21);
			this.cbLowRevLimit.TabIndex = 66;
			this.cbLowRevLimit.SelectedIndexChanged += new System.EventHandler(this.cbLowRevLimit_SelectedIndexChanged);
			// 
			// cbHighRevLimit
			// 
			this.cbHighRevLimit.FormattingEnabled = true;
			this.cbHighRevLimit.Items.AddRange(new object[] {
            "6000",
            "5800",
            "5600",
            "5400",
            "5200",
            "5000",
            "4800",
            "4600",
            "4200",
            "4000"});
			this.cbHighRevLimit.Location = new System.Drawing.Point(457, 39);
			this.cbHighRevLimit.Name = "cbHighRevLimit";
			this.cbHighRevLimit.Size = new System.Drawing.Size(94, 21);
			this.cbHighRevLimit.TabIndex = 65;
			this.cbHighRevLimit.SelectedIndexChanged += new System.EventHandler(this.cbHighRevLimit_SelectedIndexChanged);
			// 
			// label14
			// 
			this.label14.AutoSize = true;
			this.label14.Location = new System.Drawing.Point(345, 182);
			this.label14.Name = "label14";
			this.label14.Size = new System.Drawing.Size(100, 13);
			this.label14.TabIndex = 64;
			this.label14.Text = "MPH Transmit Rate";
			// 
			// label13
			// 
			this.label13.AutoSize = true;
			this.label13.Location = new System.Drawing.Point(345, 155);
			this.label13.Name = "label13";
			this.label13.Size = new System.Drawing.Size(95, 13);
			this.label13.TabIndex = 63;
			this.label13.Text = "RPM Update Rate";
			// 
			// label12
			// 
			this.label12.AutoSize = true;
			this.label12.Location = new System.Drawing.Point(345, 129);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(104, 13);
			this.label12.TabIndex = 62;
			this.label12.Text = "FPGA Transmit Rate";
			// 
			// label11
			// 
			this.label11.AutoSize = true;
			this.label11.Location = new System.Drawing.Point(345, 98);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(82, 13);
			this.label11.TabIndex = 61;
			this.label11.Text = "Lights On Delay";
			// 
			// label10
			// 
			this.label10.AutoSize = true;
			this.label10.Location = new System.Drawing.Point(345, 73);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(74, 13);
			this.label10.TabIndex = 60;
			this.label10.Text = "Low Rev Limit";
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Location = new System.Drawing.Point(345, 44);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(76, 13);
			this.label9.TabIndex = 59;
			this.label9.Text = "High Rev Limit";
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(379, 76);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(0, 13);
			this.label8.TabIndex = 58;
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(357, 44);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(0, 13);
			this.label7.TabIndex = 57;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.cbTempLimit);
			this.groupBox1.Controls.Add(this.cbBlower1);
			this.groupBox1.Controls.Add(this.label17);
			this.groupBox1.Controls.Add(this.cbBlower2);
			this.groupBox1.Controls.Add(this.cbBlower3);
			this.groupBox1.Controls.Add(this.cbBlowerEnabled);
			this.groupBox1.Controls.Add(this.cbFanOff);
			this.groupBox1.Controls.Add(this.cbFanOn);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.label16);
			this.groupBox1.Controls.Add(this.label18);
			this.groupBox1.Controls.Add(this.label19);
			this.groupBox1.Controls.Add(this.label20);
			this.groupBox1.Location = new System.Drawing.Point(12, 12);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(252, 229);
			this.groupBox1.TabIndex = 56;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Temperatures";
			// 
			// cbTempLimit
			// 
			this.cbTempLimit.FormattingEnabled = true;
			this.cbTempLimit.Location = new System.Drawing.Point(159, 188);
			this.cbTempLimit.Name = "cbTempLimit";
			this.cbTempLimit.Size = new System.Drawing.Size(76, 21);
			this.cbTempLimit.TabIndex = 42;
			this.cbTempLimit.SelectedIndexChanged += new System.EventHandler(this.cbTempLimit_SelectedIndexChanged);
			// 
			// cbBlower1
			// 
			this.cbBlower1.FormattingEnabled = true;
			this.cbBlower1.Location = new System.Drawing.Point(159, 161);
			this.cbBlower1.Name = "cbBlower1";
			this.cbBlower1.Size = new System.Drawing.Size(76, 21);
			this.cbBlower1.TabIndex = 31;
			this.cbBlower1.SelectedIndexChanged += new System.EventHandler(this.cbBlower1_SelectedIndexChanged);
			// 
			// label17
			// 
			this.label17.AutoSize = true;
			this.label17.Location = new System.Drawing.Point(25, 191);
			this.label17.Name = "label17";
			this.label17.Size = new System.Drawing.Size(94, 13);
			this.label17.TabIndex = 41;
			this.label17.Text = "Engine Temp Limit";
			// 
			// cbBlower2
			// 
			this.cbBlower2.FormattingEnabled = true;
			this.cbBlower2.Location = new System.Drawing.Point(159, 134);
			this.cbBlower2.Name = "cbBlower2";
			this.cbBlower2.Size = new System.Drawing.Size(76, 21);
			this.cbBlower2.TabIndex = 31;
			this.cbBlower2.SelectedIndexChanged += new System.EventHandler(this.cbBlower2_SelectedIndexChanged);
			// 
			// cbBlower3
			// 
			this.cbBlower3.FormattingEnabled = true;
			this.cbBlower3.Location = new System.Drawing.Point(159, 108);
			this.cbBlower3.Name = "cbBlower3";
			this.cbBlower3.Size = new System.Drawing.Size(76, 21);
			this.cbBlower3.TabIndex = 31;
			this.cbBlower3.SelectedIndexChanged += new System.EventHandler(this.cbBlower3_SelectedIndexChanged);
			// 
			// cbBlowerEnabled
			// 
			this.cbBlowerEnabled.FormattingEnabled = true;
			this.cbBlowerEnabled.Location = new System.Drawing.Point(159, 80);
			this.cbBlowerEnabled.Name = "cbBlowerEnabled";
			this.cbBlowerEnabled.Size = new System.Drawing.Size(76, 21);
			this.cbBlowerEnabled.TabIndex = 31;
			this.cbBlowerEnabled.SelectedIndexChanged += new System.EventHandler(this.cbBlowerEnabled_SelectedIndexChanged);
			// 
			// cbFanOff
			// 
			this.cbFanOff.FormattingEnabled = true;
			this.cbFanOff.Location = new System.Drawing.Point(159, 53);
			this.cbFanOff.Name = "cbFanOff";
			this.cbFanOff.Size = new System.Drawing.Size(76, 21);
			this.cbFanOff.TabIndex = 31;
			this.cbFanOff.SelectedIndexChanged += new System.EventHandler(this.cbFanOff_SelectedIndexChanged);
			// 
			// cbFanOn
			// 
			this.cbFanOn.FormattingEnabled = true;
			this.cbFanOn.Location = new System.Drawing.Point(159, 26);
			this.cbFanOn.Name = "cbFanOn";
			this.cbFanOn.Size = new System.Drawing.Size(76, 21);
			this.cbFanOn.TabIndex = 30;
			this.cbFanOn.SelectedIndexChanged += new System.EventHandler(this.cbFanOn_SelectedIndexChanged);
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(25, 84);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(120, 13);
			this.label5.TabIndex = 7;
			this.label5.Text = "Engine Temp for Blower";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(25, 164);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(95, 13);
			this.label6.TabIndex = 13;
			this.label6.Text = "Blower 1 On Temp";
			// 
			// label16
			// 
			this.label16.AutoSize = true;
			this.label16.Location = new System.Drawing.Point(26, 138);
			this.label16.Name = "label16";
			this.label16.Size = new System.Drawing.Size(95, 13);
			this.label16.TabIndex = 12;
			this.label16.Text = "Blower 2 On Temp";
			// 
			// label18
			// 
			this.label18.AutoSize = true;
			this.label18.Location = new System.Drawing.Point(25, 111);
			this.label18.Name = "label18";
			this.label18.Size = new System.Drawing.Size(95, 13);
			this.label18.TabIndex = 11;
			this.label18.Text = "Blower 3 On Temp";
			// 
			// label19
			// 
			this.label19.AutoSize = true;
			this.label19.Location = new System.Drawing.Point(28, 30);
			this.label19.Name = "label19";
			this.label19.Size = new System.Drawing.Size(72, 13);
			this.label19.TabIndex = 5;
			this.label19.Text = "Fan On Temp";
			// 
			// label20
			// 
			this.label20.AutoSize = true;
			this.label20.Location = new System.Drawing.Point(26, 57);
			this.label20.Name = "label20";
			this.label20.Size = new System.Drawing.Size(72, 13);
			this.label20.TabIndex = 6;
			this.label20.Text = "Fan Off Temp";
			// 
			// btnOK
			// 
			this.btnOK.Location = new System.Drawing.Point(380, 248);
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size(75, 23);
			this.btnOK.TabIndex = 73;
			this.btnOK.Text = "OK";
			this.btnOK.UseVisualStyleBackColor = true;
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(474, 248);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(75, 23);
			this.btnCancel.TabIndex = 74;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.UseVisualStyleBackColor = true;
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// tbDebug
			// 
			this.tbDebug.Location = new System.Drawing.Point(2, 250);
			this.tbDebug.Multiline = true;
			this.tbDebug.Name = "tbDebug";
			this.tbDebug.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbDebug.Size = new System.Drawing.Size(355, 294);
			this.tbDebug.TabIndex = 75;
			// 
			// btnTest
			// 
			this.btnTest.Location = new System.Drawing.Point(405, 397);
			this.btnTest.Name = "btnTest";
			this.btnTest.Size = new System.Drawing.Size(75, 23);
			this.btnTest.TabIndex = 76;
			this.btnTest.Text = "button1";
			this.btnTest.UseVisualStyleBackColor = true;
			this.btnTest.Click += new System.EventHandler(this.btnTest_Click);
			// 
			// ParamsForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(580, 567);
			this.Controls.Add(this.btnTest);
			this.Controls.Add(this.tbDebug);
			this.Controls.Add(this.btnCancel);
			this.Controls.Add(this.btnOK);
			this.Controls.Add(this.label15);
			this.Controls.Add(this.cbTestBank);
			this.Controls.Add(this.cbMPHUpdateRate);
			this.Controls.Add(this.cbRPMUpdateRate);
			this.Controls.Add(this.cbFPGAXmitRate);
			this.Controls.Add(this.cbLightsOnDelay);
			this.Controls.Add(this.cbLowRevLimit);
			this.Controls.Add(this.cbHighRevLimit);
			this.Controls.Add(this.label14);
			this.Controls.Add(this.label13);
			this.Controls.Add(this.label12);
			this.Controls.Add(this.label11);
			this.Controls.Add(this.label10);
			this.Controls.Add(this.label9);
			this.Controls.Add(this.label8);
			this.Controls.Add(this.label7);
			this.Controls.Add(this.groupBox1);
			this.Name = "ParamsForm";
			this.Text = "ParamsForm";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label label15;
		private System.Windows.Forms.ComboBox cbTestBank;
		private System.Windows.Forms.ComboBox cbMPHUpdateRate;
		private System.Windows.Forms.ComboBox cbRPMUpdateRate;
		private System.Windows.Forms.ComboBox cbFPGAXmitRate;
		private System.Windows.Forms.ComboBox cbLightsOnDelay;
		private System.Windows.Forms.ComboBox cbLowRevLimit;
		private System.Windows.Forms.ComboBox cbHighRevLimit;
		private System.Windows.Forms.Label label14;
		private System.Windows.Forms.Label label13;
		private System.Windows.Forms.Label label12;
		private System.Windows.Forms.Label label11;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.ComboBox cbTempLimit;
		private System.Windows.Forms.ComboBox cbBlower1;
		private System.Windows.Forms.Label label17;
		private System.Windows.Forms.ComboBox cbBlower2;
		private System.Windows.Forms.ComboBox cbBlower3;
		private System.Windows.Forms.ComboBox cbBlowerEnabled;
		private System.Windows.Forms.ComboBox cbFanOff;
		private System.Windows.Forms.ComboBox cbFanOn;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label16;
		private System.Windows.Forms.Label label18;
		private System.Windows.Forms.Label label19;
		private System.Windows.Forms.Label label20;
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.TextBox tbDebug;
		private System.Windows.Forms.Button btnTest;
	}
}