using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EpLibrary.cs;
using EpServerEngine.cs;
using static EpServerEngineSampleClient.FrmSampleClient;

namespace EpServerEngineSampleClient
{
	public partial class PortSet2 : Form
	{
		private INetworkClient m_client;
		ServerCmds svrcmd = new ServerCmds();
		private int current_button = 0;
		private int previous_button = 0;
		public System.Collections.Generic.List<CommonControls> m_ctls;
		bool m_wait = false;

		public void SetList(System.Collections.Generic.List<CommonControls> ctls)
		{
			m_ctls = ctls;
		}
		public PortSet2()
		{
			InitializeComponent();
		}
		public void SetButtonLabels()
		{ 
			this.button0.Text = m_ctls[0].CtlText;
			this.button1.Text = m_ctls[1].CtlText;
			this.button2.Text = m_ctls[2].CtlText;
			this.button3.Text = m_ctls[3].CtlText;
			this.button4.Text = m_ctls[4].CtlText;
			this.button5.Text = m_ctls[5].CtlText;
			this.button6.Text = m_ctls[6].CtlText;
			this.button7.Text = m_ctls[7].CtlText;
			this.button8.Text = m_ctls[8].CtlText;
			this.button9.Text = m_ctls[9].CtlText;
		}
		public void SetClient(INetworkClient client)
        {
            m_client = client;
            svrcmd.SetClient(m_client);
            //            AddMsg(m_ctls.ToString());
        }
        public void OnReceived(INetworkClient client, Packet receivedPacket)
        {
            Process_Msg(receivedPacket.PacketRaw);
        }
		public void Enable_Dlg(bool wait)
		{
			m_wait = wait;
			if (wait)
				tbReceived.Clear();
		}
        delegate void AddMsg_Involk(string message);
        public void AddMsg(string message)
        {
            if (tbReceived.InvokeRequired)
            {
                AddMsg_Involk CI = new AddMsg_Involk(AddMsg);
                tbReceived.Invoke(CI, message);
            }
            else
            {
                tbReceived.Text += message + "\r\n";
            }
        }

		private int GetTabOrder(int i)
		{
			return m_ctls[i].TabOrder;
		}
		private string GetName(int i)
		{
			return m_ctls[i].CtlText;
		}
		private int GetInstByTabOrder(int tab)
		{
			int i;
			for(i = 0;i < m_ctls.Count();i++)
			{
				if (tab == m_ctls[i].TabOrder)
				{
					return i;
				}
			}
			return -1;
		}
			
        public void Process_Msg(byte[] bytes)
        {
            int type_msg;
			int i,j;
            string ret = null;
            char[] chars = new char[bytes.Length / sizeof(char) + 2];
            char[] chars2 = new char[bytes.Length / sizeof(char)];
            // src srcoffset dest destoffset len
            System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
            type_msg = (int)chars[0];
            System.Buffer.BlockCopy(bytes, 2, chars2, 0, bytes.Length - 2);
            ret = new string(chars2);
			string str = svrcmd.GetName(type_msg);
			//AddMsg(str);

			if (m_wait == true && (str == "NAV_UP" || str == "NAV_DOWN" || str == "NAV_CLICK" || str == "NAV_CLOSE"))
			{
				previous_button = current_button;
				switch (str)
				{
					case "NAV_UP":
						current_button--;
						if (current_button < 0)
							current_button = m_ctls.Count() - 1;
						break;
					case "NAV_DOWN":
						current_button++;
						if (current_button > m_ctls.Count() - 1)
							current_button = 0;
						break;
					case "NAV_SIDE":
						if (current_button > 6)
							current_button -= 7;
						else current_button += 7;
						break;
					case "NAV_CLICK":
						i = GetInstByTabOrder(current_button);
						//if(m_ctls[i].CtlText != "Close")
						{
							//AddMsg(i.ToString());
							//AddMsg(m_ctls[i].CtlText);
							Button temp = (Button)(m_ctls[i].Ctlinst);
							temp.PerformClick();
						}

						break;
					case "NAV_CLOSE":
						//AddMsg("close");
						this.Close();
						break;
					default:
						break;
				}

				if (str == "NAV_UP" || str == "NAV_DOWN" || str == "NAV_SIDE")
				{
					i = GetInstByTabOrder(current_button);
					j = GetInstByTabOrder(previous_button);
					if (i > -1 && j > -1)
					{
						//AddMsg(m_ctls[i].TabOrder.ToString());
						Button temp = (Button)m_ctls[i].Ctlinst;
						temp.BackColor = Color.Aqua;
						temp = (Button)m_ctls[j].Ctlinst;
						temp.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
						//						AddMsg(current_button.ToString() + " " + i.ToString());
					}
					else
					{
						AddMsg("bad tab order" + " " + current_button.ToString() + i.ToString());
					}
				}
			}
        }
		private void button0_Click(object sender, EventArgs e)
		{
			send_cmd();
		}
		private void button1_Click(object sender, EventArgs e)
		{
			send_cmd();
		}
		private void button2_Click(object sender, EventArgs e)
		{
			send_cmd();
		}
		private void button3_Click(object sender, EventArgs e)
		{
			send_cmd();
		}
		private void button4_Click(object sender, EventArgs e)
		{
			send_cmd();
		}
		private void button5_Click(object sender, EventArgs e)
		{
			send_cmd();
		}
		private void button6_Click(object sender, EventArgs e)
		{
			send_cmd();
		}
		private void button7_Click(object sender, EventArgs e)
		{
			send_cmd();
		}
		private void button8_Click(object sender, EventArgs e)
		{
			send_cmd();
		}
		private void button9_Click(object sender, EventArgs e)
		{
			send_cmd();
		}
		private void send_cmd()
		{
			int command = m_ctls[current_button].cmd;
			int offset = m_ctls[current_button].offset;
			int len = m_ctls[current_button].len;

			string cmd = svrcmd.GetName(command + offset);

//			AddMsg(cmd + " " + offset.ToString());
			//AddMsg(cmd + " " + current_button.ToString() + " " + command.ToString() + " " + offset.ToString());

			svrcmd.Send_Cmd(command + offset);

			if (++offset >= len)
				m_ctls[current_button].offset = 0;
			else m_ctls[current_button].offset = offset;
		}

	}
}
