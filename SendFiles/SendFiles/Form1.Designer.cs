namespace SendFiles
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
			this.label1 = new System.Windows.Forms.Label();
			this.btnBrowse = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.txtIP = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.txtPort = new System.Windows.Forms.TextBox();
			this.btnSend = new System.Windows.Forms.Button();
			this.lblStatus = new System.Windows.Forms.Label();
			this.progressBar1 = new System.Windows.Forms.ProgressBar();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.tbFileSelected = new System.Windows.Forms.TextBox();
			this.tbDebug = new System.Windows.Forms.TextBox();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(12, 118);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(89, 13);
			this.label1.TabIndex = 2;
			this.label1.Text = "Select the File";
			// 
			// btnBrowse
			// 
			this.btnBrowse.Location = new System.Drawing.Point(108, 113);
			this.btnBrowse.Name = "btnBrowse";
			this.btnBrowse.Size = new System.Drawing.Size(75, 23);
			this.btnBrowse.TabIndex = 0;
			this.btnBrowse.Text = "Browse";
			this.btnBrowse.UseVisualStyleBackColor = true;
			this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(18, 31);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(58, 13);
			this.label2.TabIndex = 2;
			this.label2.Text = "IP Address";
			// 
			// txtIP
			// 
			this.txtIP.Location = new System.Drawing.Point(76, 28);
			this.txtIP.Name = "txtIP";
			this.txtIP.Size = new System.Drawing.Size(131, 20);
			this.txtIP.TabIndex = 3;
			this.txtIP.Text = "192.168.42.149";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(22, 58);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(46, 13);
			this.label3.TabIndex = 4;
			this.label3.Text = "Port No.";
			// 
			// txtPort
			// 
			this.txtPort.Location = new System.Drawing.Point(76, 54);
			this.txtPort.Name = "txtPort";
			this.txtPort.Size = new System.Drawing.Size(131, 20);
			this.txtPort.TabIndex = 5;
			this.txtPort.Text = "5193";
			// 
			// btnSend
			// 
			this.btnSend.Location = new System.Drawing.Point(189, 113);
			this.btnSend.Name = "btnSend";
			this.btnSend.Size = new System.Drawing.Size(75, 23);
			this.btnSend.TabIndex = 1;
			this.btnSend.Text = "Send";
			this.btnSend.UseVisualStyleBackColor = true;
			this.btnSend.Click += new System.EventHandler(this.btnSend_Click);
			// 
			// lblStatus
			// 
			this.lblStatus.AutoSize = true;
			this.lblStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblStatus.Location = new System.Drawing.Point(25, 147);
			this.lblStatus.Name = "lblStatus";
			this.lblStatus.Size = new System.Drawing.Size(0, 13);
			this.lblStatus.TabIndex = 7;
			// 
			// progressBar1
			// 
			this.progressBar1.Location = new System.Drawing.Point(-1, 567);
			this.progressBar1.Name = "progressBar1";
			this.progressBar1.Size = new System.Drawing.Size(280, 23);
			this.progressBar1.TabIndex = 5;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.txtIP);
			this.groupBox1.Controls.Add(this.txtPort);
			this.groupBox1.Location = new System.Drawing.Point(12, 12);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(232, 90);
			this.groupBox1.TabIndex = 9;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "IP Settings";
			this.groupBox1.Enter += new System.EventHandler(this.groupBox1_Enter);
			// 
			// tbFileSelected
			// 
			this.tbFileSelected.Location = new System.Drawing.Point(12, 186);
			this.tbFileSelected.Name = "tbFileSelected";
			this.tbFileSelected.Size = new System.Drawing.Size(249, 20);
			this.tbFileSelected.TabIndex = 3;
			// 
			// tbDebug
			// 
			this.tbDebug.Location = new System.Drawing.Point(12, 222);
			this.tbDebug.Multiline = true;
			this.tbDebug.Name = "tbDebug";
			this.tbDebug.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbDebug.Size = new System.Drawing.Size(249, 326);
			this.tbDebug.TabIndex = 4;
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(281, 590);
			this.Controls.Add(this.tbDebug);
			this.Controls.Add(this.tbFileSelected);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.progressBar1);
			this.Controls.Add(this.lblStatus);
			this.Controls.Add(this.btnSend);
			this.Controls.Add(this.btnBrowse);
			this.Controls.Add(this.label1);
			this.Name = "Form1";
			this.Text = "Send Files";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnBrowse;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtIP;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtPort;
        private System.Windows.Forms.Button btnSend;
        private System.Windows.Forms.Label lblStatus;
        private System.Windows.Forms.ProgressBar progressBar1;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TextBox tbFileSelected;
		private System.Windows.Forms.TextBox tbDebug;
	}
}

