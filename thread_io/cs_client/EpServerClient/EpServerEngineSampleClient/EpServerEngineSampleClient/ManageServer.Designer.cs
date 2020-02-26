namespace EpServerEngineSampleClient
{
    partial class ManageServer
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
			this.btnRebootServer = new System.Windows.Forms.Button();
			this.btnShutdownServer = new System.Windows.Forms.Button();
			this.btnUploadNew = new System.Windows.Forms.Button();
			this.btnUploadOther = new System.Windows.Forms.Button();
			this.tbReceived = new System.Windows.Forms.TextBox();
			this.btnOK = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnUploadNewParam = new System.Windows.Forms.Button();
			this.btnShellRename = new System.Windows.Forms.Button();
			this.button1 = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// btnRebootServer
			// 
			this.btnRebootServer.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnRebootServer.Location = new System.Drawing.Point(12, 8);
			this.btnRebootServer.Name = "btnRebootServer";
			this.btnRebootServer.Size = new System.Drawing.Size(163, 36);
			this.btnRebootServer.TabIndex = 0;
			this.btnRebootServer.Text = "Reboot Server";
			this.btnRebootServer.UseVisualStyleBackColor = true;
			this.btnRebootServer.Click += new System.EventHandler(this.RebootServer_Click);
			// 
			// btnShutdownServer
			// 
			this.btnShutdownServer.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnShutdownServer.Location = new System.Drawing.Point(12, 54);
			this.btnShutdownServer.Name = "btnShutdownServer";
			this.btnShutdownServer.Size = new System.Drawing.Size(163, 35);
			this.btnShutdownServer.TabIndex = 1;
			this.btnShutdownServer.Text = "Shutdown Server";
			this.btnShutdownServer.UseVisualStyleBackColor = true;
			this.btnShutdownServer.Click += new System.EventHandler(this.ShutdownServer_Click);
			// 
			// btnUploadNew
			// 
			this.btnUploadNew.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnUploadNew.Location = new System.Drawing.Point(12, 99);
			this.btnUploadNew.Name = "btnUploadNew";
			this.btnUploadNew.Size = new System.Drawing.Size(163, 35);
			this.btnUploadNew.TabIndex = 2;
			this.btnUploadNew.Text = "Upload New";
			this.btnUploadNew.UseVisualStyleBackColor = true;
			this.btnUploadNew.Click += new System.EventHandler(this.UploadNew_Click);
			// 
			// btnUploadOther
			// 
			this.btnUploadOther.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnUploadOther.Location = new System.Drawing.Point(12, 181);
			this.btnUploadOther.Name = "btnUploadOther";
			this.btnUploadOther.Size = new System.Drawing.Size(163, 35);
			this.btnUploadOther.TabIndex = 3;
			this.btnUploadOther.Text = "Exit to Shell";
			this.btnUploadOther.UseVisualStyleBackColor = true;
			this.btnUploadOther.Click += new System.EventHandler(this.UploadOther_Click);
			// 
			// tbReceived
			// 
			this.tbReceived.Location = new System.Drawing.Point(195, 99);
			this.tbReceived.Multiline = true;
			this.tbReceived.Name = "tbReceived";
			this.tbReceived.Size = new System.Drawing.Size(163, 244);
			this.tbReceived.TabIndex = 4;
			this.tbReceived.TextChanged += new System.EventHandler(this.tbReceived_TextChanged);
			// 
			// btnOK
			// 
			this.btnOK.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnOK.Location = new System.Drawing.Point(12, 267);
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size(163, 35);
			this.btnOK.TabIndex = 5;
			this.btnOK.Text = "OK";
			this.btnOK.UseVisualStyleBackColor = true;
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnCancel.Location = new System.Drawing.Point(12, 311);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(163, 35);
			this.btnCancel.TabIndex = 6;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.UseVisualStyleBackColor = true;
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// btnUploadNewParam
			// 
			this.btnUploadNewParam.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnUploadNewParam.Location = new System.Drawing.Point(12, 140);
			this.btnUploadNewParam.Name = "btnUploadNewParam";
			this.btnUploadNewParam.Size = new System.Drawing.Size(163, 35);
			this.btnUploadNewParam.TabIndex = 7;
			this.btnUploadNewParam.Text = "New Param";
			this.btnUploadNewParam.UseVisualStyleBackColor = true;
			this.btnUploadNewParam.Click += new System.EventHandler(this.btnUploadNewParam_Click);
			// 
			// btnShellRename
			// 
			this.btnShellRename.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnShellRename.Location = new System.Drawing.Point(12, 222);
			this.btnShellRename.Name = "btnShellRename";
			this.btnShellRename.Size = new System.Drawing.Size(163, 35);
			this.btnShellRename.TabIndex = 8;
			this.btnShellRename.Text = "Shell - Rename";
			this.btnShellRename.UseVisualStyleBackColor = true;
			this.btnShellRename.Click += new System.EventHandler(this.btnShellRename_Click);
			// 
			// button1
			// 
			this.button1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.button1.Location = new System.Drawing.Point(195, 9);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(163, 35);
			this.button1.TabIndex = 9;
			this.button1.Text = "ServerUp";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// button2
			// 
			this.button2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.button2.Location = new System.Drawing.Point(196, 54);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(163, 35);
			this.button2.TabIndex = 10;
			this.button2.Text = "Server Down";
			this.button2.UseVisualStyleBackColor = true;
			this.button2.Click += new System.EventHandler(this.button2_Click);
			// 
			// ManageServer
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
			this.ClientSize = new System.Drawing.Size(371, 358);
			this.Controls.Add(this.button2);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.btnShellRename);
			this.Controls.Add(this.btnUploadNewParam);
			this.Controls.Add(this.btnCancel);
			this.Controls.Add(this.btnOK);
			this.Controls.Add(this.tbReceived);
			this.Controls.Add(this.btnUploadOther);
			this.Controls.Add(this.btnUploadNew);
			this.Controls.Add(this.btnShutdownServer);
			this.Controls.Add(this.btnRebootServer);
			this.Name = "ManageServer";
			this.Text = "ManageServer";
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnRebootServer;
        private System.Windows.Forms.Button btnShutdownServer;
        private System.Windows.Forms.Button btnUploadNew;
        private System.Windows.Forms.Button btnUploadOther;
        private System.Windows.Forms.TextBox tbReceived;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Button btnUploadNewParam;
		private System.Windows.Forms.Button btnShellRename;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Button button2;
	}
}