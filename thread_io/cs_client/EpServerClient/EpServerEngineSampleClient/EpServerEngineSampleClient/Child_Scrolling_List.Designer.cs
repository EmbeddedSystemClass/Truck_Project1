namespace EpServerEngineSampleClient
{
	partial class Child_Scrolling_List
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
			this.lbScroll = new System.Windows.Forms.ListBox();
			this.tbMsgBox = new System.Windows.Forms.TextBox();
			this.tbChoice = new System.Windows.Forms.TextBox();
			this.btnOK = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// lbScroll
			// 
			this.lbScroll.Font = new System.Drawing.Font("Arial Rounded MT Bold", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbScroll.FormattingEnabled = true;
			this.lbScroll.ItemHeight = 28;
			this.lbScroll.Location = new System.Drawing.Point(17, 41);
			this.lbScroll.Name = "lbScroll";
			this.lbScroll.Size = new System.Drawing.Size(323, 368);
			this.lbScroll.TabIndex = 0;
			this.lbScroll.SelectedIndexChanged += new System.EventHandler(this.ListBoxChanged_Click);
			// 
			// tbMsgBox
			// 
			this.tbMsgBox.Location = new System.Drawing.Point(358, 12);
			this.tbMsgBox.Multiline = true;
			this.tbMsgBox.Name = "tbMsgBox";
			this.tbMsgBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbMsgBox.Size = new System.Drawing.Size(246, 341);
			this.tbMsgBox.TabIndex = 1;
			// 
			// tbChoice
			// 
			this.tbChoice.Location = new System.Drawing.Point(18, 13);
			this.tbChoice.Name = "tbChoice";
			this.tbChoice.Size = new System.Drawing.Size(323, 20);
			this.tbChoice.TabIndex = 2;
			// 
			// btnOK
			// 
			this.btnOK.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.btnOK.Font = new System.Drawing.Font("Arial Rounded MT Bold", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnOK.Location = new System.Drawing.Point(358, 370);
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size(117, 38);
			this.btnOK.TabIndex = 3;
			this.btnOK.Text = "OK";
			this.btnOK.UseVisualStyleBackColor = false;
			this.btnOK.Click += new System.EventHandler(this.OK_Clicked);
			// 
			// btnCancel
			// 
			this.btnCancel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(244)))));
			this.btnCancel.Font = new System.Drawing.Font("Arial Rounded MT Bold", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnCancel.Location = new System.Drawing.Point(487, 370);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(117, 38);
			this.btnCancel.TabIndex = 4;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.UseVisualStyleBackColor = false;
			this.btnCancel.Click += new System.EventHandler(this.Cancel_Clicked);
			// 
			// Child_Scrolling_List
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
			this.ClientSize = new System.Drawing.Size(616, 420);
			this.Controls.Add(this.btnCancel);
			this.Controls.Add(this.btnOK);
			this.Controls.Add(this.tbChoice);
			this.Controls.Add(this.tbMsgBox);
			this.Controls.Add(this.lbScroll);
			this.Name = "Child_Scrolling_List";
			this.Text = "Child_Scrolling_List";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ListBox lbScroll;
		private System.Windows.Forms.TextBox tbMsgBox;
		private System.Windows.Forms.TextBox tbChoice;
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Button btnCancel;
	}
}