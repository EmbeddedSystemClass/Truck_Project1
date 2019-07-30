namespace EpServerEngineSampleClient
{
    partial class FrmSampleClient
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
			this.components = new System.ComponentModel.Container();
			this.tbReceived = new System.Windows.Forms.TextBox();
			this.btnConnect = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.tbPort = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.tbHostname = new System.Windows.Forms.TextBox();
			this.table = new System.Data.DataTable();
			this.tbConnected = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.btn_SetTime = new System.Windows.Forms.Button();
			this.button3 = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.tbServerTime = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.tbEngRunTime = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label8 = new System.Windows.Forms.Label();
			this.tbMPH = new System.Windows.Forms.TextBox();
			this.label7 = new System.Windows.Forms.Label();
			this.tbRPM = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.tbEngineTemp = new System.Windows.Forms.TextBox();
			this.btnShutdown = new System.Windows.Forms.Button();
			this.btnReboot = new System.Windows.Forms.Button();
			this.btnStopSerial = new System.Windows.Forms.Button();
			this.btnDBMgmt = new System.Windows.Forms.Button();
			this.btnClear = new System.Windows.Forms.Button();
			this.btnGetTime = new System.Windows.Forms.Button();
			this.btnSetParams = new System.Windows.Forms.Button();
			this.Upload_New = new System.Windows.Forms.Button();
			this.btnPortSettings = new System.Windows.Forms.Button();
			this.btnLoadXML = new System.Windows.Forms.Button();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			((System.ComponentModel.ISupportInitialize)(this.table)).BeginInit();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// tbReceived
			// 
			this.tbReceived.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.tbReceived.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbReceived.Location = new System.Drawing.Point(491, 394);
			this.tbReceived.Multiline = true;
			this.tbReceived.Name = "tbReceived";
			this.tbReceived.ReadOnly = true;
			this.tbReceived.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbReceived.Size = new System.Drawing.Size(437, 124);
			this.tbReceived.TabIndex = 18;
			this.tbReceived.TabStop = false;
			// 
			// btnConnect
			// 
			this.btnConnect.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnConnect.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnConnect.Location = new System.Drawing.Point(267, 58);
			this.btnConnect.Name = "btnConnect";
			this.btnConnect.Size = new System.Drawing.Size(200, 48);
			this.btnConnect.TabIndex = 0;
			this.btnConnect.Text = "Connect";
			this.btnConnect.UseVisualStyleBackColor = false;
			this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label2.Location = new System.Drawing.Point(13, 86);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(46, 20);
			this.label2.TabIndex = 3;
			this.label2.Text = "port:";
			// 
			// tbPort
			// 
			this.tbPort.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.tbPort.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbPort.Location = new System.Drawing.Point(77, 79);
			this.tbPort.Name = "tbPort";
			this.tbPort.Size = new System.Drawing.Size(68, 29);
			this.tbPort.TabIndex = 12;
			this.tbPort.Text = "5193";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(13, 46);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(49, 20);
			this.label1.TabIndex = 1;
			this.label1.Text = "host:";
			// 
			// tbHostname
			// 
			this.tbHostname.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.tbHostname.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbHostname.Location = new System.Drawing.Point(77, 37);
			this.tbHostname.Name = "tbHostname";
			this.tbHostname.Size = new System.Drawing.Size(330, 29);
			this.tbHostname.TabIndex = 0;
			this.tbHostname.Text = "192.168.42.145";
			// 
			// tbConnected
			// 
			this.tbConnected.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbConnected.Location = new System.Drawing.Point(108, 133);
			this.tbConnected.Name = "tbConnected";
			this.tbConnected.ReadOnly = true;
			this.tbConnected.Size = new System.Drawing.Size(149, 29);
			this.tbConnected.TabIndex = 13;
			this.tbConnected.TabStop = false;
			this.tbConnected.Text = "not connected";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label3.Location = new System.Drawing.Point(13, 142);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(67, 20);
			this.label3.TabIndex = 7;
			this.label3.Text = "Status:";
			// 
			// btn_SetTime
			// 
			this.btn_SetTime.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btn_SetTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btn_SetTime.Location = new System.Drawing.Point(267, 338);
			this.btn_SetTime.Name = "btn_SetTime";
			this.btn_SetTime.Size = new System.Drawing.Size(200, 48);
			this.btn_SetTime.TabIndex = 4;
			this.btn_SetTime.Text = "Set Time";
			this.btn_SetTime.UseVisualStyleBackColor = false;
			this.btn_SetTime.Click += new System.EventHandler(this.Btn_SetTime_Click);
			// 
			// button3
			// 
			this.button3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.button3.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.button3.Location = new System.Drawing.Point(267, 270);
			this.button3.Name = "button3";
			this.button3.Size = new System.Drawing.Size(200, 48);
			this.button3.TabIndex = 3;
			this.button3.Text = "Off Engine";
			this.button3.UseVisualStyleBackColor = false;
			this.button3.Click += new System.EventHandler(this.shutdown_Click_1);
			// 
			// button2
			// 
			this.button2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.button2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.button2.Location = new System.Drawing.Point(267, 197);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(200, 48);
			this.button2.TabIndex = 2;
			this.button2.Text = "Start Engine";
			this.button2.UseVisualStyleBackColor = false;
			this.button2.Click += new System.EventHandler(this.button2_Click);
			// 
			// tbServerTime
			// 
			this.tbServerTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbServerTime.Location = new System.Drawing.Point(108, 176);
			this.tbServerTime.Name = "tbServerTime";
			this.tbServerTime.Size = new System.Drawing.Size(149, 29);
			this.tbServerTime.TabIndex = 30;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label5.Location = new System.Drawing.Point(13, 182);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(94, 20);
			this.label5.TabIndex = 31;
			this.label5.Text = "Server Up:";
			// 
			// tbEngRunTime
			// 
			this.tbEngRunTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbEngRunTime.Location = new System.Drawing.Point(108, 217);
			this.tbEngRunTime.Name = "tbEngRunTime";
			this.tbEngRunTime.Size = new System.Drawing.Size(149, 29);
			this.tbEngRunTime.TabIndex = 32;
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label6.Location = new System.Drawing.Point(13, 224);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(89, 20);
			this.label6.TabIndex = 33;
			this.label6.Text = "Eng. Time";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label8);
			this.groupBox1.Controls.Add(this.tbMPH);
			this.groupBox1.Controls.Add(this.label7);
			this.groupBox1.Controls.Add(this.tbRPM);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.tbEngineTemp);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.tbEngRunTime);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.tbServerTime);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.tbConnected);
			this.groupBox1.Controls.Add(this.tbHostname);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.tbPort);
			this.groupBox1.Location = new System.Drawing.Point(491, 21);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(437, 313);
			this.groupBox1.TabIndex = 34;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Status";
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label8.Location = new System.Drawing.Point(263, 229);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(47, 20);
			this.label8.TabIndex = 39;
			this.label8.Text = "MPH";
			// 
			// tbMPH
			// 
			this.tbMPH.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbMPH.Location = new System.Drawing.Point(344, 218);
			this.tbMPH.Name = "tbMPH";
			this.tbMPH.Size = new System.Drawing.Size(76, 29);
			this.tbMPH.TabIndex = 38;
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label7.Location = new System.Drawing.Point(263, 187);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(47, 20);
			this.label7.TabIndex = 37;
			this.label7.Text = "RPM";
			// 
			// tbRPM
			// 
			this.tbRPM.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbRPM.Location = new System.Drawing.Point(344, 176);
			this.tbRPM.Name = "tbRPM";
			this.tbRPM.Size = new System.Drawing.Size(76, 29);
			this.tbRPM.TabIndex = 36;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label4.Location = new System.Drawing.Point(263, 145);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(58, 20);
			this.label4.TabIndex = 35;
			this.label4.Text = "Temp:";
			// 
			// tbEngineTemp
			// 
			this.tbEngineTemp.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbEngineTemp.Location = new System.Drawing.Point(344, 134);
			this.tbEngineTemp.Name = "tbEngineTemp";
			this.tbEngineTemp.Size = new System.Drawing.Size(76, 29);
			this.tbEngineTemp.TabIndex = 34;
			// 
			// btnShutdown
			// 
			this.btnShutdown.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnShutdown.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnShutdown.Location = new System.Drawing.Point(46, 58);
			this.btnShutdown.Name = "btnShutdown";
			this.btnShutdown.Size = new System.Drawing.Size(200, 48);
			this.btnShutdown.TabIndex = 7;
			this.btnShutdown.Text = "Shutdown Server";
			this.btnShutdown.UseVisualStyleBackColor = false;
			this.btnShutdown.Click += new System.EventHandler(this.ShutdownServer);
			// 
			// btnReboot
			// 
			this.btnReboot.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnReboot.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnReboot.Location = new System.Drawing.Point(46, 126);
			this.btnReboot.Name = "btnReboot";
			this.btnReboot.Size = new System.Drawing.Size(200, 48);
			this.btnReboot.TabIndex = 8;
			this.btnReboot.Text = "Reboot Server";
			this.btnReboot.UseVisualStyleBackColor = false;
			this.btnReboot.Click += new System.EventHandler(this.RebootServer);
			// 
			// btnStopSerial
			// 
			this.btnStopSerial.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnStopSerial.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnStopSerial.Location = new System.Drawing.Point(46, 197);
			this.btnStopSerial.Name = "btnStopSerial";
			this.btnStopSerial.Size = new System.Drawing.Size(200, 48);
			this.btnStopSerial.TabIndex = 9;
			this.btnStopSerial.Text = "Stop Mbox Xmit";
			this.btnStopSerial.UseVisualStyleBackColor = false;
			this.btnStopSerial.Click += new System.EventHandler(this.StopMbox);
			// 
			// btnDBMgmt
			// 
			this.btnDBMgmt.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnDBMgmt.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnDBMgmt.Location = new System.Drawing.Point(46, 270);
			this.btnDBMgmt.Name = "btnDBMgmt";
			this.btnDBMgmt.Size = new System.Drawing.Size(200, 48);
			this.btnDBMgmt.TabIndex = 10;
			this.btnDBMgmt.Text = "DB Mgmt";
			this.btnDBMgmt.UseVisualStyleBackColor = false;
			this.btnDBMgmt.Click += new System.EventHandler(this.DBMgmt);
			// 
			// btnClear
			// 
			this.btnClear.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnClear.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnClear.Location = new System.Drawing.Point(46, 339);
			this.btnClear.Name = "btnClear";
			this.btnClear.Size = new System.Drawing.Size(200, 48);
			this.btnClear.TabIndex = 11;
			this.btnClear.Text = "Clear Screen";
			this.btnClear.UseVisualStyleBackColor = false;
			this.btnClear.Click += new System.EventHandler(this.ClearScreen);
			// 
			// btnGetTime
			// 
			this.btnGetTime.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnGetTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnGetTime.Location = new System.Drawing.Point(267, 407);
			this.btnGetTime.Name = "btnGetTime";
			this.btnGetTime.Size = new System.Drawing.Size(200, 48);
			this.btnGetTime.TabIndex = 5;
			this.btnGetTime.Text = "Get Time";
			this.btnGetTime.UseVisualStyleBackColor = false;
			this.btnGetTime.Click += new System.EventHandler(this.GetTime);
			// 
			// btnSetParams
			// 
			this.btnSetParams.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnSetParams.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnSetParams.Location = new System.Drawing.Point(46, 407);
			this.btnSetParams.Name = "btnSetParams";
			this.btnSetParams.Size = new System.Drawing.Size(200, 48);
			this.btnSetParams.TabIndex = 12;
			this.btnSetParams.Text = "Set Svr Params";
			this.btnSetParams.UseVisualStyleBackColor = false;
			this.btnSetParams.Click += new System.EventHandler(this.SetParamsClick);
			// 
			// Upload_New
			// 
			this.Upload_New.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.Upload_New.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.Upload_New.Location = new System.Drawing.Point(267, 471);
			this.Upload_New.Name = "Upload_New";
			this.Upload_New.Size = new System.Drawing.Size(200, 48);
			this.Upload_New.TabIndex = 6;
			this.Upload_New.Text = "Upload New";
			this.Upload_New.UseVisualStyleBackColor = false;
			this.Upload_New.Click += new System.EventHandler(this.Upload_New_Click);
			// 
			// btnPortSettings
			// 
			this.btnPortSettings.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnPortSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnPortSettings.Location = new System.Drawing.Point(267, 126);
			this.btnPortSettings.Name = "btnPortSettings";
			this.btnPortSettings.Size = new System.Drawing.Size(200, 48);
			this.btnPortSettings.TabIndex = 1;
			this.btnPortSettings.Text = "Port Settings";
			this.btnPortSettings.UseVisualStyleBackColor = false;
			this.btnPortSettings.Click += new System.EventHandler(this.btnPortSettings_Click);
			// 
			// btnLoadXML
			// 
			this.btnLoadXML.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnLoadXML.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnLoadXML.Location = new System.Drawing.Point(46, 471);
			this.btnLoadXML.Name = "btnLoadXML";
			this.btnLoadXML.Size = new System.Drawing.Size(200, 48);
			this.btnLoadXML.TabIndex = 13;
			this.btnLoadXML.Text = "Load XML";
			this.btnLoadXML.UseVisualStyleBackColor = false;
			this.btnLoadXML.Click += new System.EventHandler(this.btnLoadXML_Click);
			// 
			// textBox1
			// 
			this.textBox1.Location = new System.Drawing.Point(491, 355);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(310, 20);
			this.textBox1.TabIndex = 35;
			// 
			// timer1
			// 
			this.timer1.Interval = 5000;
			this.timer1.Tick += new System.EventHandler(this.myTimerTick);
			// 
			// FrmSampleClient
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
			this.ClientSize = new System.Drawing.Size(958, 535);
			this.Controls.Add(this.textBox1);
			this.Controls.Add(this.btnLoadXML);
			this.Controls.Add(this.btnPortSettings);
			this.Controls.Add(this.Upload_New);
			this.Controls.Add(this.btnSetParams);
			this.Controls.Add(this.btnGetTime);
			this.Controls.Add(this.btnClear);
			this.Controls.Add(this.btnDBMgmt);
			this.Controls.Add(this.btnStopSerial);
			this.Controls.Add(this.btnReboot);
			this.Controls.Add(this.btnShutdown);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.btn_SetTime);
			this.Controls.Add(this.button3);
			this.Controls.Add(this.button2);
			this.Controls.Add(this.tbReceived);
			this.Controls.Add(this.btnConnect);
			this.Name = "FrmSampleClient";
			this.Text = "SampleClient";
			((System.ComponentModel.ISupportInitialize)(this.table)).EndInit();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbReceived;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbPort;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbHostname;
        private System.Data.DataTable table;
        private System.Data.SqlClient.SqlConnection conn;
//        private System.Data.SqlClient.SqlCommand cmd;


        // desktop
        //string connectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL-DB.mdf;Integrated Security=True;Connect Timeout=30";
        // laptop

        string currentconnectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL.mdf;Integrated Security=True;Connect Timeout=30";
        string connectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL.mdf;Integrated Security=True;Connect Timeout=30";

        private System.Windows.Forms.TextBox tbConnected;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btn_SetTime;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.TextBox tbServerTime;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox tbEngRunTime;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnShutdown;
        private System.Windows.Forms.Button btnReboot;
        private System.Windows.Forms.Button btnStopSerial;
        private System.Windows.Forms.Button btnDBMgmt;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.Button btnGetTime;
        private System.Windows.Forms.Button btnSetParams;
        private System.Windows.Forms.Button Upload_New;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbEngineTemp;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox tbMPH;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox tbRPM;
        private System.Windows.Forms.Button btnPortSettings;
		private System.Windows.Forms.Button btnLoadXML;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.Timer timer1;
	}
}

