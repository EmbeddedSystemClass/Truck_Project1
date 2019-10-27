namespace EpServerEngineSampleServer
{
    partial class FrmSampleServer
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
			this.tbPort = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.btnConnect = new System.Windows.Forms.Button();
			this.tbReceived = new System.Windows.Forms.TextBox();
			this.tbServerTime = new System.Windows.Forms.TextBox();
			this.tbEngRunTime = new System.Windows.Forms.TextBox();
			this.tbEngineTemp = new System.Windows.Forms.TextBox();
			this.btnLights = new System.Windows.Forms.Button();
			this.btnFan = new System.Windows.Forms.Button();
			this.btnRunningLights = new System.Windows.Forms.Button();
			this.btnWiper = new System.Windows.Forms.Button();
			this.btnSpecial = new System.Windows.Forms.Button();
			this.button6 = new System.Windows.Forms.Button();
			this.button7 = new System.Windows.Forms.Button();
			this.button8 = new System.Windows.Forms.Button();
			this.button9 = new System.Windows.Forms.Button();
			this.btnShutdown = new System.Windows.Forms.Button();
			this.btnBrights = new System.Windows.Forms.Button();
			this.btnBlower = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.btnConfigParams = new System.Windows.Forms.Button();
			this.gbControls = new System.Windows.Forms.GroupBox();
			this.btnGetParams = new System.Windows.Forms.Button();
			this.btnUploadParams = new System.Windows.Forms.Button();
			this.gbControls.SuspendLayout();
			this.SuspendLayout();
			// 
			// tbPort
			// 
			this.tbPort.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.tbPort.Location = new System.Drawing.Point(46, 12);
			this.tbPort.Name = "tbPort";
			this.tbPort.Size = new System.Drawing.Size(44, 20);
			this.tbPort.TabIndex = 1;
			this.tbPort.Text = "8000";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(12, 15);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(28, 13);
			this.label2.TabIndex = 3;
			this.label2.Text = "port:";
			// 
			// btnConnect
			// 
			this.btnConnect.Location = new System.Drawing.Point(15, 43);
			this.btnConnect.Name = "btnConnect";
			this.btnConnect.Size = new System.Drawing.Size(75, 23);
			this.btnConnect.TabIndex = 4;
			this.btnConnect.Text = "Start";
			this.btnConnect.UseVisualStyleBackColor = true;
			this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
			// 
			// tbReceived
			// 
			this.tbReceived.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.tbReceived.Location = new System.Drawing.Point(13, 93);
			this.tbReceived.Multiline = true;
			this.tbReceived.Name = "tbReceived";
			this.tbReceived.ReadOnly = true;
			this.tbReceived.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbReceived.Size = new System.Drawing.Size(357, 377);
			this.tbReceived.TabIndex = 7;
			// 
			// tbServerTime
			// 
			this.tbServerTime.Location = new System.Drawing.Point(240, 12);
			this.tbServerTime.Name = "tbServerTime";
			this.tbServerTime.Size = new System.Drawing.Size(111, 20);
			this.tbServerTime.TabIndex = 8;
			// 
			// tbEngRunTime
			// 
			this.tbEngRunTime.Location = new System.Drawing.Point(240, 38);
			this.tbEngRunTime.Name = "tbEngRunTime";
			this.tbEngRunTime.Size = new System.Drawing.Size(111, 20);
			this.tbEngRunTime.TabIndex = 9;
			// 
			// tbEngineTemp
			// 
			this.tbEngineTemp.Location = new System.Drawing.Point(240, 64);
			this.tbEngineTemp.Name = "tbEngineTemp";
			this.tbEngineTemp.Size = new System.Drawing.Size(52, 20);
			this.tbEngineTemp.TabIndex = 10;
			// 
			// btnLights
			// 
			this.btnLights.Enabled = false;
			this.btnLights.Location = new System.Drawing.Point(16, 17);
			this.btnLights.Name = "btnLights";
			this.btnLights.Size = new System.Drawing.Size(116, 23);
			this.btnLights.TabIndex = 11;
			this.btnLights.Text = "Lights On";
			this.btnLights.UseVisualStyleBackColor = true;
			this.btnLights.Click += new System.EventHandler(this.btnSendCmd_Click);
			// 
			// btnFan
			// 
			this.btnFan.Enabled = false;
			this.btnFan.Location = new System.Drawing.Point(16, 43);
			this.btnFan.Name = "btnFan";
			this.btnFan.Size = new System.Drawing.Size(116, 23);
			this.btnFan.TabIndex = 12;
			this.btnFan.Text = "Fan On";
			this.btnFan.UseVisualStyleBackColor = true;
			this.btnFan.Click += new System.EventHandler(this.btnFan_Click);
			// 
			// btnRunningLights
			// 
			this.btnRunningLights.Enabled = false;
			this.btnRunningLights.Location = new System.Drawing.Point(16, 69);
			this.btnRunningLights.Name = "btnRunningLights";
			this.btnRunningLights.Size = new System.Drawing.Size(116, 23);
			this.btnRunningLights.TabIndex = 13;
			this.btnRunningLights.Text = "RunLights On";
			this.btnRunningLights.UseVisualStyleBackColor = true;
			this.btnRunningLights.Click += new System.EventHandler(this.btnRunningLights_Click);
			// 
			// btnWiper
			// 
			this.btnWiper.Enabled = false;
			this.btnWiper.Location = new System.Drawing.Point(16, 121);
			this.btnWiper.Name = "btnWiper";
			this.btnWiper.Size = new System.Drawing.Size(116, 23);
			this.btnWiper.TabIndex = 15;
			this.btnWiper.Text = "Wiper On";
			this.btnWiper.UseVisualStyleBackColor = true;
			this.btnWiper.Click += new System.EventHandler(this.btnWiper_Click);
			// 
			// btnSpecial
			// 
			this.btnSpecial.Enabled = false;
			this.btnSpecial.Location = new System.Drawing.Point(16, 147);
			this.btnSpecial.Name = "btnSpecial";
			this.btnSpecial.Size = new System.Drawing.Size(116, 23);
			this.btnSpecial.TabIndex = 16;
			this.btnSpecial.Text = "Special Cmd";
			this.btnSpecial.UseVisualStyleBackColor = true;
			this.btnSpecial.Click += new System.EventHandler(this.btnSpecial_Click);
			// 
			// button6
			// 
			this.button6.Enabled = false;
			this.button6.Location = new System.Drawing.Point(142, 147);
			this.button6.Name = "button6";
			this.button6.Size = new System.Drawing.Size(116, 23);
			this.button6.TabIndex = 22;
			this.button6.UseVisualStyleBackColor = true;
			// 
			// button7
			// 
			this.button7.Enabled = false;
			this.button7.Location = new System.Drawing.Point(142, 121);
			this.button7.Name = "button7";
			this.button7.Size = new System.Drawing.Size(116, 23);
			this.button7.TabIndex = 21;
			this.button7.UseVisualStyleBackColor = true;
			// 
			// button8
			// 
			this.button8.Enabled = false;
			this.button8.Location = new System.Drawing.Point(142, 95);
			this.button8.Name = "button8";
			this.button8.Size = new System.Drawing.Size(116, 23);
			this.button8.TabIndex = 20;
			this.button8.UseVisualStyleBackColor = true;
			// 
			// button9
			// 
			this.button9.Enabled = false;
			this.button9.Location = new System.Drawing.Point(142, 69);
			this.button9.Name = "button9";
			this.button9.Size = new System.Drawing.Size(116, 23);
			this.button9.TabIndex = 19;
			this.button9.UseVisualStyleBackColor = true;
			// 
			// btnShutdown
			// 
			this.btnShutdown.Enabled = false;
			this.btnShutdown.Location = new System.Drawing.Point(142, 43);
			this.btnShutdown.Name = "btnShutdown";
			this.btnShutdown.Size = new System.Drawing.Size(116, 23);
			this.btnShutdown.TabIndex = 18;
			this.btnShutdown.Text = "Shutdown Engine";
			this.btnShutdown.UseVisualStyleBackColor = true;
			this.btnShutdown.Click += new System.EventHandler(this.btnShutdown_Click);
			// 
			// btnBrights
			// 
			this.btnBrights.Enabled = false;
			this.btnBrights.Location = new System.Drawing.Point(142, 17);
			this.btnBrights.Name = "btnBrights";
			this.btnBrights.Size = new System.Drawing.Size(116, 23);
			this.btnBrights.TabIndex = 17;
			this.btnBrights.Text = "Brights Off";
			this.btnBrights.UseVisualStyleBackColor = true;
			this.btnBrights.Click += new System.EventHandler(this.btnBrights_Click);
			// 
			// btnBlower
			// 
			this.btnBlower.Enabled = false;
			this.btnBlower.Location = new System.Drawing.Point(16, 95);
			this.btnBlower.Name = "btnBlower";
			this.btnBlower.Size = new System.Drawing.Size(116, 23);
			this.btnBlower.TabIndex = 24;
			this.btnBlower.Text = "Blower On";
			this.btnBlower.UseVisualStyleBackColor = true;
			this.btnBlower.Click += new System.EventHandler(this.btnBlower_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(149, 15);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(77, 13);
			this.label1.TabIndex = 25;
			this.label1.Text = "Server Uptime:";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(149, 41);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(85, 13);
			this.label3.TabIndex = 26;
			this.label3.Text = "Engine Runtime:";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(149, 63);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(73, 13);
			this.label4.TabIndex = 27;
			this.label4.Text = "Engine Temp:";
			// 
			// btnConfigParams
			// 
			this.btnConfigParams.Location = new System.Drawing.Point(393, 245);
			this.btnConfigParams.Name = "btnConfigParams";
			this.btnConfigParams.Size = new System.Drawing.Size(116, 23);
			this.btnConfigParams.TabIndex = 28;
			this.btnConfigParams.Text = "Config Params";
			this.btnConfigParams.UseVisualStyleBackColor = true;
			this.btnConfigParams.Click += new System.EventHandler(this.btnConfigParams_Click);
			// 
			// gbControls
			// 
			this.gbControls.Controls.Add(this.btnBlower);
			this.gbControls.Controls.Add(this.button6);
			this.gbControls.Controls.Add(this.button7);
			this.gbControls.Controls.Add(this.button8);
			this.gbControls.Controls.Add(this.button9);
			this.gbControls.Controls.Add(this.btnShutdown);
			this.gbControls.Controls.Add(this.btnBrights);
			this.gbControls.Controls.Add(this.btnSpecial);
			this.gbControls.Controls.Add(this.btnWiper);
			this.gbControls.Controls.Add(this.btnRunningLights);
			this.gbControls.Controls.Add(this.btnFan);
			this.gbControls.Controls.Add(this.btnLights);
			this.gbControls.Location = new System.Drawing.Point(391, 20);
			this.gbControls.Name = "gbControls";
			this.gbControls.Size = new System.Drawing.Size(274, 190);
			this.gbControls.TabIndex = 29;
			this.gbControls.TabStop = false;
			this.gbControls.Text = "Controls";
			// 
			// btnGetParams
			// 
			this.btnGetParams.Location = new System.Drawing.Point(393, 216);
			this.btnGetParams.Name = "btnGetParams";
			this.btnGetParams.Size = new System.Drawing.Size(116, 23);
			this.btnGetParams.TabIndex = 30;
			this.btnGetParams.Text = "Get Params";
			this.btnGetParams.UseVisualStyleBackColor = true;
			this.btnGetParams.Click += new System.EventHandler(this.btnGetParams_Click);
			// 
			// btnUploadParams
			// 
			this.btnUploadParams.Location = new System.Drawing.Point(393, 274);
			this.btnUploadParams.Name = "btnUploadParams";
			this.btnUploadParams.Size = new System.Drawing.Size(116, 23);
			this.btnUploadParams.TabIndex = 31;
			this.btnUploadParams.Text = "Upload Params";
			this.btnUploadParams.UseVisualStyleBackColor = true;
			this.btnUploadParams.Click += new System.EventHandler(this.btnUploadParams_Click);
			// 
			// FrmSampleServer
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(678, 480);
			this.Controls.Add(this.btnUploadParams);
			this.Controls.Add(this.btnGetParams);
			this.Controls.Add(this.gbControls);
			this.Controls.Add(this.btnConfigParams);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.tbEngineTemp);
			this.Controls.Add(this.tbEngRunTime);
			this.Controls.Add(this.tbServerTime);
			this.Controls.Add(this.tbReceived);
			this.Controls.Add(this.btnConnect);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.tbPort);
			this.Name = "FrmSampleServer";
			this.Text = "SampleServer";
			this.gbControls.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbPort;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.TextBox tbReceived;
		private System.Windows.Forms.TextBox tbServerTime;
		private System.Windows.Forms.TextBox tbEngRunTime;
		private System.Windows.Forms.TextBox tbEngineTemp;
		private System.Windows.Forms.Button btnLights;
		private System.Windows.Forms.Button btnFan;
		private System.Windows.Forms.Button btnRunningLights;
		private System.Windows.Forms.Button btnWiper;
		private System.Windows.Forms.Button btnSpecial;
		private System.Windows.Forms.Button button6;
		private System.Windows.Forms.Button button7;
		private System.Windows.Forms.Button button8;
		private System.Windows.Forms.Button button9;
		private System.Windows.Forms.Button btnShutdown;
		private System.Windows.Forms.Button btnBrights;
		private System.Windows.Forms.Button btnBlower;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Button btnConfigParams;
		private System.Windows.Forms.GroupBox gbControls;
		private System.Windows.Forms.Button btnGetParams;
		private System.Windows.Forms.Button btnUploadParams;
	}
}

