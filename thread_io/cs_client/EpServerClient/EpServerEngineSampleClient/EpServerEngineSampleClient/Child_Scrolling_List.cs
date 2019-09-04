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
	public partial class Child_Scrolling_List : Form
	{
		private INetworkClient m_client;
		ServerCmds svrcmd = new ServerCmds();
		private List<SListTypes> s_lists = null;
		private int list_index = 0;
		private int prev_list_index = 0;
		private bool m_wait;

		public Child_Scrolling_List(INetworkClient client)
		{
			InitializeComponent();
			m_client = client;
			svrcmd.SetClient(m_client);
		}
		public void SetXMLFile(string xml_file_location)
		{
			SListTypes item = null;
			XmlReader xmlfile = null;
			DataSet ds = new DataSet();
			string filePath = xml_file_location;
			xmlfile = XmlReader.Create(filePath, new XmlReaderSettings());
			ds.ReadXml(xmlfile);
			s_lists = new List<SListTypes>();
			foreach (DataRow dr in ds.Tables[0].Rows)
			{
				item = new SListTypes();
				item.Offset = Convert.ToInt16(dr.ItemArray[0]);
				item.Name = dr.ItemArray[1].ToString();
				item.Value = dr.ItemArray[2].ToString();
				//AddMsg(item.Label + " " + item.Command.ToString() + " " + item.Length.ToString());
				s_lists.Add(item);
				lbScroll.Items.Add(item.Name);
				item = null;
			}
			lbScroll.SetSelected(list_index, true);

		}
		delegate void AddMsg_Involk(string message);
		public void AddMsg(string message)
		{
			if (tbMsgBox.InvokeRequired)
			{
				AddMsg_Involk CI = new AddMsg_Involk(AddMsg);
				tbMsgBox.Invoke(CI, message);
			}
			else
			{
				//tbReceived.Text += message + "\r\n";
				tbMsgBox.AppendText(message + "\r\n");
			}
		}
		private void ListBoxChanged_Click(object sender, EventArgs e)
		{
			//int i = lbScroll.Items.IndexOf(lbScroll.SelectedIndex);
			for (int i = 0; i < lbScroll.Items.Count; i++)
			{
				//if (lbScroll.GetSelected(i))
					//AddMsg(i.ToString() + " " + s_lists[i].Name);
			}
		}
		public void Enable_Dlg(bool wait)
		{
			m_wait = wait;
			if (wait)
				tbMsgBox.Clear();
		}
		public void OnReceived(INetworkClient client, Packet receivedPacket)
		{
			Process_Msg(receivedPacket.PacketRaw);
		}
		public void Process_Msg(byte[] bytes)
		{
			int type_msg;
			int i, j;
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

			//			if (m_wait == true && (str == "NAV_UP" || str == "NAV_DOWN" || str == "NAV_CLICK" || str == "NAV_CLOSE" || str == "NAV_SIDE"))
			if (m_wait == true && (str == "NAV_UP" || str == "NAV_DOWN" || str == "NAV_CLICK" || str == "NAV_CLOSE"))
			{
				AddMsg(str);
				//AddMsg(ret);
				prev_list_index = list_index;
				//previous_button = current_button;
				switch (str)
				{
					case "NAV_UP":
						list_index--;
						if (list_index < 0)
							list_index = lbScroll.Items.Count-1;
						lbScroll.SetSelected(list_index, true);
						break;

					case "NAV_DOWN":
						list_index++;
						if (list_index > lbScroll.Items.Count-1)
							list_index = 0;
						lbScroll.SetSelected(list_index, true);
						break;

					case "NAV_SIDE":
						break;

					case "NAV_CLICK":
						AddMsg(lbScroll.Items[list_index].ToString() + " " + s_lists[list_index].Value.ToString());
						break;

					case "NAV_CLOSE":
						lbScroll.Items.Clear();
						this.Close();
						break;

					default:
						break;
				}

				if (str == "NAV_UP" || str == "NAV_DOWN" || str == "NAV_SIDE")
				{
				}
			}
		}

		public void Process_Msg2(string str)
		{
			if (m_wait == true && (str == "NAV_UP" || str == "NAV_DOWN" || str == "NAV_CLICK" || str == "NAV_CLOSE"))
			{
				AddMsg(str);
				//AddMsg(ret);
				prev_list_index = list_index;
				//previous_button = current_button;
				switch (str)
				{
					case "NAV_UP":
						list_index--;
						if (list_index < 0)
							list_index = lbScroll.Items.Count - 1;
						lbScroll.SetSelected(list_index, true);
						break;

					case "NAV_DOWN":
						list_index++;
						if (list_index > lbScroll.Items.Count - 1)
							list_index = 0;
						lbScroll.SetSelected(list_index, true);
						break;

					case "NAV_SIDE":
						break;

					case "NAV_CLICK":
						AddMsg(lbScroll.Items[list_index].ToString() + " " + s_lists[list_index].Value.ToString());
						break;

					case "NAV_CLOSE":
						lbScroll.Items.Clear();
						this.Close();
						break;

					default:
						break;
				}

				if (str == "NAV_UP" || str == "NAV_DOWN" || str == "NAV_SIDE")
				{
				}
			}
		}
		private void OK_Clicked(object sender, EventArgs e)
		{

		}

		private void Cancel_Clicked(object sender, EventArgs e)
		{

		}
	}
}
