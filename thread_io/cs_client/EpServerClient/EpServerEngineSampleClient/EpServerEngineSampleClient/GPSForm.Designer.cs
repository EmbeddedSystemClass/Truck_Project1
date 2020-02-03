namespace EpServerEngineSampleClient
{
	partial class GPSForm
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
			this.btnSend = new System.Windows.Forms.Button();
			this.tbAddMsg = new System.Windows.Forms.TextBox();
			this.cbMsg = new System.Windows.Forms.ComboBox();
			this.lbUpdateRate = new System.Windows.Forms.ListBox();
			this.label1 = new System.Windows.Forms.Label();
			this.lbCurrentList = new System.Windows.Forms.ListBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.btnPause = new System.Windows.Forms.Button();
			this.button1 = new System.Windows.Forms.Button();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.tbGGA = new System.Windows.Forms.TextBox();
			this.tbGLL = new System.Windows.Forms.TextBox();
			this.tbGSA = new System.Windows.Forms.TextBox();
			this.tbVTG = new System.Windows.Forms.TextBox();
			this.tbRMC = new System.Windows.Forms.TextBox();
			this.tbGSV = new System.Windows.Forms.TextBox();
			this.lbGLL = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// btnSend
			// 
			this.btnSend.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnSend.Location = new System.Drawing.Point(35, 70);
			this.btnSend.Name = "btnSend";
			this.btnSend.Size = new System.Drawing.Size(75, 23);
			this.btnSend.TabIndex = 6;
			this.btnSend.Text = "Send";
			this.btnSend.UseVisualStyleBackColor = false;
			this.btnSend.Click += new System.EventHandler(this.btnSend_Click);
			// 
			// tbAddMsg
			// 
			this.tbAddMsg.Location = new System.Drawing.Point(12, 362);
			this.tbAddMsg.Multiline = true;
			this.tbAddMsg.Name = "tbAddMsg";
			this.tbAddMsg.ReadOnly = true;
			this.tbAddMsg.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbAddMsg.Size = new System.Drawing.Size(522, 383);
			this.tbAddMsg.TabIndex = 8;
			// 
			// cbMsg
			// 
			this.cbMsg.FormattingEnabled = true;
			this.cbMsg.Location = new System.Drawing.Point(35, 41);
			this.cbMsg.Name = "cbMsg";
			this.cbMsg.Size = new System.Drawing.Size(121, 21);
			this.cbMsg.TabIndex = 9;
			this.cbMsg.SelectedIndexChanged += new System.EventHandler(this.cbMsg_SelectedIndexChanged);
			// 
			// lbUpdateRate
			// 
			this.lbUpdateRate.FormattingEnabled = true;
			this.lbUpdateRate.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10"});
			this.lbUpdateRate.Location = new System.Drawing.Point(285, 21);
			this.lbUpdateRate.Name = "lbUpdateRate";
			this.lbUpdateRate.Size = new System.Drawing.Size(47, 43);
			this.lbUpdateRate.TabIndex = 11;
			this.lbUpdateRate.SelectedIndexChanged += new System.EventHandler(this.listBox1_SelectedIndexChanged);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(185, 21);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(94, 13);
			this.label1.TabIndex = 12;
			this.label1.Text = "Update Rate (sec)";
			// 
			// lbCurrentList
			// 
			this.lbCurrentList.FormattingEnabled = true;
			this.lbCurrentList.Location = new System.Drawing.Point(166, 86);
			this.lbCurrentList.Name = "lbCurrentList";
			this.lbCurrentList.Size = new System.Drawing.Size(120, 134);
			this.lbCurrentList.TabIndex = 13;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(163, 70);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(83, 13);
			this.label2.TabIndex = 14;
			this.label2.Text = "Current Msg List";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(32, 21);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(101, 13);
			this.label3.TabIndex = 15;
			this.label3.Text = "Message to change";
			// 
			// btnPause
			// 
			this.btnPause.Location = new System.Drawing.Point(559, 722);
			this.btnPause.Name = "btnPause";
			this.btnPause.Size = new System.Drawing.Size(75, 23);
			this.btnPause.TabIndex = 16;
			this.btnPause.Text = "Pause Output";
			this.btnPause.UseVisualStyleBackColor = true;
			this.btnPause.Click += new System.EventHandler(this.btnPause_Click);
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(1036, 286);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(114, 32);
			this.button1.TabIndex = 17;
			this.button1.Text = "button1";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.lbCurrentList);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.lbUpdateRate);
			this.groupBox1.Controls.Add(this.cbMsg);
			this.groupBox1.Controls.Add(this.btnSend);
			this.groupBox1.Location = new System.Drawing.Point(797, 13);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(353, 252);
			this.groupBox1.TabIndex = 18;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Send Data";
			// 
			// tbGGA
			// 
			this.tbGGA.Location = new System.Drawing.Point(14, 43);
			this.tbGGA.Multiline = true;
			this.tbGGA.Name = "tbGGA";
			this.tbGGA.ReadOnly = true;
			this.tbGGA.Size = new System.Drawing.Size(193, 128);
			this.tbGGA.TabIndex = 19;
			// 
			// tbGLL
			// 
			this.tbGLL.Location = new System.Drawing.Point(232, 43);
			this.tbGLL.Multiline = true;
			this.tbGLL.Name = "tbGLL";
			this.tbGLL.ReadOnly = true;
			this.tbGLL.Size = new System.Drawing.Size(193, 128);
			this.tbGLL.TabIndex = 20;
			// 
			// tbGSA
			// 
			this.tbGSA.Location = new System.Drawing.Point(455, 43);
			this.tbGSA.Multiline = true;
			this.tbGSA.Name = "tbGSA";
			this.tbGSA.ReadOnly = true;
			this.tbGSA.Size = new System.Drawing.Size(193, 128);
			this.tbGSA.TabIndex = 21;
			// 
			// tbVTG
			// 
			this.tbVTG.Location = new System.Drawing.Point(455, 209);
			this.tbVTG.Multiline = true;
			this.tbVTG.Name = "tbVTG";
			this.tbVTG.ReadOnly = true;
			this.tbVTG.Size = new System.Drawing.Size(193, 128);
			this.tbVTG.TabIndex = 24;
			// 
			// tbRMC
			// 
			this.tbRMC.Location = new System.Drawing.Point(232, 209);
			this.tbRMC.Multiline = true;
			this.tbRMC.Name = "tbRMC";
			this.tbRMC.ReadOnly = true;
			this.tbRMC.Size = new System.Drawing.Size(193, 128);
			this.tbRMC.TabIndex = 23;
			// 
			// tbGSV
			// 
			this.tbGSV.Location = new System.Drawing.Point(14, 209);
			this.tbGSV.Multiline = true;
			this.tbGSV.Name = "tbGSV";
			this.tbGSV.ReadOnly = true;
			this.tbGSV.Size = new System.Drawing.Size(193, 128);
			this.tbGSV.TabIndex = 22;
			// 
			// lbGLL
			// 
			this.lbGLL.AutoSize = true;
			this.lbGLL.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbGLL.Location = new System.Drawing.Point(14, 17);
			this.lbGLL.Name = "lbGLL";
			this.lbGLL.Size = new System.Drawing.Size(49, 20);
			this.lbGLL.TabIndex = 25;
			this.lbGLL.Text = "GGA";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label4.Location = new System.Drawing.Point(462, 185);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(45, 20);
			this.label4.TabIndex = 26;
			this.label4.Text = "VTG";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label5.Location = new System.Drawing.Point(238, 186);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(48, 20);
			this.label5.TabIndex = 27;
			this.label5.Text = "RMC";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label6.Location = new System.Drawing.Point(14, 185);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(47, 20);
			this.label6.TabIndex = 28;
			this.label6.Text = "GSV";
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label7.Location = new System.Drawing.Point(462, 17);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(47, 20);
			this.label7.TabIndex = 29;
			this.label7.Text = "GSA";
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label8.Location = new System.Drawing.Point(238, 17);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(43, 20);
			this.label8.TabIndex = 30;
			this.label8.Text = "GLL";
			// 
			// GPSForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
			this.ClientSize = new System.Drawing.Size(1166, 757);
			this.Controls.Add(this.label8);
			this.Controls.Add(this.label7);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.lbGLL);
			this.Controls.Add(this.tbVTG);
			this.Controls.Add(this.tbRMC);
			this.Controls.Add(this.tbGSV);
			this.Controls.Add(this.tbGSA);
			this.Controls.Add(this.tbGLL);
			this.Controls.Add(this.tbGGA);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.btnPause);
			this.Controls.Add(this.tbAddMsg);
			this.Name = "GPSForm";
			this.Text = "GPSForm";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion
		private System.Windows.Forms.Button btnSend;
		private System.Windows.Forms.TextBox tbAddMsg;
		private System.Windows.Forms.ComboBox cbMsg;
		private System.Windows.Forms.ListBox lbUpdateRate;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.ListBox lbCurrentList;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Button btnPause;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TextBox tbGGA;
		private System.Windows.Forms.TextBox tbGLL;
		private System.Windows.Forms.TextBox tbGSA;
		private System.Windows.Forms.TextBox tbVTG;
		private System.Windows.Forms.TextBox tbRMC;
		private System.Windows.Forms.TextBox tbGSV;
		private System.Windows.Forms.Label lbGLL;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label8;
	}
}