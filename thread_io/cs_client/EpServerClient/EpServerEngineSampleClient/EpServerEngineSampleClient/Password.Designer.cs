namespace EpServerEngineSampleClient
{
	partial class Password
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
			this.tbPassword = new System.Windows.Forms.TextBox();
			this.btnOK = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.tbNewPassword = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.tbReceived = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(12, 9);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(204, 29);
			this.label1.TabIndex = 0;
			this.label1.Text = "Enter Password:";
			// 
			// tbPassword
			// 
			this.tbPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbPassword.Location = new System.Drawing.Point(257, 9);
			this.tbPassword.MaxLength = 8;
			this.tbPassword.Name = "tbPassword";
			this.tbPassword.PasswordChar = '*';
			this.tbPassword.Size = new System.Drawing.Size(187, 35);
			this.tbPassword.TabIndex = 3;
			this.tbPassword.TextChanged += new System.EventHandler(this.PasswordChanged_Click);
			this.tbPassword.KeyUp += new System.Windows.Forms.KeyEventHandler(this.KeyUp_Click);
			// 
			// btnOK
			// 
			this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.btnOK.Enabled = false;
			this.btnOK.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnOK.Location = new System.Drawing.Point(260, 99);
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size(75, 42);
			this.btnOK.TabIndex = 4;
			this.btnOK.Text = "OK";
			this.btnOK.UseVisualStyleBackColor = true;
			this.btnOK.Click += new System.EventHandler(this.OK_Clicked);
			// 
			// button2
			// 
			this.button2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.button2.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.button2.Location = new System.Drawing.Point(344, 99);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(103, 42);
			this.button2.TabIndex = 5;
			this.button2.Text = "Cancel";
			this.button2.UseVisualStyleBackColor = true;
			this.button2.Click += new System.EventHandler(this.Cancel_Clicked);
			// 
			// tbNewPassword
			// 
			this.tbNewPassword.Enabled = false;
			this.tbNewPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbNewPassword.Location = new System.Drawing.Point(257, 55);
			this.tbNewPassword.MaxLength = 8;
			this.tbNewPassword.Name = "tbNewPassword";
			this.tbNewPassword.PasswordChar = '*';
			this.tbNewPassword.Size = new System.Drawing.Size(187, 35);
			this.tbNewPassword.TabIndex = 7;
			this.tbNewPassword.Visible = false;
			this.tbNewPassword.TextChanged += new System.EventHandler(this.tbNewPassword_TextChanged);
			this.tbNewPassword.KeyUp += new System.Windows.Forms.KeyEventHandler(this.KeyUpNewPW_Click);
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label3.Location = new System.Drawing.Point(12, 58);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(195, 29);
			this.label3.TabIndex = 8;
			this.label3.Text = "New Password:";
			this.label3.Visible = false;
			// 
			// tbReceived
			// 
			this.tbReceived.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbReceived.ForeColor = System.Drawing.Color.Red;
			this.tbReceived.Location = new System.Drawing.Point(12, 106);
			this.tbReceived.Name = "tbReceived";
			this.tbReceived.Size = new System.Drawing.Size(237, 31);
			this.tbReceived.TabIndex = 9;
			// 
			// Password
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
			this.ClientSize = new System.Drawing.Size(456, 156);
			this.Controls.Add(this.tbReceived);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.tbNewPassword);
			this.Controls.Add(this.button2);
			this.Controls.Add(this.btnOK);
			this.Controls.Add(this.tbPassword);
			this.Controls.Add(this.label1);
			this.Name = "Password";
			this.Text = "Password";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox tbPassword;
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.TextBox tbNewPassword;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox tbReceived;
	}
}