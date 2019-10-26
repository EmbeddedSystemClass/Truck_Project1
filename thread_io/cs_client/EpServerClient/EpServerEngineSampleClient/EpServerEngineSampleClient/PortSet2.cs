using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Threading.Tasks;
using System.Data.SqlClient;
using System.Linq;
using System.Xml;
using System.Text;
using System.Windows.Forms;
using EpServerEngine.cs;
using System.Diagnostics;
using System.Globalization;
using EpLibrary.cs;
using System.IO;
using System.Xml.Serialization;
using System.Drawing;
using System.Timers;
using static EpServerEngineSampleClient.FrmSampleClient;

namespace EpServerEngineSampleClient
{
	public partial class PortSet2 : Form
	{
		private INetworkClient m_client;
		//private INetworkClient m_client2;
		ServerCmds svrcmd = new ServerCmds();
		private int current_button = 0;
		private int previous_button = 0;
		public System.Collections.Generic.List<CommonControls> m_ctls;
		private bool m_wait = false;
		private List<UIFormat> ui_format;
		private List<ChildDialogs> child_dialogs;
		private string xml_child_dialogs_location = "c:\\users\\daniel\\dev\\ChildDialog.xml";
		private Child_Scrolling_List slist = null;
        string slist_cmd = "";
        int slist_value = 0;

		public PortSet2(string xml_file_location, INetworkClient client)
		{
			InitializeComponent();
			ui_format = new List<UIFormat>();
			UIFormat item = null;
			XmlReader xmlfile = null;
			DataSet ds = new DataSet();
			var filePath = xml_file_location;
			xmlfile = XmlReader.Create(filePath, new XmlReaderSettings());
			ds.ReadXml(xmlfile);
			// load the ui_format for this dialog from xml_file_location
			foreach(DataRow dr in ds.Tables[0].Rows)
			{
				item = new UIFormat();
				item.Label = dr.ItemArray[0].ToString();
				item.Command = Convert.ToInt16(dr.ItemArray[1]);
				item.Length = Convert.ToInt16(dr.ItemArray[2]);
				//AddMsg(item.Label + " " + item.Command.ToString() + " " + item.Length.ToString());
				ui_format.Add(item);
				item = null;
			}
			child_dialogs = new List<ChildDialogs>();
			ChildDialogs item2 = null;
			filePath = xml_child_dialogs_location;
			xmlfile = XmlReader.Create(filePath, new XmlReaderSettings());
			ds.Reset();
			ds.ReadXml(xmlfile);
			// load the list of child dialogs
			foreach (DataRow dr in ds.Tables[0].Rows)
			{
				item2 = new ChildDialogs();
				item2.Type = Convert.ToInt16(dr.ItemArray[0]);
				item2.Num = Convert.ToInt16(dr.ItemArray[1]);
				item2.Name = dr.ItemArray[2].ToString();
				item2.Name += ".xml";
				//AddMsg(item2.Num.ToString() + " " + item2.Name);
				child_dialogs.Add(item2);
				item2 = null;
			}
			m_client = client;
			svrcmd.SetClient(m_client);
            //slist = new Child_Scrolling_List(client);

			int i = 0;
			m_ctls = new List<CommonControls>();
			foreach (Button btn in this.Controls.OfType<Button>())
			{
				m_ctls.Add(new CommonControls()
				{
					CtlName = btn.Name,
					CtlText = ui_format[i].Label,
					TabOrder = btn.TabIndex,
					Ctlinst = btn,
					cmd = ui_format[i].Command,
					len = ui_format[i].Length,
					offset = 0
				});
				i++;
			}
			for(i = 0;i < m_ctls.Count();i++)
			{
				if (m_ctls[i].cmd == 0)
					m_ctls[9-i].Ctlinst.Enabled = false;
			}
			//slist = new Child_Scrolling_List(m_client);
			//slist.Enable_Dlg(false);
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
        public void OnReceived(INetworkClient client, Packet receivedPacket)
        {
			Process_Msg(receivedPacket.PacketRaw);
        }
		public void Enable_Dlg(bool wait)
		{
			m_wait = wait;
			//if (wait)
				//tbReceived.Clear();
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
				//tbReceived.Text += message + "\r\n";
				tbReceived.AppendText(message + "\r\n");
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
		private bool m_keypad;
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

			if (m_wait == true)
			{
                if(str == "SEND_STATUS")
                {
                    AddMsg(ret);

                } else if (str == "NAV_UP" || str == "NAV_DOWN" || str == "NAV_CLICK" || str == "NAV_CLOSE" || str == "NAV_SIDE")
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
							if (current_button > 4)
								current_button -= 5;
							else current_button += 5;
							break;
						case "NAV_CLICK":
							i = GetInstByTabOrder(current_button);
							if(m_ctls[i].CtlText != "Close")
							{
								//AddMsg(m_ctls[9 - i].CtlText);
								Button temp = (Button)(m_ctls[i].Ctlinst);
								m_keypad = true;
								temp.PerformClick();
								m_keypad = false;
							}

							break;
						case "NAV_CLOSE":
							//this.Dispose();
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

				/*
				else if(str == "HOME_SVR_ON" && !m_client.IsConnectionAlive)
				{
					ClientOps ops = new ClientOps((FrmSampleClient)this.Parent, "192.168.42.150", "8000");
					m_client2.Connect(ops);
				}
				else if (str == "HOME_SVR_OFF" && m_client.IsConnectionAlive)
				{
					m_client2.Disconnect();
				}
				*/

			}
			else if(!slist.IsDisposed)// then the slist dlg is visible...
			{
				slist.Process_Msg2(str);
			}
        }
		private void button0_Click(object sender, EventArgs e)
		{
			if (!m_keypad)
				current_button = 0;
			send_cmd();
		}
		private void button1_Click(object sender, EventArgs e)
		{
			if (!m_keypad)
				current_button = 1;
			send_cmd();
		}
		private void button2_Click(object sender, EventArgs e)
		{
			if (!m_keypad)
				current_button = 2;
			send_cmd();
		}
		private void button3_Click(object sender, EventArgs e)
		{
			if (!m_keypad)
				current_button = 3;
			send_cmd();
		}
		private void button4_Click(object sender, EventArgs e)
		{
			if (!m_keypad)
				current_button = 4;
			send_cmd();
		}
		private void button5_Click(object sender, EventArgs e)
		{
			if (!m_keypad)
				current_button = 5;
			send_cmd();
		}
		private void button6_Click(object sender, EventArgs e)
		{
			if (!m_keypad)
				current_button = 6;
			send_cmd();
		}
		private void button7_Click(object sender, EventArgs e)
		{
			if (!m_keypad)
				current_button = 7;
			send_cmd();
		}
		private void button8_Click(object sender, EventArgs e)
		{
			if (!m_keypad)
				current_button = 8;
			send_cmd();
		}
		private void button9_Click(object sender, EventArgs e)
		{
			if (!m_keypad)
				current_button = 9;
			send_cmd();
		}
		private void send_cmd()
		{
			int command = m_ctls[current_button].cmd;
			int offset = m_ctls[current_button].offset;
			int len = m_ctls[current_button].len;
			// if the command in the xml file is > 200 then create and
			// execute a child dialog according to child dialogs enum
			if (command > 199 && command < 256)
			{
				if (command > child_dialogs.Count() + 200)
				{
					AddMsg("cd cmd out of range");
				}
				else
				{
                    slist = new Child_Scrolling_List(m_client);
                    slist.Enabled = true;

                    int index = command - 200;
                    byte indexb = (byte)index;
					AddMsg("special cmd: " + command.ToString());
					AddMsg(child_dialogs[index].Name + " " + child_dialogs[index].Num.ToString());
                    
					if (child_dialogs[index].Type == 0) // Scrolling_List type dialog
					{
						slist.SetXMLFile(child_dialogs[index].Name);
                        //m_wait = false;
                        slist.Enable_Dlg(true);
						this.Enable_Dlg(false);
						slist.ShowDialog(this);
                        slist_value = slist.final_value;
                        slist_cmd = slist.cmd;
                        AddMsg("test: " + slist_cmd + " " + slist_value.ToString());
                        slist.Enable_Dlg(false);
                        this.Enable_Dlg(true);
                        slist.Dispose();
                        if (slist_value != 0)
                        {
                            string str = slist_cmd;
                            byte[] bytes1 = BitConverter.GetBytes(slist_value);
                            byte[] bytes = new byte[bytes1.Count() + 2];
                            bytes[0] = svrcmd.GetCmdIndexB(str);
                            System.Buffer.BlockCopy(bytes1, 0, bytes, 2, bytes1.Count());
                            Packet packet = new Packet(bytes, 0, bytes.Count(), false);
                            m_client.Send(packet);
                        }
                    }
                }
			}
			else
			{
				if (offset > 101 && offset < 103)
				{
					AddMsg("svr cmd: " + offset.ToString());
				}
				string cmd = svrcmd.GetName(command + offset);
				//AddMsg(cmd + " " + current_button.ToString() + " " + command.ToString() + " " + offset.ToString());
				svrcmd.Send_Cmd(command + offset);
				if (++offset >= len)
					m_ctls[current_button].offset = 0;
				else m_ctls[current_button].offset = offset;
			}
		}
	}
}