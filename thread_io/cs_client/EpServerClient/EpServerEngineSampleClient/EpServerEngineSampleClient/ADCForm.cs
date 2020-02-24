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
	public partial class ADCForm : Form
	{
		private INetworkClient m_client;
		private bool m_wait = false;
		ServerCmds svrcmd = new ServerCmds();
		private List<int> CurrentList = new List<int>();
		public System.Collections.Generic.List<GPSlist> gps_list;
		private int selected_index = 0;
		//private GPSdata gps_data;
		private byte[] recv_buff;
		private bool m_pause = false;
		//private bool m_pause = true;
		//int selected_baudrate = 0;
		int ms_delay;
		public ADCForm(string xml_file_location, INetworkClient client)
		{
			InitializeComponent();
			m_client = client;
			svrcmd.SetClient(m_client);
			gps_list = new List<GPSlist>();
			
			XmlReader xmlfile = null;
			DataSet ds = new DataSet();
			var filePath = xml_file_location;
			xmlfile = XmlReader.Create(filePath, new XmlReaderSettings());
			ds.ReadXml(xmlfile);
			GPSlist item = null;
			foreach (DataRow dr in ds.Tables[0].Rows)
			{
				item = new GPSlist();
				item.Name = dr.ItemArray[0].ToString();
				item.index = Convert.ToInt16(dr.ItemArray[1]);
				item.update_rate = Convert.ToInt16(dr.ItemArray[2]);
				item.selected = Convert.ToBoolean(dr.ItemArray[3]);
				//cbMsg.Items.Add(item.Name);
				gps_list.Add(item);
//				AddMsg(item.Name);
			}
			//cbMsg.SelectedIndex = 0;
			recv_buff = new byte[200];
			
		}
		public void Enable_Dlg(bool wait)
		{
			m_wait = wait;
			//if (wait)
				//tbAddMsg.Clear();
		}
		public void OnReceived(INetworkClient client, Packet receivedPacket)
		{
			Process_Msg(receivedPacket.PacketRaw);
		}
		delegate void AddMsg_Involk(string message);
		public void AddMsg(string message)
		{
			if (tbAddMsg.InvokeRequired)
			{
				AddMsg_Involk CI = new AddMsg_Involk(AddMsg);
				tbAddMsg.Invoke(CI, message);
			}
			else
			{
				//tbReceived.Text += message + "\r\n";
				tbAddMsg.AppendText(message + "\r\n");
			}
		}
		String StringFromByteArr(byte[] bytes)
		{
			char[] chars = new char[bytes.Length / sizeof(char)];
			System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
			return new string(chars);
		}
		public void Process_Msg(byte[] bytes)
		{
			if (m_wait == true && m_pause == false)
			{
				int type_msg = (int)bytes[0];
				System.Buffer.BlockCopy(bytes, 2, recv_buff, 0, bytes.Length - 2);
				string msg = svrcmd.GetName(type_msg);
//				AddMsg(msg);
				int i;

				switch (msg)
				{
					case "SEND_ADCS1":
						string[] words2 = StringFromByteArr(recv_buff).Split(' ');
						i = 0;
						foreach (var word in words2)
						{
							switch (i)
							{
								case 0:
									tbCh0.Text = word;
									break;
								case 1:
									tbCh1.Text = word;
									break;
								case 2:
									tbCh2.Text = word;
									break;
								case 3:
									tbCh3.Text = word;
									break;
								default:
									break;
							}
							i++;

						}
						break;

					case "SEND_ADCS2":
						words2 = msg.Split(' ');
						i = 0;
						words2 = StringFromByteArr(recv_buff).Split(' ');
						foreach (var word in words2)
						{
							switch (i)
							{
								case 0:
									tbCh4.Text = word;
									break;
								case 1:
									tbCh5.Text = word;
									break;
								case 2:
									tbCh6.Text = word;
									break;
								case 3:
									tbCh7.Text = word;
									break;
								default:
									break;
							}
							i++;
						}
						break;
				}
			}
		}

		private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
		{
			int sel = lbMsDelay.SelectedIndex;
			switch(sel)
			{
				case 0:
					ms_delay = 1000;
					break;
				case 1:
					ms_delay = 500;
					break;
				case 2:
					ms_delay = 250;
					break;
				case 3:
					ms_delay = 125;
					break;
				case 4:
					ms_delay = 75;
					break;
				case 5:
					ms_delay = 30;
					break;
				case 6:
					ms_delay = 20;
					break;
				case 7:
					ms_delay = 10;
					break;
				default:
					break;
			}
			AddMsg(ms_delay.ToString());
			byte[] update_rate = BitConverter.GetBytes(ms_delay);
			byte[] bytes = new byte[update_rate.Count() + 2];
			System.Buffer.BlockCopy(update_rate, 0, bytes, 2, update_rate.Count());
			bytes[0] = svrcmd.GetCmdIndexB("SET_ADC_RATE");
			Packet packet = new Packet(bytes, 0, bytes.Count(), false);
			//AddMsg(bytes.Count().ToString());
			if (m_client.IsConnectionAlive)
			{
				m_client.Send(packet);
			}

		}
	}
}
