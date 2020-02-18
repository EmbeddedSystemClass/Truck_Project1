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
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.cbTempLimit = new System.Windows.Forms.ComboBox();
			this.cbBlower1 = new System.Windows.Forms.ComboBox();
			this.label17 = new System.Windows.Forms.Label();
			this.cbBlower2 = new System.Windows.Forms.ComboBox();
			this.cbBlower3 = new System.Windows.Forms.ComboBox();
			this.cbBlowerEnabled = new System.Windows.Forms.ComboBox();
			this.cbFanOff = new System.Windows.Forms.ComboBox();
			this.cbFanOn = new System.Windows.Forms.ComboBox();
			this.label7 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.label9 = new System.Windows.Forms.Label();
			this.label10 = new System.Windows.Forms.Label();
			this.label11 = new System.Windows.Forms.Label();
			this.label12 = new System.Windows.Forms.Label();
			this.label13 = new System.Windows.Forms.Label();
			this.label14 = new System.Windows.Forms.Label();
			this.cbHighRevLimit = new System.Windows.Forms.ComboBox();
			this.cbLowRevLimit = new System.Windows.Forms.ComboBox();
			this.cbLightsOnDelay = new System.Windows.Forms.ComboBox();
			this.cbFPGAXmitRate = new System.Windows.Forms.ComboBox();
			this.cbRPMUpdateRate = new System.Windows.Forms.ComboBox();
			this.cbMPHUpdateRate = new System.Windows.Forms.ComboBox();
			this.cbTestBank = new System.Windows.Forms.ComboBox();
			this.label15 = new System.Windows.Forms.Label();
			this.label16 = new System.Windows.Forms.Label();
			this.cbPasswordTimeout = new System.Windows.Forms.ComboBox();
			this.label18 = new System.Windows.Forms.Label();
			this.cbPasswordRetries = new System.Windows.Forms.ComboBox();
			this.btnChangePassword = new System.Windows.Forms.Button();
			this.tbNewPasssword = new System.Windows.Forms.TextBox();
			this.tbTest = new System.Windows.Forms.TextBox();
			this.label19 = new System.Windows.Forms.Label();
			this.cbBaudRate3 = new System.Windows.Forms.ComboBox();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// btnOK
			// 
			this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.btnOK.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnOK.Location = new System.Drawing.Point(606, 494);
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size(99, 44);
			this.btnOK.TabIndex = 0;
			this.btnOK.Text = "OK";
			this.btnOK.UseVisualStyleBackColor = true;
			this.btnOK.Click += new System.EventHandler(this.OKClicked);
			// 
			// btnCancel
			// 
			this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnCancel.Location = new System.Drawing.Point(732, 494);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(99, 44);
			this.btnCancel.TabIndex = 1;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.UseVisualStyleBackColor = true;
			this.btnCancel.Click += new System.EventHandler(this.CancelClick);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(24, 47);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(154, 25);
			this.label1.TabIndex = 5;
			this.label1.Text = "Fan On Temp";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label2.Location = new System.Drawing.Point(24, 90);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(155, 25);
			this.label2.TabIndex = 6;
			this.label2.Text = "Fan Off Temp";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label3.Location = new System.Drawing.Point(23, 133);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(263, 25);
			this.label3.TabIndex = 7;
			this.label3.Text = "Engine Temp for Blower";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label4.Location = new System.Drawing.Point(23, 176);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(205, 25);
			this.label4.TabIndex = 11;
			this.label4.Text = "Blower 3 On Temp";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label5.Location = new System.Drawing.Point(24, 219);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(205, 25);
			this.label5.TabIndex = 12;
			this.label5.Text = "Blower 2 On Temp";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label6.Location = new System.Drawing.Point(23, 261);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(205, 25);
			this.label6.TabIndex = 13;
			this.label6.Text = "Blower 1 On Temp";
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
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Location = new System.Drawing.Point(12, 12);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(407, 370);
			this.groupBox1.TabIndex = 14;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Temperatures";
			// 
			// cbTempLimit
			// 
			this.cbTempLimit.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbTempLimit.FormattingEnabled = true;
			this.cbTempLimit.Location = new System.Drawing.Point(298, 302);
			this.cbTempLimit.Name = "cbTempLimit";
			this.cbTempLimit.Size = new System.Drawing.Size(89, 33);
			this.cbTempLimit.TabIndex = 42;
			this.cbTempLimit.SelectedIndexChanged += new System.EventHandler(this.cbTempLimit_selected_index_changed);
			// 
			// cbBlower1
			// 
			this.cbBlower1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbBlower1.FormattingEnabled = true;
			this.cbBlower1.Location = new System.Drawing.Point(298, 259);
			this.cbBlower1.Name = "cbBlower1";
			this.cbBlower1.Size = new System.Drawing.Size(89, 33);
			this.cbBlower1.TabIndex = 31;
			this.cbBlower1.SelectedIndexChanged += new System.EventHandler(this.cbBlower1_SelectedIndexChanged);
			// 
			// label17
			// 
			this.label17.AutoSize = true;
			this.label17.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label17.Location = new System.Drawing.Point(23, 304);
			this.label17.Name = "label17";
			this.label17.Size = new System.Drawing.Size(207, 25);
			this.label17.TabIndex = 41;
			this.label17.Text = "Engine Temp Limit";
			// 
			// cbBlower2
			// 
			this.cbBlower2.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbBlower2.FormattingEnabled = true;
			this.cbBlower2.Location = new System.Drawing.Point(298, 216);
			this.cbBlower2.Name = "cbBlower2";
			this.cbBlower2.Size = new System.Drawing.Size(89, 33);
			this.cbBlower2.TabIndex = 31;
			this.cbBlower2.SelectedIndexChanged += new System.EventHandler(this.cbBlower2_SelectedIndexChanged);
			// 
			// cbBlower3
			// 
			this.cbBlower3.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbBlower3.FormattingEnabled = true;
			this.cbBlower3.Location = new System.Drawing.Point(298, 173);
			this.cbBlower3.Name = "cbBlower3";
			this.cbBlower3.Size = new System.Drawing.Size(89, 33);
			this.cbBlower3.TabIndex = 31;
			this.cbBlower3.SelectedIndexChanged += new System.EventHandler(this.cbBlower3_SelectedIndexChanged);
			// 
			// cbBlowerEnabled
			// 
			this.cbBlowerEnabled.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbBlowerEnabled.FormattingEnabled = true;
			this.cbBlowerEnabled.Location = new System.Drawing.Point(298, 130);
			this.cbBlowerEnabled.Name = "cbBlowerEnabled";
			this.cbBlowerEnabled.Size = new System.Drawing.Size(89, 33);
			this.cbBlowerEnabled.TabIndex = 31;
			this.cbBlowerEnabled.SelectedIndexChanged += new System.EventHandler(this.cbBlowerEnabled_SelectedIndexChanged);
			// 
			// cbFanOff
			// 
			this.cbFanOff.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbFanOff.FormattingEnabled = true;
			this.cbFanOff.Location = new System.Drawing.Point(298, 87);
			this.cbFanOff.Name = "cbFanOff";
			this.cbFanOff.Size = new System.Drawing.Size(89, 33);
			this.cbFanOff.TabIndex = 31;
			this.cbFanOff.SelectedIndexChanged += new System.EventHandler(this.cbFanOff_SelectedIndexChanged);
			// 
			// cbFanOn
			// 
			this.cbFanOn.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbFanOn.FormattingEnabled = true;
			this.cbFanOn.Location = new System.Drawing.Point(298, 44);
			this.cbFanOn.Name = "cbFanOn";
			this.cbFanOn.Size = new System.Drawing.Size(89, 33);
			this.cbFanOn.TabIndex = 30;
			this.cbFanOn.SelectedIndexChanged += new System.EventHandler(this.cbFanOn_SelectedIndexChanged);
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label7.Location = new System.Drawing.Point(487, 57);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(0, 25);
			this.label7.TabIndex = 19;
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label8.Location = new System.Drawing.Point(509, 131);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(0, 25);
			this.label8.TabIndex = 20;
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label9.Location = new System.Drawing.Point(452, 43);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(165, 25);
			this.label9.TabIndex = 21;
			this.label9.Text = "High Rev Limit";
			// 
			// label10
			// 
			this.label10.AutoSize = true;
			this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label10.Location = new System.Drawing.Point(452, 87);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(159, 25);
			this.label10.TabIndex = 22;
			this.label10.Text = "Low Rev Limit";
			// 
			// label11
			// 
			this.label11.AutoSize = true;
			this.label11.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label11.Location = new System.Drawing.Point(452, 136);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(180, 25);
			this.label11.TabIndex = 23;
			this.label11.Text = "Lights On Delay";
			// 
			// label12
			// 
			this.label12.AutoSize = true;
			this.label12.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label12.Location = new System.Drawing.Point(452, 179);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(227, 25);
			this.label12.TabIndex = 24;
			this.label12.Text = "FPGA Transmit Rate";
			// 
			// label13
			// 
			this.label13.AutoSize = true;
			this.label13.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label13.Location = new System.Drawing.Point(452, 223);
			this.label13.Name = "label13";
			this.label13.Size = new System.Drawing.Size(200, 25);
			this.label13.TabIndex = 27;
			this.label13.Text = "RPM Update Rate";
			// 
			// label14
			// 
			this.label14.AutoSize = true;
			this.label14.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label14.Location = new System.Drawing.Point(452, 268);
			this.label14.Name = "label14";
			this.label14.Size = new System.Drawing.Size(216, 25);
			this.label14.TabIndex = 28;
			this.label14.Text = "MPH Transmit Rate";
			// 
			// cbHighRevLimit
			// 
			this.cbHighRevLimit.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
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
			this.cbHighRevLimit.Location = new System.Drawing.Point(690, 40);
			this.cbHighRevLimit.Name = "cbHighRevLimit";
			this.cbHighRevLimit.Size = new System.Drawing.Size(141, 33);
			this.cbHighRevLimit.TabIndex = 31;
			this.cbHighRevLimit.SelectedIndexChanged += new System.EventHandler(this.cbHighRevLimit_SelectedIndexChanged);
			// 
			// cbLowRevLimit
			// 
			this.cbLowRevLimit.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
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
			this.cbLowRevLimit.Location = new System.Drawing.Point(690, 85);
			this.cbLowRevLimit.Name = "cbLowRevLimit";
			this.cbLowRevLimit.Size = new System.Drawing.Size(141, 33);
			this.cbLowRevLimit.TabIndex = 32;
			this.cbLowRevLimit.SelectedIndexChanged += new System.EventHandler(this.cbLowRevLimit_SelectedIndexChanged);
			// 
			// cbLightsOnDelay
			// 
			this.cbLightsOnDelay.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
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
			this.cbLightsOnDelay.Location = new System.Drawing.Point(690, 130);
			this.cbLightsOnDelay.Name = "cbLightsOnDelay";
			this.cbLightsOnDelay.Size = new System.Drawing.Size(141, 33);
			this.cbLightsOnDelay.TabIndex = 33;
			this.cbLightsOnDelay.SelectedIndexChanged += new System.EventHandler(this.cbLightsOnDelay_SelectedIndexChanged);
			// 
			// cbFPGAXmitRate
			// 
			this.cbFPGAXmitRate.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbFPGAXmitRate.FormattingEnabled = true;
			this.cbFPGAXmitRate.Items.AddRange(new object[] {
            "1000",
            "2000",
            "3000"});
			this.cbFPGAXmitRate.Location = new System.Drawing.Point(690, 175);
			this.cbFPGAXmitRate.Name = "cbFPGAXmitRate";
			this.cbFPGAXmitRate.Size = new System.Drawing.Size(141, 33);
			this.cbFPGAXmitRate.TabIndex = 34;
			this.cbFPGAXmitRate.SelectedIndexChanged += new System.EventHandler(this.cbFPGAXmitRate_SelectedIndexChanged);
			// 
			// cbRPMUpdateRate
			// 
			this.cbRPMUpdateRate.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbRPMUpdateRate.FormattingEnabled = true;
			this.cbRPMUpdateRate.Items.AddRange(new object[] {
            "1000",
            "2000",
            "3000"});
			this.cbRPMUpdateRate.Location = new System.Drawing.Point(690, 220);
			this.cbRPMUpdateRate.Name = "cbRPMUpdateRate";
			this.cbRPMUpdateRate.Size = new System.Drawing.Size(141, 33);
			this.cbRPMUpdateRate.TabIndex = 35;
			this.cbRPMUpdateRate.SelectedIndexChanged += new System.EventHandler(this.cbRPMUpdateRate_SelectedIndexChanged);
			// 
			// cbMPHUpdateRate
			// 
			this.cbMPHUpdateRate.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbMPHUpdateRate.FormattingEnabled = true;
			this.cbMPHUpdateRate.Items.AddRange(new object[] {
            "1000",
            "2000",
            "3000"});
			this.cbMPHUpdateRate.Location = new System.Drawing.Point(690, 265);
			this.cbMPHUpdateRate.Name = "cbMPHUpdateRate";
			this.cbMPHUpdateRate.Size = new System.Drawing.Size(141, 33);
			this.cbMPHUpdateRate.TabIndex = 36;
			this.cbMPHUpdateRate.SelectedIndexChanged += new System.EventHandler(this.comboBox6_SelectedIndexChanged);
			// 
			// cbTestBank
			// 
			this.cbTestBank.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbTestBank.FormattingEnabled = true;
			this.cbTestBank.Items.AddRange(new object[] {
            "On",
            "Off"});
			this.cbTestBank.Location = new System.Drawing.Point(690, 310);
			this.cbTestBank.Name = "cbTestBank";
			this.cbTestBank.Size = new System.Drawing.Size(141, 33);
			this.cbTestBank.TabIndex = 37;
			this.cbTestBank.SelectedIndexChanged += new System.EventHandler(this.cbTestBank_SelectedIndexChanged);
			// 
			// label15
			// 
			this.label15.AutoSize = true;
			this.label15.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label15.Location = new System.Drawing.Point(452, 316);
			this.label15.Name = "label15";
			this.label15.Size = new System.Drawing.Size(143, 25);
			this.label15.TabIndex = 38;
			this.label15.Text = "Serial Port 2";
			// 
			// label16
			// 
			this.label16.AutoSize = true;
			this.label16.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label16.Location = new System.Drawing.Point(452, 357);
			this.label16.Name = "label16";
			this.label16.Size = new System.Drawing.Size(212, 25);
			this.label16.TabIndex = 40;
			this.label16.Text = "Password Timeout:";
			// 
			// cbPasswordTimeout
			// 
			this.cbPasswordTimeout.AutoCompleteCustomSource.AddRange(new string[] {
            "10 seconds",
            "15 seconds",
            "30 seconds",
            "1 minute",
            "2 minutes",
            "5 minutes",
            "10 minutes"});
			this.cbPasswordTimeout.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbPasswordTimeout.FormattingEnabled = true;
			this.cbPasswordTimeout.Items.AddRange(new object[] {
            "10 seconds",
            "15 seconds",
            "30 seconds",
            "1 minute",
            "2 minutes",
            "5 minutes",
            "10 minutes"});
			this.cbPasswordTimeout.Location = new System.Drawing.Point(690, 355);
			this.cbPasswordTimeout.Name = "cbPasswordTimeout";
			this.cbPasswordTimeout.Size = new System.Drawing.Size(141, 33);
			this.cbPasswordTimeout.TabIndex = 39;
			this.cbPasswordTimeout.SelectedIndexChanged += new System.EventHandler(this.cbPasswordTimeout_SelectedIndexChanged);
			// 
			// label18
			// 
			this.label18.AutoSize = true;
			this.label18.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label18.Location = new System.Drawing.Point(452, 403);
			this.label18.Name = "label18";
			this.label18.Size = new System.Drawing.Size(203, 25);
			this.label18.TabIndex = 42;
			this.label18.Text = "Password Retries:";
			// 
			// cbPasswordRetries
			// 
			this.cbPasswordRetries.AutoCompleteCustomSource.AddRange(new string[] {
            "2",
            "3",
            "4",
            "5",
            "6"});
			this.cbPasswordRetries.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbPasswordRetries.FormattingEnabled = true;
			this.cbPasswordRetries.Items.AddRange(new object[] {
            "2",
            "3",
            "4",
            "5",
            "6"});
			this.cbPasswordRetries.Location = new System.Drawing.Point(690, 400);
			this.cbPasswordRetries.Name = "cbPasswordRetries";
			this.cbPasswordRetries.Size = new System.Drawing.Size(141, 33);
			this.cbPasswordRetries.TabIndex = 41;
			this.cbPasswordRetries.SelectedIndexChanged += new System.EventHandler(this.cbPasswordRetries_SelectedIndexChanged);
			// 
			// btnChangePassword
			// 
			this.btnChangePassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnChangePassword.Location = new System.Drawing.Point(40, 434);
			this.btnChangePassword.Name = "btnChangePassword";
			this.btnChangePassword.Size = new System.Drawing.Size(223, 36);
			this.btnChangePassword.TabIndex = 43;
			this.btnChangePassword.Text = "Change Password";
			this.btnChangePassword.UseVisualStyleBackColor = true;
			this.btnChangePassword.Click += new System.EventHandler(this.btnChangePassword_Clicked);
			// 
			// tbNewPasssword
			// 
			this.tbNewPasssword.Location = new System.Drawing.Point(40, 494);
			this.tbNewPasssword.Name = "tbNewPasssword";
			this.tbNewPasssword.Size = new System.Drawing.Size(128, 20);
			this.tbNewPasssword.TabIndex = 44;
			this.tbNewPasssword.Visible = false;
			// 
			// tbTest
			// 
			this.tbTest.Location = new System.Drawing.Point(384, 496);
			this.tbTest.Name = "tbTest";
			this.tbTest.Size = new System.Drawing.Size(100, 20);
			this.tbTest.TabIndex = 45;
			// 
			// label19
			// 
			this.label19.AutoSize = true;
			this.label19.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label19.Location = new System.Drawing.Point(453, 448);
			this.label19.Name = "label19";
			this.label19.Size = new System.Drawing.Size(199, 25);
			this.label19.TabIndex = 47;
			this.label19.Text = "Comm3 Baudrate:";
			// 
			// cbBaudRate3
			// 
			this.cbBaudRate3.AutoCompleteCustomSource.AddRange(new string[] {
            "2",
            "3",
            "4",
            "5",
            "6"});
			this.cbBaudRate3.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbBaudRate3.FormattingEnabled = true;
			this.cbBaudRate3.Items.AddRange(new object[] {
            "4800",
            "9600",
            "19200",
            "38400"});
			this.cbBaudRate3.Location = new System.Drawing.Point(691, 445);
			this.cbBaudRate3.Name = "cbBaudRate3";
			this.cbBaudRate3.Size = new System.Drawing.Size(141, 33);
			this.cbBaudRate3.TabIndex = 46;
			this.cbBaudRate3.SelectedIndexChanged += new System.EventHandler(this.comboBox1_SelectedIndexChanged);
			// 
			// DlgSetParams
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
			this.ClientSize = new System.Drawing.Size(857, 556);
			this.Controls.Add(this.label19);
			this.Controls.Add(this.cbBaudRate3);
			this.Controls.Add(this.tbTest);
			this.Controls.Add(this.tbNewPasssword);
			this.Controls.Add(this.btnChangePassword);
			this.Controls.Add(this.label18);
			this.Controls.Add(this.cbPasswordRetries);
			this.Controls.Add(this.label16);
			this.Controls.Add(this.cbPasswordTimeout);
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
			this.Controls.Add(this.btnCancel);
			this.Controls.Add(this.btnOK);
			this.Name = "DlgSetParams";
			this.Text = "DlgSetParams";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.ComboBox cbFanOn;
        private System.Windows.Forms.ComboBox cbBlower1;
        private System.Windows.Forms.ComboBox cbBlower2;
        private System.Windows.Forms.ComboBox cbBlower3;
        private System.Windows.Forms.ComboBox cbBlowerEnabled;
        private System.Windows.Forms.ComboBox cbFanOff;
        private System.Windows.Forms.ComboBox cbHighRevLimit;
        private System.Windows.Forms.ComboBox cbLowRevLimit;
        private System.Windows.Forms.ComboBox cbLightsOnDelay;
        private System.Windows.Forms.ComboBox cbFPGAXmitRate;
        private System.Windows.Forms.ComboBox cbRPMUpdateRate;
        private System.Windows.Forms.ComboBox cbMPHUpdateRate;
        private System.Windows.Forms.ComboBox cbTestBank;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.ComboBox cbTempLimit;
        private System.Windows.Forms.Label label17;
		private System.Windows.Forms.Label label16;
		private System.Windows.Forms.ComboBox cbPasswordTimeout;
		private System.Windows.Forms.Label label18;
		private System.Windows.Forms.ComboBox cbPasswordRetries;
		private System.Windows.Forms.Button btnChangePassword;
		private System.Windows.Forms.TextBox tbNewPasssword;
		private System.Windows.Forms.TextBox tbTest;
		private System.Windows.Forms.Label label19;
		private System.Windows.Forms.ComboBox cbBaudRate3;
	}
}