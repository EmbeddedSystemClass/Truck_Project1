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
	public partial class PlayerDlg : Form
	{
		private INetworkClient m_client;
		private List<Mp3File> mp3file;
		System.Media.SoundPlayer player;
		private int current_selection;
		private int no_selections = 0;
		private bool m_wait = false;
		ServerCmds svrcmd = new ServerCmds();

		public PlayerDlg(string filePath, INetworkClient client)
		{
			InitializeComponent();
			m_client = client;
			svrcmd.SetClient(m_client);
			player = new System.Media.SoundPlayer();
			mp3file = new List<Mp3File>();
			Mp3File item = null;
			string[] fileEntries = Directory.GetFiles(filePath);
			int i = 0;
			foreach (string fileName in fileEntries)
			{
				item = new Mp3File();
				item.mp3_location = fileName;
				item.index = i++;

				if (File.Exists(item.mp3_location))
                {
                    //AddMsg(item.mp3_location + " " + item.index.ToString());
                    mp3file.Add(item);
                    string str = item.mp3_location.ToString();
                    int index = str.LastIndexOf("\\");
                    string str2 = str.Substring(index+1);
                    lbPlayList.Items.Add(str2);
                    item = null;
                    no_selections++;
                }
			}
			current_selection = 0;
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
		private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
		{
			current_selection = lbPlayList.SelectedIndex;
		}

		private void btnPlay_Click(object sender, EventArgs e)
		{
			current_selection = lbPlayList.SelectedIndex;
			string song = mp3file[current_selection].mp3_location;
            //AddMsg(song.ToString());               
			player.SoundLocation = song;
			player.Play();
		}

		private void Next_Click(object sender, EventArgs e)
		{
			current_selection++;
			if (current_selection > no_selections - 1)
				current_selection = 0;
			lbPlayList.SelectedIndex = current_selection;
			//AddMsg(current_selection.ToString() + ": " + mp3file[current_selection].mp3_location.ToString());
		}

		private void Prev_Click(object sender, EventArgs e)
		{
			current_selection--;
			if (current_selection < 0)
				current_selection = no_selections - 1;
			lbPlayList.SelectedIndex = current_selection;
			//AddMsg(current_selection.ToString() + ": " + mp3file[current_selection].mp3_location.ToString());
		}
		public void Enable_Dlg(bool wait)
		{
			m_wait = wait;
			if (wait)
				tbAddMsg.Clear();
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

			if (m_wait == true && (str == "NAV_UP" || str == "NAV_DOWN" || str == "NAV_CLICK" || str == "NAV_CLOSE" || str == "NAV_SIDE"))
			{
				AddMsg(str);
				AddMsg(ret);
				switch (str)
				{
					case "NAV_UP":
						Prev_Click(new object(), new EventArgs());
						break;
					case "NAV_DOWN":
						Next_Click(new object(), new EventArgs());
						break;
					case "NAV_SIDE":
						current_selection += 10;
						if (current_selection > no_selections - 1)
							current_selection = 0;
						lbPlayList.SelectedIndex = current_selection;
						break;
					case "NAV_CLICK":
						btnPlay_Click(new object(), new EventArgs());
						break;
					case "NAV_CLOSE":
						//this.Dispose();
						this.Close();
						break;
					default:
						break;
				}
			}
		}

        private void Loop_Clicked(object sender, EventArgs e)
        {
            current_selection = lbPlayList.SelectedIndex;
            string song = mp3file[current_selection].mp3_location;
            AddMsg(song.ToString());
            player.SoundLocation = song;
            player.PlayLooping();       // plays the same damn file over and over again
        }

        private void Test_Click(object sender, EventArgs e)
        {
            //player.
            //AddMsg(current_selection.ToString());
        }
    }
}
