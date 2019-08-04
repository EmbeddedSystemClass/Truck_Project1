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
			this.table = new System.Data.DataTable();
			this.tbConnected = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.btn_SetTime = new System.Windows.Forms.Button();
			this.btnTestPorts = new System.Windows.Forms.Button();
			this.btnStartEng = new System.Windows.Forms.Button();
			this.tbServerTime = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.tbEngRunTime = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.cbIPAdress = new System.Windows.Forms.ComboBox();
			this.cbAutoConnecct = new System.Windows.Forms.CheckBox();
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
			this.DialogTwo = new System.Windows.Forms.Button();
			this.btnAVR = new System.Windows.Forms.Button();
			this.DialogOne = new System.Windows.Forms.Button();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.tbServerUpTimeSeconds = new System.Windows.Forms.TextBox();
			this.label10 = new System.Windows.Forms.Label();
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
			this.tbReceived.Location = new System.Drawing.Point(491, 395);
			this.tbReceived.Multiline = true;
			this.tbReceived.Name = "tbReceived";
			this.tbReceived.ReadOnly = true;
			this.tbReceived.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbReceived.Size = new System.Drawing.Size(437, 216);
			this.tbReceived.TabIndex = 18;
			this.tbReceived.TabStop = false;
			// 
			// btnConnect
			// 
			this.btnConnect.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnConnect.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnConnect.Location = new System.Drawing.Point(255, 27);
			this.btnConnect.Name = "btnConnect";
			this.btnConnect.Size = new System.Drawing.Size(200, 48);
			this.btnConnect.TabIndex = 0;
			this.btnConnect.Text = "Connect Server";
			this.btnConnect.UseVisualStyleBackColor = false;
			this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label2.Location = new System.Drawing.Point(13, 84);
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
			this.label1.Location = new System.Drawing.Point(13, 40);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(49, 20);
			this.label1.TabIndex = 1;
			this.label1.Text = "host:";
			// 
			// tbConnected
			// 
			this.tbConnected.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbConnected.Location = new System.Drawing.Point(108, 175);
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
			this.label3.Location = new System.Drawing.Point(13, 184);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(67, 20);
			this.label3.TabIndex = 7;
			this.label3.Text = "Status:";
			// 
			// btn_SetTime
			// 
			this.btn_SetTime.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btn_SetTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btn_SetTime.Location = new System.Drawing.Point(255, 239);
			this.btn_SetTime.Name = "btn_SetTime";
			this.btn_SetTime.Size = new System.Drawing.Size(200, 48);
			this.btn_SetTime.TabIndex = 3;
			this.btn_SetTime.Text = "Set Time";
			this.btn_SetTime.UseVisualStyleBackColor = false;
			this.btn_SetTime.Click += new System.EventHandler(this.Btn_SetTime_Click);
			// 
			// btnTestPorts
			// 
			this.btnTestPorts.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnTestPorts.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnTestPorts.Location = new System.Drawing.Point(255, 166);
			this.btnTestPorts.Name = "btnTestPorts";
			this.btnTestPorts.Size = new System.Drawing.Size(200, 48);
			this.btnTestPorts.TabIndex = 2;
			this.btnTestPorts.Text = "Test Ports";
			this.btnTestPorts.UseVisualStyleBackColor = false;
			this.btnTestPorts.Click += new System.EventHandler(this.TestPorts_Click);
			// 
			// btnStartEng
			// 
			this.btnStartEng.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnStartEng.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnStartEng.Location = new System.Drawing.Point(255, 95);
			this.btnStartEng.Name = "btnStartEng";
			this.btnStartEng.Size = new System.Drawing.Size(200, 48);
			this.btnStartEng.TabIndex = 1;
			this.btnStartEng.Text = "Start Engine";
			this.btnStartEng.UseVisualStyleBackColor = false;
			this.btnStartEng.Click += new System.EventHandler(this.StartEng_Click);
			// 
			// tbServerTime
			// 
			this.tbServerTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbServerTime.Location = new System.Drawing.Point(108, 218);
			this.tbServerTime.Name = "tbServerTime";
			this.tbServerTime.Size = new System.Drawing.Size(149, 29);
			this.tbServerTime.TabIndex = 30;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label5.Location = new System.Drawing.Point(13, 224);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(94, 20);
			this.label5.TabIndex = 31;
			this.label5.Text = "Server Up:";
			// 
			// tbEngRunTime
			// 
			this.tbEngRunTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbEngRunTime.Location = new System.Drawing.Point(108, 259);
			this.tbEngRunTime.Name = "tbEngRunTime";
			this.tbEngRunTime.Size = new System.Drawing.Size(149, 29);
			this.tbEngRunTime.TabIndex = 32;
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label6.Location = new System.Drawing.Point(13, 266);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(89, 20);
			this.label6.TabIndex = 33;
			this.label6.Text = "Eng. Time";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.cbIPAdress);
			this.groupBox1.Controls.Add(this.cbAutoConnecct);
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
			// cbIPAdress
			// 
			this.cbIPAdress.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbIPAdress.FormattingEnabled = true;
			this.cbIPAdress.Location = new System.Drawing.Point(77, 35);
			this.cbIPAdress.Name = "cbIPAdress";
			this.cbIPAdress.Size = new System.Drawing.Size(180, 28);
			this.cbIPAdress.TabIndex = 41;
			this.cbIPAdress.SelectedIndexChanged += new System.EventHandler(this.IPAddressChanged);
			// 
			// cbAutoConnecct
			// 
			this.cbAutoConnecct.AutoSize = true;
			this.cbAutoConnecct.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbAutoConnecct.Location = new System.Drawing.Point(154, 80);
			this.cbAutoConnecct.Name = "cbAutoConnecct";
			this.cbAutoConnecct.Size = new System.Drawing.Size(156, 28);
			this.cbAutoConnecct.TabIndex = 40;
			this.cbAutoConnecct.Text = "Auto Connect";
			this.cbAutoConnecct.UseVisualStyleBackColor = true;
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label8.Location = new System.Drawing.Point(263, 271);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(47, 20);
			this.label8.TabIndex = 39;
			this.label8.Text = "MPH";
			// 
			// tbMPH
			// 
			this.tbMPH.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbMPH.Location = new System.Drawing.Point(344, 260);
			this.tbMPH.Name = "tbMPH";
			this.tbMPH.Size = new System.Drawing.Size(76, 29);
			this.tbMPH.TabIndex = 38;
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label7.Location = new System.Drawing.Point(263, 229);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(47, 20);
			this.label7.TabIndex = 37;
			this.label7.Text = "RPM";
			// 
			// tbRPM
			// 
			this.tbRPM.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbRPM.Location = new System.Drawing.Point(344, 218);
			this.tbRPM.Name = "tbRPM";
			this.tbRPM.Size = new System.Drawing.Size(76, 29);
			this.tbRPM.TabIndex = 36;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label4.Location = new System.Drawing.Point(263, 187);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(58, 20);
			this.label4.TabIndex = 35;
			this.label4.Text = "Temp:";
			// 
			// tbEngineTemp
			// 
			this.tbEngineTemp.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbEngineTemp.Location = new System.Drawing.Point(344, 176);
			this.tbEngineTemp.Name = "tbEngineTemp";
			this.tbEngineTemp.Size = new System.Drawing.Size(76, 29);
			this.tbEngineTemp.TabIndex = 34;
			// 
			// btnShutdown
			// 
			this.btnShutdown.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnShutdown.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnShutdown.Location = new System.Drawing.Point(34, 27);
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
			this.btnReboot.Location = new System.Drawing.Point(34, 95);
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
			this.btnStopSerial.Location = new System.Drawing.Point(34, 166);
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
			this.btnDBMgmt.Location = new System.Drawing.Point(34, 239);
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
			this.btnClear.Location = new System.Drawing.Point(34, 308);
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
			this.btnGetTime.Location = new System.Drawing.Point(255, 307);
			this.btnGetTime.Name = "btnGetTime";
			this.btnGetTime.Size = new System.Drawing.Size(200, 48);
			this.btnGetTime.TabIndex = 4;
			this.btnGetTime.Text = "Get Time";
			this.btnGetTime.UseVisualStyleBackColor = false;
			this.btnGetTime.Click += new System.EventHandler(this.GetTime);
			// 
			// btnSetParams
			// 
			this.btnSetParams.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnSetParams.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnSetParams.Location = new System.Drawing.Point(34, 376);
			this.btnSetParams.Name = "btnSetParams";
			this.btnSetParams.Size = new System.Drawing.Size(200, 48);
			this.btnSetParams.TabIndex = 12;
			this.btnSetParams.Text = "Set Svr Params";
			this.btnSetParams.UseVisualStyleBackColor = false;
			this.btnSetParams.Click += new System.EventHandler(this.SetParamsClick);
			// 
			// DialogTwo
			// 
			this.DialogTwo.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.DialogTwo.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.DialogTwo.Location = new System.Drawing.Point(255, 440);
			this.DialogTwo.Name = "DialogTwo";
			this.DialogTwo.Size = new System.Drawing.Size(200, 48);
			this.DialogTwo.TabIndex = 6;
			this.DialogTwo.Text = "Settings Two";
			this.DialogTwo.UseVisualStyleBackColor = false;
			this.DialogTwo.Click += new System.EventHandler(this.Dialog2_Click);
			// 
			// btnAVR
			// 
			this.btnAVR.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnAVR.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnAVR.Location = new System.Drawing.Point(255, 376);
			this.btnAVR.Name = "btnAVR";
			this.btnAVR.Size = new System.Drawing.Size(200, 48);
			this.btnAVR.TabIndex = 5;
			this.btnAVR.Text = "Set LCD on";
			this.btnAVR.UseVisualStyleBackColor = false;
			this.btnAVR.Click += new System.EventHandler(this.btnAVR_Click);
			// 
			// DialogOne
			// 
			this.DialogOne.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.DialogOne.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.DialogOne.Location = new System.Drawing.Point(34, 440);
			this.DialogOne.Name = "DialogOne";
			this.DialogOne.Size = new System.Drawing.Size(200, 48);
			this.DialogOne.TabIndex = 13;
			this.DialogOne.Text = "Settings One";
			this.DialogOne.UseVisualStyleBackColor = false;
			this.DialogOne.Click += new System.EventHandler(this.Dialog1_Click);
			// 
			// timer1
			// 
			this.timer1.Interval = 2000;
			this.timer1.Tick += new System.EventHandler(this.myTimerTick);
			// 
			// tbServerUpTimeSeconds
			// 
			this.tbServerUpTimeSeconds.Location = new System.Drawing.Point(620, 354);
			this.tbServerUpTimeSeconds.Name = "tbServerUpTimeSeconds";
			this.tbServerUpTimeSeconds.Size = new System.Drawing.Size(308, 20);
			this.tbServerUpTimeSeconds.TabIndex = 35;
			// 
			// label10
			// 
			this.label10.AutoSize = true;
			this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label10.Location = new System.Drawing.Point(487, 352);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(127, 20);
			this.label10.TabIndex = 36;
			this.label10.Text = "server up time ";
			// 
			// FrmSampleClient
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
			this.ClientSize = new System.Drawing.Size(958, 631);
			this.Controls.Add(this.label10);
			this.Controls.Add(this.tbServerUpTimeSeconds);
			this.Controls.Add(this.DialogOne);
			this.Controls.Add(this.btnAVR);
			this.Controls.Add(this.DialogTwo);
			this.Controls.Add(this.btnSetParams);
			this.Controls.Add(this.btnGetTime);
			this.Controls.Add(this.btnClear);
			this.Controls.Add(this.btnDBMgmt);
			this.Controls.Add(this.btnStopSerial);
			this.Controls.Add(this.btnReboot);
			this.Controls.Add(this.btnShutdown);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.btn_SetTime);
			this.Controls.Add(this.btnTestPorts);
			this.Controls.Add(this.btnStartEng);
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
        private System.Data.DataTable table;
        private System.Data.SqlClient.SqlConnection conn;
//        private System.Data.SqlClient.SqlCommand cmd;


        // desktop
        //string connectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL-DB.mdf;Integrated Security=True;Connect Timeout=30";
        // laptop

        //string currentconnectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL.mdf;Integrated Security=True;Connect Timeout=30";
        string connectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL.mdf;Integrated Security=True;Connect Timeout=30";

        private System.Windows.Forms.TextBox tbConnected;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btn_SetTime;
        private System.Windows.Forms.Button btnTestPorts;
        private System.Windows.Forms.Button btnStartEng;
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
        private System.Windows.Forms.Button DialogTwo;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbEngineTemp;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox tbMPH;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox tbRPM;
        private System.Windows.Forms.Button btnAVR;
		private System.Windows.Forms.Button DialogOne;
		private System.Windows.Forms.Timer timer1;
		private System.Windows.Forms.ComboBox cbIPAdress;
		private System.Windows.Forms.CheckBox cbAutoConnecct;
		private System.Windows.Forms.TextBox tbServerUpTimeSeconds;
		private System.Windows.Forms.Label label10;
	}
}

