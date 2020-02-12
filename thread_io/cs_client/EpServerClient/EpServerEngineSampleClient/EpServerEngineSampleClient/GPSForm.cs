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
	public partial class GPSForm : Form
	{
		private INetworkClient m_client;
		private bool m_wait = false;
		ServerCmds svrcmd = new ServerCmds();
		private List<int> CurrentList = new List<int>();
		public System.Collections.Generic.List<GPSlist> gps_list;
		private int selected_index = 0;
		private GPSdata gps_data;
		private byte[] recv_buff;
		private bool m_pause = false;

		public GPSForm(string xml_file_location, INetworkClient client)
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
				cbMsg.Items.Add(item.Name);
				gps_list.Add(item);
			}
			cbMsg.SelectedIndex = 0;
			gps_data = new GPSdata();
			recv_buff = new byte[200];
			
		}
		public void OnReceived(INetworkClient client, Packet receivedPacket)
		{
			Process_Msg(receivedPacket.PacketRaw);
		}

		private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
		{
			gps_list[selected_index].update_rate = lbUpdateRate.SelectedIndex;
		}

		private void cbMsg_SelectedIndexChanged(object sender, EventArgs e)
		{
			selected_index = cbMsg.SelectedIndex;
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
		public void Enable_Dlg(bool wait)
		{
			m_wait = wait;
			if (wait)
				tbAddMsg.Clear();
		}
		String StringFromByteArr(byte[] bytes)
		{
			char[] chars = new char[bytes.Length / sizeof(char)];
			System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
			return new string(chars);
		}

		public void Process_Msg(byte[] bytes)
		{
			if (m_wait == true)
			{
				int type_msg = (int)bytes[0];
				System.Buffer.BlockCopy(bytes, 2, recv_buff, 0, bytes.Length-2);
				string msg = svrcmd.GetName(type_msg);
				switch (msg)
				{
					case "SEND_GPS_GLL_DATA":
						AddMsg("GLL: " + StringFromByteArr(recv_buff));
						//System.Buffer.BlockCopy(recv_buff, 0, gps_data.gll, 0, 36);
						break;
					case "SEND_GPS_GGA_DATA":
						//AddMsg("GGA: " + StringFromByteArr(recv_buff));
						tbAltitude.Text = StringFromByteArr(recv_buff);
						break;
					case "SEND_GPS_GSA_DATA":
						AddMsg("GSA: " + StringFromByteArr(recv_buff));
						break;
					case "SEND_GPS_GSV_DATA":
						AddMsg("GSV: " + StringFromByteArr(recv_buff));
						break;
					case "SEND_GPS_RMC_DATA":
						
						string[] words = StringFromByteArr(recv_buff).Split(',');
						
						int i = 0;
						foreach (var word in words)
						{
							switch (i)
							{
								case 0:
									tbLatt.Text = word;
									break;
								case 1:
									tbLong.Text = word;
									break;
								case 2:
									tbSpeed.Text = word;
									break;
								case 3:
									tbDirection.Text = word;
									break;
								case 4:
									tbNWPDist.Text = word + " miles";
									break;
								case 5:
									tbNextWP.Text = word;
									break;
								case 6:
									tbLWPDist.Text = word + " miles";
									break;
								case 7:
									tbLastWP.Text = word;
									break;
								default:
									AddMsg(i.ToString());
									break;
							}

							i++;
						}
						
						if(!m_pause)						
							AddMsg(StringFromByteArr(recv_buff) + " ");
						
						break;
					case "SEND_GPS_VTG_DATA":
						AddMsg("VTG: " + StringFromByteArr(recv_buff));
						break;
					case "SEND_GPS_ZDA_DATA":
						AddMsg("ZDA: " + StringFromByteArr(recv_buff));
						break;
					default:
						break;
				}
				
				/*
				char[] chars = new char[bytes.Length / sizeof(char) + 2];
				char[] chars2 = new char[bytes.Length / sizeof(char)];
				// src srcoffset dest destoffset len
				System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
				System.Buffer.BlockCopy(bytes, 2, chars2, 0, bytes.Length - 2);
				string ret = new string(chars2);
				string str = svrcmd.GetName(type_msg);
				*/
			}
		}

		private void btnSend_Click(object sender, EventArgs e)
		{
			byte[] update_rate = BitConverter.GetBytes(gps_list[selected_index].update_rate);
			byte[] msg = new byte[2];
			AddMsg(gps_list[selected_index].update_rate.ToString() + " " + gps_list[selected_index].Name);
			switch (gps_list[selected_index].Name)
			{
				case "GGA":
					msg = BitConverter.GetBytes(0);
					break;
				case "GLL":
					msg = BitConverter.GetBytes(1);
					break;
				case "GSA":
					msg = BitConverter.GetBytes(2);
					break;
				case "GSV":
					msg = BitConverter.GetBytes(3);
					break;
				case "RMC":
					msg = BitConverter.GetBytes(4);
					break;
				case "VTG":
					msg = BitConverter.GetBytes(5);
					break;
				case "ZDA":
					msg = BitConverter.GetBytes(6);
					break;
				default:
					break;
			}
			byte[] bytes = new byte[msg.Count() + update_rate.Count() + 2];

			//System.Buffer.BlockCopy(src, src_offset, dest, dest_offset,count)
			System.Buffer.BlockCopy(msg, 0, bytes, 2, msg.Count());
			System.Buffer.BlockCopy(update_rate, 0, bytes, 4, update_rate.Count());
			bytes[0] = svrcmd.GetCmdIndexB("SET_GPS_DATA");
			Packet packet = new Packet(bytes, 0, bytes.Count(), false);
			//AddMsg(bytes.Count().ToString());
			if (m_client.IsConnectionAlive)
			{
				m_client.Send(packet);
			}
		}

		private void btnPause_Click(object sender, EventArgs e)
		{
			if(m_pause)
			{
				m_pause = false;
				btnPause.Text = "Continue";
			}
			else
			{
				m_pause = true;
				btnPause.Text = "Pause";
			}
		}

		private void button1_Click(object sender, EventArgs e)
		{
			byte[] test = ReadFile("@C:\\Users\\Daniel\\dev\\Gps\\output\\single\\gga.out");
			AddMsg(test.Length.ToString());
			//gps_data.gga.altitude.value
			//System.Buffer.BlockCopy(test, 0, gps_data.gga, 0, test.Length);
		}
		public static byte[] ReadFile(string filePath)
		{
			byte[] buffer;
			FileStream fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read);
			try
			{
				int length = (int)fileStream.Length;  // get file length
				buffer = new byte[length];            // create buffer
				int count;                            // actual number of bytes read
				int sum = 0;                          // total number of bytes read

				// read until Read method returns 0 (end of the stream has been reached)
				while ((count = fileStream.Read(buffer, sum, length - sum)) > 0)
					sum += count;  // sum is a buffer offset for next reading
			}
			finally
			{
				fileStream.Close();
			}
			return buffer;
		}

		private void btnClear_Click(object sender, EventArgs e)
		{
			tbAddMsg.Clear();
		}
	}
}
