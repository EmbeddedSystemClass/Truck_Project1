namespace EpServerEngineSampleClient
{
	partial class PlayerDlg
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
			this.lbPlayList = new System.Windows.Forms.ListBox();
			this.btnPlay = new System.Windows.Forms.Button();
			this.tbAddMsg = new System.Windows.Forms.TextBox();
			this.btnNext = new System.Windows.Forms.Button();
			this.btnPrev = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// lbPlayList
			// 
			this.lbPlayList.Font = new System.Drawing.Font("Arial Rounded MT Bold", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbPlayList.FormattingEnabled = true;
			this.lbPlayList.ItemHeight = 28;
			this.lbPlayList.Location = new System.Drawing.Point(27, 12);
			this.lbPlayList.Name = "lbPlayList";
			this.lbPlayList.Size = new System.Drawing.Size(888, 396);
			this.lbPlayList.TabIndex = 0;
			this.lbPlayList.SelectedIndexChanged += new System.EventHandler(this.listBox1_SelectedIndexChanged);
			// 
			// btnPlay
			// 
			this.btnPlay.Font = new System.Drawing.Font("Arial Rounded MT Bold", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnPlay.Location = new System.Drawing.Point(812, 539);
			this.btnPlay.Name = "btnPlay";
			this.btnPlay.Size = new System.Drawing.Size(104, 38);
			this.btnPlay.TabIndex = 1;
			this.btnPlay.Text = "Play";
			this.btnPlay.UseVisualStyleBackColor = true;
			this.btnPlay.Click += new System.EventHandler(this.btnPlay_Click);
			// 
			// tbAddMsg
			// 
			this.tbAddMsg.Location = new System.Drawing.Point(27, 429);
			this.tbAddMsg.Multiline = true;
			this.tbAddMsg.Name = "tbAddMsg";
			this.tbAddMsg.Size = new System.Drawing.Size(888, 92);
			this.tbAddMsg.TabIndex = 2;
			// 
			// btnNext
			// 
			this.btnNext.Font = new System.Drawing.Font("Arial Rounded MT Bold", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnNext.Location = new System.Drawing.Point(590, 539);
			this.btnNext.Name = "btnNext";
			this.btnNext.Size = new System.Drawing.Size(104, 38);
			this.btnNext.TabIndex = 3;
			this.btnNext.Text = "Next";
			this.btnNext.UseVisualStyleBackColor = true;
			this.btnNext.Click += new System.EventHandler(this.Next_Click);
			// 
			// btnPrev
			// 
			this.btnPrev.Font = new System.Drawing.Font("Arial Rounded MT Bold", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnPrev.Location = new System.Drawing.Point(701, 539);
			this.btnPrev.Name = "btnPrev";
			this.btnPrev.Size = new System.Drawing.Size(104, 38);
			this.btnPrev.TabIndex = 4;
			this.btnPrev.Text = "Prev";
			this.btnPrev.UseVisualStyleBackColor = true;
			this.btnPrev.Click += new System.EventHandler(this.Prev_Click);
			// 
			// PlayerDlg
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
			this.ClientSize = new System.Drawing.Size(944, 589);
			this.Controls.Add(this.btnPrev);
			this.Controls.Add(this.btnNext);
			this.Controls.Add(this.tbAddMsg);
			this.Controls.Add(this.btnPlay);
			this.Controls.Add(this.lbPlayList);
			this.Name = "PlayerDlg";
			this.Text = "PlayerDlg";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ListBox lbPlayList;
		private System.Windows.Forms.Button btnPlay;
		private System.Windows.Forms.TextBox tbAddMsg;
		private System.Windows.Forms.Button btnNext;
		private System.Windows.Forms.Button btnPrev;
	}
}