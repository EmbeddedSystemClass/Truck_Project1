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
            this.tbReceived = new System.Windows.Forms.TextBox();
            this.btnConnect = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.tbPort = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tbHostname = new System.Windows.Forms.TextBox();
            this.table = new System.Data.DataTable();
            this.tbConnected = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.cblistCommon = new System.Windows.Forms.CheckedListBox();
            this.btn_SetTime = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.tbServerTime = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.tbEngRunTime = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnShutdown = new System.Windows.Forms.Button();
            this.btnReboot = new System.Windows.Forms.Button();
            this.btnStopSerial = new System.Windows.Forms.Button();
            this.btnDBMgmt = new System.Windows.Forms.Button();
            this.btnClear = new System.Windows.Forms.Button();
            this.btnGetTime = new System.Windows.Forms.Button();
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
            this.tbReceived.Location = new System.Drawing.Point(237, 191);
            this.tbReceived.Multiline = true;
            this.tbReceived.Name = "tbReceived";
            this.tbReceived.ReadOnly = true;
            this.tbReceived.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbReceived.Size = new System.Drawing.Size(201, 328);
            this.tbReceived.TabIndex = 18;
            this.tbReceived.TabStop = false;
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(139, 21);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(75, 23);
            this.btnConnect.TabIndex = 0;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(22, 51);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(28, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "port:";
            // 
            // tbPort
            // 
            this.tbPort.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbPort.Location = new System.Drawing.Point(148, 48);
            this.tbPort.Name = "tbPort";
            this.tbPort.Size = new System.Drawing.Size(31, 20);
            this.tbPort.TabIndex = 12;
            this.tbPort.Text = "5193";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 27);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "hostname:";
            // 
            // tbHostname
            // 
            this.tbHostname.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbHostname.Location = new System.Drawing.Point(85, 24);
            this.tbHostname.Name = "tbHostname";
            this.tbHostname.Size = new System.Drawing.Size(94, 20);
            this.tbHostname.TabIndex = 11;
            this.tbHostname.Text = "192.168.42.146";
            // 
            // tbConnected
            // 
            this.tbConnected.Location = new System.Drawing.Point(95, 74);
            this.tbConnected.Name = "tbConnected";
            this.tbConnected.ReadOnly = true;
            this.tbConnected.Size = new System.Drawing.Size(84, 20);
            this.tbConnected.TabIndex = 13;
            this.tbConnected.TabStop = false;
            this.tbConnected.Text = "not connected";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(19, 76);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(38, 13);
            this.label3.TabIndex = 7;
            this.label3.Text = "status:";
            // 
            // cblistCommon
            // 
            this.cblistCommon.CheckOnClick = true;
            this.cblistCommon.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cblistCommon.FormattingEnabled = true;
            this.cblistCommon.Items.AddRange(new object[] {
            "STARTER",
            "IGNITION",
            "FUELPUMP",
            "COOLINGFAN",
            "LIGHTS",
            "BRIGHTS",
            "LEFT_LIGHTS",
            "RIGHT_LIGHTS",
            "LEFT_BRIGHTS",
            "RIGHT_BRIGHTS",
            "TEST_LEFT_BLINKER",
            "TEST_RIGHT_BLINKER",
            "BRAKES",
            "RUNNING_LIGHTS",
            "SPECIAL_CMD",
            "BLOWER1_ON",
            "BLOWER2_ON",
            "BLOWER3_ON",
            "BLOWER_OFF"});
            this.cblistCommon.Location = new System.Drawing.Point(13, 191);
            this.cblistCommon.Name = "cblistCommon";
            this.cblistCommon.Size = new System.Drawing.Size(202, 327);
            this.cblistCommon.TabIndex = 9;
            this.cblistCommon.SelectedValueChanged += new System.EventHandler(this.cblist_SelValueChanged);
            // 
            // btn_SetTime
            // 
            this.btn_SetTime.Location = new System.Drawing.Point(139, 114);
            this.btn_SetTime.Name = "btn_SetTime";
            this.btn_SetTime.Size = new System.Drawing.Size(75, 23);
            this.btn_SetTime.TabIndex = 3;
            this.btn_SetTime.Text = "Set Time";
            this.btn_SetTime.UseVisualStyleBackColor = true;
            this.btn_SetTime.Click += new System.EventHandler(this.Btn_SetTime_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(139, 81);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 23);
            this.button3.TabIndex = 2;
            this.button3.Text = "Off Engine";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click_1);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(139, 50);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 1;
            this.button2.Text = "Start Engine";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // tbServerTime
            // 
            this.tbServerTime.Location = new System.Drawing.Point(122, 132);
            this.tbServerTime.Name = "tbServerTime";
            this.tbServerTime.Size = new System.Drawing.Size(57, 20);
            this.tbServerTime.TabIndex = 30;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(22, 135);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(71, 13);
            this.label5.TabIndex = 31;
            this.label5.Text = "connect time:";
            // 
            // tbEngRunTime
            // 
            this.tbEngRunTime.Location = new System.Drawing.Point(120, 102);
            this.tbEngRunTime.Name = "tbEngRunTime";
            this.tbEngRunTime.Size = new System.Drawing.Size(59, 20);
            this.tbEngRunTime.TabIndex = 32;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(19, 109);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(78, 13);
            this.label6.TabIndex = 33;
            this.label6.Text = "Eng. Run Time";
            // 
            // groupBox1
            // 
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
            this.groupBox1.Location = new System.Drawing.Point(237, 16);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(201, 169);
            this.groupBox1.TabIndex = 34;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Status";
            // 
            // btnShutdown
            // 
            this.btnShutdown.Location = new System.Drawing.Point(13, 21);
            this.btnShutdown.Name = "btnShutdown";
            this.btnShutdown.Size = new System.Drawing.Size(120, 23);
            this.btnShutdown.TabIndex = 35;
            this.btnShutdown.Text = "Shutdown Server";
            this.btnShutdown.UseVisualStyleBackColor = true;
            this.btnShutdown.Click += new System.EventHandler(this.ShutdownServer);
            // 
            // btnReboot
            // 
            this.btnReboot.Location = new System.Drawing.Point(13, 50);
            this.btnReboot.Name = "btnReboot";
            this.btnReboot.Size = new System.Drawing.Size(120, 23);
            this.btnReboot.TabIndex = 36;
            this.btnReboot.Text = "Reboot Server";
            this.btnReboot.UseVisualStyleBackColor = true;
            this.btnReboot.Click += new System.EventHandler(this.RebootServer);
            // 
            // btnStopSerial
            // 
            this.btnStopSerial.Location = new System.Drawing.Point(13, 81);
            this.btnStopSerial.Name = "btnStopSerial";
            this.btnStopSerial.Size = new System.Drawing.Size(120, 23);
            this.btnStopSerial.TabIndex = 37;
            this.btnStopSerial.Text = "Stop Mbox";
            this.btnStopSerial.UseVisualStyleBackColor = true;
            this.btnStopSerial.Click += new System.EventHandler(this.StopMbox);
            // 
            // btnDBMgmt
            // 
            this.btnDBMgmt.Location = new System.Drawing.Point(12, 114);
            this.btnDBMgmt.Name = "btnDBMgmt";
            this.btnDBMgmt.Size = new System.Drawing.Size(120, 23);
            this.btnDBMgmt.TabIndex = 38;
            this.btnDBMgmt.Text = "DB Mgmt";
            this.btnDBMgmt.UseVisualStyleBackColor = true;
            this.btnDBMgmt.Click += new System.EventHandler(this.DBMgmt);
            // 
            // btnClear
            // 
            this.btnClear.Location = new System.Drawing.Point(13, 143);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(119, 23);
            this.btnClear.TabIndex = 39;
            this.btnClear.Text = "Clear Screen";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.ClearScreen);
            // 
            // btnGetTime
            // 
            this.btnGetTime.Location = new System.Drawing.Point(138, 143);
            this.btnGetTime.Name = "btnGetTime";
            this.btnGetTime.Size = new System.Drawing.Size(75, 23);
            this.btnGetTime.TabIndex = 40;
            this.btnGetTime.Text = "Get Time";
            this.btnGetTime.UseVisualStyleBackColor = true;
            this.btnGetTime.Click += new System.EventHandler(this.GetTime);
            // 
            // FrmSampleClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(460, 534);
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
            this.Controls.Add(this.cblistCommon);
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
        private System.Windows.Forms.CheckedListBox cblistCommon;
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
    }
}

