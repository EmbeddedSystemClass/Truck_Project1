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
            this.btnUploadOther.Location = new System.Drawing.Point(12, 144);
            this.btnUploadOther.Name = "btnUploadOther";
            this.btnUploadOther.Size = new System.Drawing.Size(163, 35);
            this.btnUploadOther.TabIndex = 3;
            this.btnUploadOther.Text = "Upload Other";
            this.btnUploadOther.UseVisualStyleBackColor = true;
            this.btnUploadOther.Click += new System.EventHandler(this.UploadOther_Click);
            // 
            // tbReceived
            // 
            this.tbReceived.Location = new System.Drawing.Point(195, 8);
            this.tbReceived.Multiline = true;
            this.tbReceived.Name = "tbReceived";
            this.tbReceived.Size = new System.Drawing.Size(254, 256);
            this.tbReceived.TabIndex = 4;
            this.tbReceived.TextChanged += new System.EventHandler(this.tbReceived_TextChanged);
            // 
            // btnOK
            // 
            this.btnOK.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnOK.Location = new System.Drawing.Point(12, 186);
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
            this.btnCancel.Location = new System.Drawing.Point(12, 229);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(163, 35);
            this.btnCancel.TabIndex = 6;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // ManageServer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.ClientSize = new System.Drawing.Size(461, 279);
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
    }
}