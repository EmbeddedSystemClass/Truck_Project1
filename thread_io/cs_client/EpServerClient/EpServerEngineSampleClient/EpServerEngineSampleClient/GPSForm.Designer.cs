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
			this.tbLatt = new System.Windows.Forms.TextBox();
			this.tbLong = new System.Windows.Forms.TextBox();
			this.tbNextWP = new System.Windows.Forms.TextBox();
			this.tbLastWP = new System.Windows.Forms.TextBox();
			this.tbNWPDist = new System.Windows.Forms.TextBox();
			this.tbLWPDist = new System.Windows.Forms.TextBox();
			this.tbSpeed = new System.Windows.Forms.TextBox();
			this.lat_label = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.btnClear = new System.Windows.Forms.Button();
			this.label4 = new System.Windows.Forms.Label();
			this.tbAltitude = new System.Windows.Forms.TextBox();
			this.label9 = new System.Windows.Forms.Label();
			this.tbDirection = new System.Windows.Forms.TextBox();
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
			this.tbAddMsg.Location = new System.Drawing.Point(8, 434);
			this.tbAddMsg.Multiline = true;
			this.tbAddMsg.Name = "tbAddMsg";
			this.tbAddMsg.ReadOnly = true;
			this.tbAddMsg.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbAddMsg.Size = new System.Drawing.Size(604, 89);
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
			this.lbCurrentList.Size = new System.Drawing.Size(120, 95);
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
			this.btnPause.Location = new System.Drawing.Point(178, 534);
			this.btnPause.Name = "btnPause";
			this.btnPause.Size = new System.Drawing.Size(75, 23);
			this.btnPause.TabIndex = 16;
			this.btnPause.Text = "Pause Output";
			this.btnPause.UseVisualStyleBackColor = true;
			this.btnPause.Click += new System.EventHandler(this.btnPause_Click);
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(8, 529);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(114, 32);
			this.button1.TabIndex = 17;
			this.button1.Text = "button1";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Visible = false;
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
			this.groupBox1.Location = new System.Drawing.Point(12, 12);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(352, 195);
			this.groupBox1.TabIndex = 18;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Send Data";
			// 
			// tbLatt
			// 
			this.tbLatt.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbLatt.Location = new System.Drawing.Point(111, 238);
			this.tbLatt.Name = "tbLatt";
			this.tbLatt.Size = new System.Drawing.Size(100, 31);
			this.tbLatt.TabIndex = 19;
			// 
			// tbLong
			// 
			this.tbLong.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbLong.Location = new System.Drawing.Point(111, 288);
			this.tbLong.Name = "tbLong";
			this.tbLong.Size = new System.Drawing.Size(100, 29);
			this.tbLong.TabIndex = 20;
			// 
			// tbNextWP
			// 
			this.tbNextWP.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbNextWP.Location = new System.Drawing.Point(111, 338);
			this.tbNextWP.Name = "tbNextWP";
			this.tbNextWP.Size = new System.Drawing.Size(180, 29);
			this.tbNextWP.TabIndex = 21;
			// 
			// tbLastWP
			// 
			this.tbLastWP.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbLastWP.Location = new System.Drawing.Point(111, 388);
			this.tbLastWP.Name = "tbLastWP";
			this.tbLastWP.Size = new System.Drawing.Size(180, 29);
			this.tbLastWP.TabIndex = 22;
			// 
			// tbNWPDist
			// 
			this.tbNWPDist.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbNWPDist.Location = new System.Drawing.Point(317, 338);
			this.tbNWPDist.Name = "tbNWPDist";
			this.tbNWPDist.Size = new System.Drawing.Size(111, 29);
			this.tbNWPDist.TabIndex = 23;
			// 
			// tbLWPDist
			// 
			this.tbLWPDist.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbLWPDist.Location = new System.Drawing.Point(317, 387);
			this.tbLWPDist.Name = "tbLWPDist";
			this.tbLWPDist.Size = new System.Drawing.Size(111, 29);
			this.tbLWPDist.TabIndex = 24;
			// 
			// tbSpeed
			// 
			this.tbSpeed.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbSpeed.Location = new System.Drawing.Point(313, 238);
			this.tbSpeed.Name = "tbSpeed";
			this.tbSpeed.Size = new System.Drawing.Size(100, 31);
			this.tbSpeed.TabIndex = 25;
			// 
			// lat_label
			// 
			this.lat_label.AutoSize = true;
			this.lat_label.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lat_label.Location = new System.Drawing.Point(17, 245);
			this.lat_label.Name = "lat_label";
			this.lat_label.Size = new System.Drawing.Size(42, 24);
			this.lat_label.TabIndex = 26;
			this.lat_label.Text = "Latt";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label5.Location = new System.Drawing.Point(16, 292);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(57, 24);
			this.label5.TabIndex = 27;
			this.label5.Text = "Long";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label6.Location = new System.Drawing.Point(13, 342);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(91, 24);
			this.label6.TabIndex = 28;
			this.label6.Text = "Next WP";
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label7.Location = new System.Drawing.Point(16, 390);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(85, 24);
			this.label7.TabIndex = 29;
			this.label7.Text = "Last WP";
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label8.Location = new System.Drawing.Point(236, 242);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(71, 24);
			this.label8.TabIndex = 30;
			this.label8.Text = "Speed";
			// 
			// btnClear
			// 
			this.btnClear.Location = new System.Drawing.Point(267, 534);
			this.btnClear.Name = "btnClear";
			this.btnClear.Size = new System.Drawing.Size(75, 23);
			this.btnClear.TabIndex = 31;
			this.btnClear.Text = "Clear";
			this.btnClear.UseVisualStyleBackColor = true;
			this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label4.Location = new System.Drawing.Point(236, 290);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(40, 24);
			this.label4.TabIndex = 32;
			this.label4.Text = "Alt.";
			// 
			// tbAltitude
			// 
			this.tbAltitude.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbAltitude.Location = new System.Drawing.Point(313, 286);
			this.tbAltitude.Name = "tbAltitude";
			this.tbAltitude.Size = new System.Drawing.Size(100, 31);
			this.tbAltitude.TabIndex = 33;
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label9.Location = new System.Drawing.Point(443, 290);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(42, 24);
			this.label9.TabIndex = 34;
			this.label9.Text = "Dir.";
			// 
			// tbDirection
			// 
			this.tbDirection.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbDirection.Location = new System.Drawing.Point(491, 286);
			this.tbDirection.Name = "tbDirection";
			this.tbDirection.Size = new System.Drawing.Size(83, 31);
			this.tbDirection.TabIndex = 35;
			// 
			// GPSForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
			this.ClientSize = new System.Drawing.Size(638, 573);
			this.Controls.Add(this.tbDirection);
			this.Controls.Add(this.label9);
			this.Controls.Add(this.tbAltitude);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.btnClear);
			this.Controls.Add(this.label8);
			this.Controls.Add(this.label7);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.lat_label);
			this.Controls.Add(this.tbSpeed);
			this.Controls.Add(this.tbLWPDist);
			this.Controls.Add(this.tbNWPDist);
			this.Controls.Add(this.tbLastWP);
			this.Controls.Add(this.tbNextWP);
			this.Controls.Add(this.tbLong);
			this.Controls.Add(this.tbLatt);
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
		private System.Windows.Forms.TextBox tbLatt;
		private System.Windows.Forms.TextBox tbLong;
		private System.Windows.Forms.TextBox tbNextWP;
		private System.Windows.Forms.TextBox tbLastWP;
		private System.Windows.Forms.TextBox tbNWPDist;
		private System.Windows.Forms.TextBox tbLWPDist;
		private System.Windows.Forms.TextBox tbSpeed;
		private System.Windows.Forms.Label lat_label;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Button btnClear;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox tbAltitude;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.TextBox tbDirection;
	}
}