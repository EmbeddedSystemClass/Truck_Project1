using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EpServerEngineSampleClient
{
	public partial class Password : Form
	{
		string m_password;
		int len;
		string m_newpassword = "";
		public Password(string password, int max_length)
		{
			InitializeComponent();
			m_password = password;
			len = 0;
			//tbReceived.Text = password.Length.ToString();
		}

		
		private void PasswordChanged_Click(object sender, EventArgs e)
		{
		}
		
		public string GetPassword()
		{
			return m_newpassword;
		}
		private void OK_Clicked(object sender, EventArgs e)
		{
			m_password = m_newpassword;
		}
		private void Cancel_Clicked(object sender, EventArgs e)
		{

		}

		private void tbNewPassword_TextChanged(object sender, EventArgs e)
		{
/*
			if(len > 3)
			{
				tbReceived.Text = "too many chars";
				tbNewPassword.Text = "";
				len = 0;
			}
			else if(len == 3)
			{
				tbReceived.Text = tbNewPassword.Text;
				m_newpassword = tbNewPassword.Text;
				len = 0;
			}
			len++;
*/
		}

		private void KeyUp_Click(object sender, KeyEventArgs e)
		{
			string test, test2;
			Int16 num;

			test = tbPassword.Text.Substring(len,1);
			test2 = m_password.Substring(len, 1);
			bool success = Int16.TryParse(test, out num);

			if (test != test2)
			{
				if (!success)
					tbReceived.Text = "number only";
				else
					tbReceived.Text = "bad password";
				len = 0;
				tbPassword.Text = "";
			}
			else
			{
				//tbReceived.Text = test + " " + test2 + " " + num.ToString();
				len++;
				if (len > 3)
				{
					len = 0;
					//tbPassword.Text = "";
					tbReceived.Text = "enter new password";
					tbNewPassword.Visible = true;
					tbNewPassword.Enabled = true;
					tbNewPassword.Focus();
					tbPassword.Enabled = false;
					label3.Visible = true;
				}
			}
		}

		private void KeyUpNewPW_Click(object sender, KeyEventArgs e)
		{
			string test;
			Int16 num;

			test = tbNewPassword.Text.Substring(len, 1);
			bool success = Int16.TryParse(test, out num);
			if (!success)
			{
				tbReceived.Text = "number only";
				len = 0;
				tbNewPassword.Text = "";
			}
			else
			{
				tbReceived.Text = num.ToString();
				m_newpassword += test;
				if(len == 3)
				{
					//tbReceived.Text = "new password changed";
					//tbReceived.Text = m_newpassword;
					m_password = m_newpassword;
					tbNewPassword.Visible = false;
					tbNewPassword.Enabled = false;
					label3.Visible = false;
					btnOK.Enabled = true;
					btnOK.Focus();
				}
				len++;
			}
		}
	}
}
