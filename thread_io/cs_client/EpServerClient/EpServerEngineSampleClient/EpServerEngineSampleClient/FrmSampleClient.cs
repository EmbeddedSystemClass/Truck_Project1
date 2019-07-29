using System;
using System.Collections.Generic;
using System.Data;
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

namespace EpServerEngineSampleClient
{
	public partial class FrmSampleClient : Form, INetworkClientCallback
	{
		public class CommonControls : IEquatable<CommonControls>
		{
			public string CtlName { get; set; }     // the name of the button control (should be button0->9)
			public string CtlText { get; set; }     // the text on the button (set programmatically)
			public int TabOrder { get; set; }       // tab order
			public Control Ctlinst { get; set; }    // instance of the control (makes the above 3 redundant)
			public int cmd { get; set; }            // the index into enum command_types
			public int len { get; set; }            // total length of included commands including 1st one (as a set)
			public int offset { get; set; }

			public override string ToString()
			{
				return CtlName + " " + CtlText + " " + TabOrder.ToString();
			}
			public override bool Equals(object obj)
			{
				if (obj == null) return false;
				CommonControls objAsPart = obj as CommonControls;
				if (objAsPart == null) return false;
				else return Equals(objAsPart);
			}
			public override int GetHashCode()
			{
				return TabOrder;
			}
			public bool Equals(CommonControls other)
			{
				if (other == null) return false;
				return (this.TabOrder.Equals(other.TabOrder));
			}

		}
		ConfigParams cfg_params = new ConfigParams();
		ServerCmds svrcmd = new ServerCmds();
		INetworkClient m_client = new IocpTcpClient();
		public System.Collections.Generic.List<CommonControls> ctls;
		public System.Collections.Generic.List<CommonControls> ctls2;
		public System.Collections.Generic.List<ButtonList> button_list;
		private bool system_up = true;
		private PortSet2 psDlg = new PortSet2();
		private PortSet2 psDlg2 = new PortSet2();
		private List<UIFormat> ui_format;
		private int current_button = 0;
		private int previous_button = 0;
		private int no_buttons = 14;
		int i = 0;
		int j = 0;
		string xml_file_location = "c:\\users\\daniel\\other_dev\\EpServerClient\\EpServerEngineSampleClient\\uiformat.xml";
		//string xml_file_location = "c:\\Users\\Dan_Laptop\\dev\\EpServerClient\\EpServerEngineSampleClient\\uiformat.xml";
		public FrmSampleClient()
		{
			InitializeComponent();
			ctls = new List<CommonControls>();
			ctls2 = new List<CommonControls>();
			button_list = new List<ButtonList>();
			//            use_main_odata = true;
			this.conn = new System.Data.SqlClient.SqlConnection(connectionString);
			//            this.cmd = new System.Data.SqlClient.SqlCommand("UPDATE O_DATA SET label=@label WHERE port=@recno", conn);
			svrcmd.SetClient(m_client);

			tbHostname.Enabled = true;
			tbReceived.Enabled = true;
			tbPort.Enabled = true;
			/*
									shutdown.Enabled = true;		// these have to be set true to use in debugger
									button2.Enabled = true;
									btnShutdown.Enabled = true;
									btnReboot.Enabled = true;
									btnStopSerial.Enabled = true;
									btn_SetTime.Enabled = true;
									btnGetTime.Enabled = true;
			*/
			button3.Enabled = false;    // these are normally set false
			button2.Enabled = false;
			btnShutdown.Enabled = false;
			btnReboot.Enabled = false;
			btnStopSerial.Enabled = false;
			btn_SetTime.Enabled = false;
			btnGetTime.Enabled = false;

			//Upload_New.Enabled = false;
			//btnSetParams.Enabled = false;
			//tbConnected.Text = "not connected";
			//target_db_closed = false;
			//use_laptop = true;
			tbReceived.Clear();
			psDlg.SetClient(m_client);
			psDlg2.SetClient(m_client);

			i = 0;
			ui_format = new List<UIFormat>();
			XmlReader xmlReader = new XmlTextReader(xml_file_location);
			UIFormat item = null;
			while (xmlReader.Read())
			{
				if (xmlReader.NodeType == XmlNodeType.Text)
				{
					//					AddMsg(xmlReader.Value.ToString());

					switch (i)
					{
						case 0:
							item = new UIFormat();
							item.Dlg_no = xmlReader.ReadContentAsInt();
							i++;
							break;
						case 1:
							item.Label = xmlReader.Value.ToString();
							//AddMsg(item.Label);
							i++;
							break;
						case 2:
							item.Command = xmlReader.ReadContentAsInt();
							//AddMsg(item.Command.ToString());
							i++;
							break;
						case 3:
							item.Length = xmlReader.ReadContentAsInt();
							i = 0;
							ui_format.Add(item);
							item = null;
							break;
					}
				}
			}
			//AddMsg("found XML file...");

			for (i = 0; i < ui_format.Count(); i++)
			{
				AddMsg(i.ToString() + " " + ui_format[i].Dlg_no.ToString() + " " + ui_format[i].Label + " " + ui_format[i].Command.ToString() + " " + ui_format[i].Length.ToString());
			}
			AddMsg("");

			i = 0;
			foreach (Button btn in psDlg.Controls.OfType<Button>())
			{
				// not getting the 1st one unless either of these are commented out
				//if (btn.Enabled && ui_format[i].Dlg_no == 0)
				//if (ui_format[i].Dlg_no == 0)
				{
					AddMsg(btn.TabIndex.ToString() + " " + btn.Name + " " + btn.Text);
					ctls.Add(new CommonControls()
					{
						CtlName = btn.Name,
						CtlText = ui_format[i].Label,
						TabOrder = btn.TabIndex,
						Ctlinst = btn,
						cmd = ui_format[i].Command,
						len = ui_format[i].Length,
						offset = 0
					});
				}
				i++;
				//                AddMsg(btn.Location.ToString());
			}
			//ctls.Reverse();
			psDlg.SetList(ctls);    // AFAICT the buttons are loaded starting with the highest tab order
									// first so let't reverse them and hope for the best
			psDlg.SetButtonLabels();
			psDlg.Name = "Dialog One";
			AddMsg("");
			j = 0;
			foreach (Button btn in psDlg2.Controls.OfType<Button>())
			{
				//if (btn.Enabled && ui_format[i].Dlg_no == 1)
				//if (ui_format[i].Dlg_no == 1)
				{
					AddMsg(btn.TabIndex.ToString() + " " + btn.Name + " " + btn.Text);
					ctls2.Add(new CommonControls()
					{
						CtlName = btn.Name,
						CtlText = ui_format[i].Label,
						TabOrder = btn.TabIndex,
						Ctlinst = btn,
						cmd = ui_format[i].Command,
						len = ui_format[i].Length,
						offset = 0
					});
				}
				i++;
				//                AddMsg(btn.Location.ToString());
			}
			//ctls2.Reverse();
			psDlg2.SetList(ctls2);
			psDlg2.SetButtonLabels();

			//			foreach (Button btn in this.Controls.OfType<Button>)

			Control sCtl = this.btnConnect;
			//Control sCtl = this.GetNextControl(this.btnConnect, true);

			//Control parent = sCtl.Parent;
			for(i = 0;i < this.Controls.Count;i++)
			{
				if (sCtl.GetType() == typeof(Button))
				{
					button_list.Add(new ButtonList()
					{
						TabOrder = sCtl.TabIndex,
						Ctl = (Button)sCtl,
						Enabled = sCtl.Enabled,
						Name = sCtl.Name
					});
					sCtl = GetNextControl(sCtl, true);
				}
			}
	
/*
			foreach (ButtonList btn in button_list)
			{
				AddMsg("Name: " + btn.Name + " en: " + btn.Enabled.ToString());
			}
*/
		}

		private void btnConnect_Click(object sender, EventArgs e)
		{
			if (btnConnect.Text.Equals("Connect"))
			{

				string hostname = tbHostname.Text;
				string port = tbPort.Text;

				tbHostname.Enabled = false;     /// from here to MPH should be commented out when in debugger
				tbPort.Enabled = false;
				//tbSend.Enabled = true;
				btnConnect.Text = "Disconnect";
				btnShutdown.Enabled = true;
				btnReboot.Enabled = true;
				btnStopSerial.Enabled = true;
				tbServerTime.Text = "";
				tbEngRunTime.Text = "";
				tbEngineTemp.Text = "";
				tbRPM.Text = "";
				tbMPH.Text = "";

				btn_SetTime.Enabled = true;
				btnGetTime.Enabled = true;
				//Upload_New.Enabled = true;
				ClientOps ops = new ClientOps(this, hostname, port);
				m_client.Connect(ops);
				/*
								if (m_client.IsConnectionAlive)
									AddMsg("connect");
								else 
									AddMsg("not connect");
				//                MessageBox.Show("connected?");
				*/
			}
			else
			{
				string cmd = "DISCONNECT";
				int offset = svrcmd.GetCmdIndexI(cmd);
				svrcmd.Send_Cmd(offset);
				tbHostname.Enabled = true;
				tbPort.Enabled = true;
				btnConnect.Text = "Connect";    // comment out in debug
				btnShutdown.Enabled = false;
				btnReboot.Enabled = false;
				btnStopSerial.Enabled = false;
				btn_SetTime.Enabled = false;
				btnGetTime.Enabled = false;
				//Upload_New.Enabled = false;

				tbEngRunTime.Text = "";         /// comment the next 4 out in debug
				tbEngineTemp.Text = "";
				tbRPM.Text = "";
				tbMPH.Text = "";
				tbServerTime.Text = "";

				if (m_client.IsConnectionAlive)
					m_client.Disconnect();
				//                string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.CLOSE_SOCKET);
				//                AddMsg("shutdown: " + cmd);
				//                Send_Cmd(cmd, 0);
			}
			reevaluate_enabled_buttons();
		}
		public void OnConnected(INetworkClient client, ConnectStatus status)
		{

			tbConnected.Text = "connected";     // comment all these out in debug
												//            cblistCommon.Enabled = true;      this one stays commneted out
			btnConnect.Text = "Disconnect";
			button3.Enabled = true;             // shutdown engine button
			button2.Enabled = true;             // start engine
			btnSetParams.Enabled = true;
			reevaluate_enabled_buttons();
		}
		public void OnDisconnect(INetworkClient client)
		{
			int i;
			tbHostname.Enabled = true;
			tbPort.Enabled = true;
			//            cblistCommon.Enabled = false;
			btnConnect.Text = "Connect";
			tbConnected.Text = "not connected";
			//            shutdown.Enabled = false;
			//            button2.Enabled = false;
			btnSetParams.Enabled = false;
			/*
						for (i = 0; i < ctls.Count; i++)
						{
							ctls[i].CtlSet = 0;
							ctls[i].Changed = 0;
						}
						for (i = 0; i < cblistCommon.Items.Count; i++)
						{
							cblistCommon.SetItemChecked(i, false);
						}
			*/
			reevaluate_enabled_buttons();
		}
		protected override void OnClosed(EventArgs e)
		{
			if (m_client.IsConnectionAlive)
				m_client.Disconnect();
			psDlg.Dispose();
			psDlg2.Dispose();
			base.OnClosed(e);
		}
		public void OnReceived(INetworkClient client, Packet receivedPacket)
		{
			if (psDlg.Visible == true)
			{
				psDlg.Process_Msg(receivedPacket.PacketRaw);
				AddMsg("dlg 1 up");
			}
			else if (psDlg2.Visible == true)
			{
				psDlg2.Process_Msg(receivedPacket.PacketRaw);
				AddMsg("dlg 2 up");
			}
			else
				Process_Msg(receivedPacket.PacketRaw);
		}
		private void Process_Msg(byte[] bytes)
		{
			int type_msg;
			string ret = null;
			int i = 0;
			char[] chars = new char[bytes.Length / sizeof(char) + 2];
			char[] chars2 = new char[bytes.Length / sizeof(char)];
			// src srcoffset dest destoffset len
			System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
			type_msg = (int)chars[0];
			System.Buffer.BlockCopy(bytes, 2, chars2, 0, bytes.Length - 2);
			ret = new string(chars2);

			//            string str = Enum.GetName(typeof(msg_types), type_msg);
			string str = svrcmd.GetName(type_msg);
			//AddMsg(ret + " " + str + " " + type_msg.ToString());

			switch (str)
			{
				case "SEND_MSG":
					if (ret == "SHUTDOWN")
					{
						//		                tbEngineTemp.Text = "";
						tbRPM.Text = "0";
						tbMPH.Text = "0";
						//                       tbEngRunTime.Text = "";
					}
					break;
				case "CURRENT_TIME":
					//                    AddMsg(mins.ToString() + " " + sec.ToString() + " " + "curr" + " " + svrcmd.GetName(cmd));
					//                    AddMsg(mins.ToString() + " " + sec.ToString());
					break;
				case "SERVER_UPTIME":
					//                    tbServerTime.Text = hours.ToString() + ':' + mins.ToString() + ':' + sec.ToString();
					tbServerTime.Text = ret;		// comment out during debug
					break;
				case "ENGINE_RUNTIME":
					//					tbEngRunTime.Text = hours.ToString() + ':' + mins.ToString() + ':' + sec.ToString();
					tbEngRunTime.Text = ret;		// comment out for debug
					break;
				case "ENGINE_TEMP":
					tbEngineTemp.Text = ret;
					break;
				case "SEND_RPM":
					tbRPM.Text = ret;			// comment out for debug
					break;
				case "SEND_MPH":
					tbMPH.Text = ret;			// comment out for debug
					break;
				case "SEND_CONFIG":
					string[] words = ret.Split(' ');
					i = 0;
					foreach (var word in words)
					{
						//                    temp = int.Parse(word);
						switch (i)
						{
							case 0:
								cfg_params.rpm_update_rate = int.Parse(word);
								break;
							case 1:
								cfg_params.mph_update_rate = int.Parse(word);
								break;
							case 2:
								cfg_params.FPGAXmitRate = int.Parse(word);
								break;
							case 3:
								cfg_params.high_rev_limit = int.Parse(word);
								break;
							case 4:
								cfg_params.low_rev_limit = int.Parse(word);
								break;
							case 5:
								cfg_params.fan_on = int.Parse(word);
								break;
							case 6:
								cfg_params.fan_off = int.Parse(word);
								break;
							case 7:
								cfg_params.blower_enabled = int.Parse(word);
								break;
							case 8:
								cfg_params.blower1_on = int.Parse(word);
								break;
							case 9:
								cfg_params.blower2_on = int.Parse(word);
								break;
							case 10:
								cfg_params.blower3_on = int.Parse(word);
								break;
							case 11:
								cfg_params.test_bank = int.Parse(word);
								break;
							default:
								break;
						}
						//                        MessageBox.Show(int.Parse(word).ToString());
						i++;
					}
					break;
				case "GET_TIME":
					AddMsg(ret);
					break;

				case "SHUTDOWN":
					tbHostname.Enabled = true;
					tbPort.Enabled = true;
					btnConnect.Text = "Connect";
					btnShutdown.Enabled = false;
					btnReboot.Enabled = false;
					btnStopSerial.Enabled = false;
					btn_SetTime.Enabled = false;
					btnGetTime.Enabled = false;
					Upload_New.Enabled = false;
					tbEngRunTime.Text = "";
					tbEngineTemp.Text = "";
					tbServerTime.Text = "";
					tbRPM.Text = "0";
					tbMPH.Text = "0";
					if (m_client.IsConnectionAlive)
						m_client.Disconnect();

					AddMsg(ret);
					reevaluate_enabled_buttons();
					break;

				case "SYSTEM_UP":
					AddMsg(ret);
					break;

				case "SYSTEM_DOWN":
//					AddMsg(ret);
					break;

				case "NAV_UP":
					navigate_buttons(str);
//					AddMsg(str);
					break;

				case "NAV_DOWN":
					navigate_buttons(str);
//					AddMsg(str);
					break;

				case "NAV_CLICK":
					navigate_buttons(str);
//					AddMsg(str);
					break;

				default:
					break;
			}
		}
		void ProcessIOnums(byte[] bytes)
		{
			int temp;
			char[] chars = new char[bytes.Length / sizeof(char)];
			System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
			string nums = new string(chars);
			//AddMsg(nums);

			string[] words = nums.Split(' ');
			//foreach (var word in words)
			//{
			//    if (word == "xx")
			//        AddMsg("nl");
			//    else
			//    {
			//        temp = int.Parse(word);
			//        AddMsg(temp.ToString());
			//    }
			//}
		}
		byte[] BytesFromString(String str)
		{
			byte[] bytes = new byte[str.Length * sizeof(char)];
			System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
			return bytes;
		}
		public void OnSent(INetworkClient client, SendStatus status, Packet sentPacket)
		{
			switch (status)
			{
				case SendStatus.SUCCESS:
					Debug.WriteLine("SEND Success");
					break;
				case SendStatus.FAIL_CONNECTION_CLOSING:
					AddMsg(status.ToString());
					Debug.WriteLine("SEND failed due to connection closing");
					break;
				case SendStatus.FAIL_INVALID_PACKET:
					AddMsg(status.ToString());
					Debug.WriteLine("SEND failed due to invalid socket");
					break;
				case SendStatus.FAIL_NOT_CONNECTED:
					AddMsg(status.ToString());
					Debug.WriteLine("SEND failed due to no connection");
					break;
				case SendStatus.FAIL_SOCKET_ERROR:
					AddMsg(status.ToString());
					Debug.WriteLine("SEND Socket Error");
					break;
			}

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
		String StringFromByteArr(byte[] bytes)
		{
			char[] chars = new char[bytes.Length / sizeof(char)];
			System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
			return new string(chars);
		}
		private void connect(object sender, EventArgs e)
		{
			btnConnect_Click(sender, e);
		}
		// start engine
		private void button2_Click(object sender, EventArgs e)
		{
			string cmd = "START_SEQ";
			AddMsg("start seq: " + cmd);
			//svrcmd.Send_Cmd(cmd, 0);
			int offset = svrcmd.GetCmdIndexI(cmd);
			svrcmd.Send_Cmd(offset);
		}
		// shutdown engine
		private void shutdown_Click_1(object sender, EventArgs e)
		{
			string cmd = "SHUTDOWN";
			int offset = svrcmd.GetCmdIndexI(cmd);
			svrcmd.Send_Cmd(offset);
		}
		private void Btn_SetTime_Click(object sender, EventArgs e)
		{
			DateTime localDate = DateTime.Now;
			String cultureName = "en-US";
			var culture = new CultureInfo(cultureName);
			AddMsg(localDate.ToString(culture));

			byte[] bytes = BytesFromString(localDate.ToString(culture));
			byte[] bytes2 = new byte[bytes.Count() + 2];
			System.Buffer.BlockCopy(bytes, 0, bytes2, 2, bytes.Length - 2);
			string set_time = "SET_TIME";
			bytes2[0] = svrcmd.GetCmdIndexB(set_time);
			Packet packet = new Packet(bytes2, 0, bytes2.Count(), false);
			m_client.Send(packet);
			//			AddMsg(bytes2.Length.ToString());
		}
		public void ShowMyDialogBox()
		{
			DlgForm1 testDialog = new DlgForm1();
			testDialog.SetClient(m_client);
			// Show testDialog as a modal dialog and determine if DialogResult = OK.
			if (testDialog.ShowDialog(this) == DialogResult.OK)
			{
				//                AddMsg("dlg = OK");
			}
			else
			{
				//                this.txtResult.Text = "Cancelled";
			}
			testDialog.Dispose();
		}

		private void ShutdownServer(object sender, EventArgs e)
		{
			string cmd = "SHUTDOWN_IOBOX";
			int offset = svrcmd.GetCmdIndexI(cmd);
			svrcmd.Send_Cmd(offset);
		}
		private void RebootServer(object sender, EventArgs e)
		{
			string cmd = "REBOOT_IOBOX";
			int offset = svrcmd.GetCmdIndexI(cmd); 
			svrcmd.Send_Cmd(offset);
		}
		private void StopMbox(object sender, EventArgs e)
		{
			if (system_up)
			{
				string cmd = "STOP_MBOX_XMIT";
				int offset = svrcmd.GetCmdIndexI(cmd);
				svrcmd.Send_Cmd(offset);
				system_up = false;
				btnStopSerial.Text = "Start Mbox Xmit";
				AddMsg("System Down");
			}
			else
			{
				string cmd = "START_MBOX_XMIT";
				int offset = svrcmd.GetCmdIndexI(cmd);
				svrcmd.Send_Cmd(offset);
				system_up = true;
				btnStopSerial.Text = "Stop Mbox Xmit";
				AddMsg("System Up");
			}
		}
		private void DBMgmt(object sender, EventArgs e)
		{
			ShowMyDialogBox();
		}
		private void ClearScreen(object sender, EventArgs e)
		{
			tbReceived.Clear();
		}
		private void GetTime(object sender, EventArgs e)
		{
			string cmd = "GET_TIME";
			int offset = svrcmd.GetCmdIndexI(cmd);
			svrcmd.Send_Cmd(offset);
		}
		private void SetParamsClick(object sender, EventArgs e)
		{
			DlgSetParams testDialog = new DlgSetParams(cfg_params);
			testDialog.SetClient(m_client);
			// Show testDialog as a modal dialog and determine if DialogResult = OK.
			if (testDialog.ShowDialog(this) == DialogResult.OK)
			{
				cfg_params = testDialog.GetParams();
			}
			else
			{
				//                this.txtResult.Text = "Cancelled";
			}
			testDialog.Dispose();
		}
		private void Upload_New_Click(object sender, EventArgs e)
		{
			/*
                        UInt32 fsize;
                        UInt32 chunk_size = 10000;
                        UInt32 iters;
                        UInt32 rem;
                        int i;

                        byte[] tbytes = ReadFile("c:\\users\\daniel\\dev\\sched");
                        string no_bytes = tbytes.Count().ToString();
            //            AddMsg(no_bytes);
                        fsize = (UInt32)tbytes.Count();
                        if(fsize > chunk_size)
                        {
                            iters = fsize/chunk_size;
                            rem = fsize - (iters*chunk_size);
                        }
                        else 
                        {
                            chunk_size = fsize;
                            iters = 1;
                            rem = 0;
                        }

                        AddMsg(fsize.ToString());
                        AddMsg(iters.ToString());
                        AddMsg(chunk_size.ToString());
                        AddMsg(rem.ToString());

                        byte[] bfsize = new byte[8];
                        bfsize[0] = (byte)fsize;
                        fsize >>= 8;
                        bfsize[2] = (byte)fsize;
                        fsize >>= 8;
                        bfsize[4] = (byte)fsize;
                        fsize >>= 8;
                        bfsize[6] = (byte)fsize;
                        byte[] total_bytes = new byte[bfsize.Count() + 2];
            //			AddMsg(bfsize.Length.ToString());
            //			AddMsg(total_bytes.Length.ToString());

                        // BlockCopy(src, srcoffset, dest, destoffset, count)
                        System.Buffer.BlockCopy(bfsize, 0, total_bytes, 2, bfsize.Length - 2);
                        string cmd = "UPLOAD_NEW";
                        total_bytes[0] = svrcmd.GetCmdIndexB(cmd);

                        Packet packet = new Packet(total_bytes, 0, total_bytes.Count(), false);
            //			AddMsg(packet.PacketByteSize.ToString());
                        m_client.Send(packet);

            //			byte[] total_bytes2 = new byte[bytes.Count() + 2];
                        byte[] total_bytes2 = new byte[chunk_size + 2];

                        byte[] rem_bytes = new byte[rem + 2];
                        AddMsg(rem.ToString());
                        Packet packet2 = new Packet(total_bytes2, 0, total_bytes2.Count(), false);
                        Packet packet3 = new Packet(rem_bytes, 0, rem_bytes.Count(), false);
                        cmd = "UPLOAD_NEW2";
                        for(i = 0;i < iters;i++)
                        {
                            System.Buffer.BlockCopy(tbytes, (int)(chunk_size*i), total_bytes2, 2, total_bytes2.Length - 2);
                            total_bytes2[0] = svrcmd.GetCmdIndexB(cmd);
            //				AddMsg(packet2.PacketByteSize.ToString());
                            m_client.Send(packet2);
                        }
                        System.Buffer.BlockCopy(tbytes, (int)(chunk_size*i), rem_bytes, 2, rem_bytes.Length - 2);
                        rem_bytes[0] = svrcmd.GetCmdIndexB(cmd);
            //			AddMsg(packet3.PacketByteSize.ToString());
                        m_client.Send(packet3);
            */

			//            foreach (Button btn in Controls.OfType<Button>())
			//               AddMsg(btn.Name);
			psDlg2.Name = "Dialog Two";
			psDlg2.Enable_Dlg(true);
			if (psDlg2.ShowDialog(this) == DialogResult.OK)
			{
				//                AddMsg("dlg = OK");
			}
			else
			{
				//                this.txtResult.Text = "Cancelled";
			}
			psDlg2.Enable_Dlg(false);
		}
		/*
                public static IEnumerable<Button> Buttons(this ControlCollection controls)
                {
                    return controls.OfType<Button>();
                }
        */
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

		private void btnPortSettings_Click(object sender, EventArgs e)
		{
			// Show testDialog as a modal dialog and determine if DialogResult = OK.
			psDlg.Name = "Dialog One";
			psDlg.Enable_Dlg(true);
			if (psDlg.ShowDialog(this) == DialogResult.OK)
			{
				//                AddMsg("dlg = OK");
			}
			else
			{
				//                this.txtResult.Text = "Cancelled";
			}
			psDlg.Enable_Dlg(false);
			//psDlg.Dispose();  don't do this if psDlg was created in constructor

		}

		private void btnLoadXML_Click(object sender, EventArgs e)
		{
			foreach (ButtonList btn in button_list)
			{
				AddMsg("Name: " + btn.Name + " en: " + btn.Enabled.ToString());
			}
			/*
						try
						{
							XmlReader xmlFile = null;
							DataSet ds2 = new DataSet();

							var fileContent = string.Empty;
							var filePath = string.Empty;

							using (OpenFileDialog openFileDialog = new OpenFileDialog())
							{
								openFileDialog.InitialDirectory = "c:\\users\\daniel\\dev";
								//                    openFileDialog.Filter = "xml files (*.xml)|*.xml|All files (*.*)|*.*";
								openFileDialog.Filter = "xml files (*.xml)|*.xml";
								openFileDialog.FilterIndex = 2;
								openFileDialog.RestoreDirectory = true;

								if (openFileDialog.ShowDialog() == DialogResult.OK)
								{
									//Get the path of specified file
									filePath = openFileDialog.FileName;

									//Read the contents of the file into a stream
									//var fileStream = openFileDialog.OpenFile();

									//using (StreamReader reader = new StreamReader(fileStream))
									//{
									//    fileContent = reader.ReadToEnd();
									//}
								}
							}
							//                MessageBox.Show(filePath.ToString());

							xmlFile = XmlReader.Create(filePath, new XmlReaderSettings());
							ds2.ReadXml(xmlFile);
							ds2.
			//				dataGridView1.DataSource = ds2.Tables[0];
						}
						catch (Exception ex)
						{
							MessageBox.Show(ex.ToString());
						}
			*/
		}

		private void reevaluate_enabled_buttons()
		{
			foreach(ButtonList btn in button_list)
			{
				btn.Enabled = btn.Ctl.Enabled;
			}
		}

		private void navigate_buttons(string str)
		{
			previous_button = current_button;
			
			switch (str)
			{
				case "NAV_UP":
					if(true)
					{
						current_button--;
						if (current_button < 0)
							current_button = no_buttons - 1;
					}
					//button_list[current_button].Ctl.Focus();
					//button_list[current_button].Ctl.Select();
					//button_list[current_button].Ctl.Text += " X";
					button_list[current_button].Ctl.BackColor = Color.White;
					//button_list[previous_button].Ctl.Text = button_list[previous_button].Name;
					button_list[previous_button].Ctl.BackColor = Color.Aqua;


					AddMsg("up  " + button_list[current_button].Name);
					//textBox1.Text = "up  " + button_list[current_button].Name;
					break;

				case "NAV_DOWN":
					if(true)
					{
						//do
							current_button++;
						//while (button_list[current_button - 1].Enabled == false);
						if (current_button > no_buttons - 1)
							current_button = 0;
					}
					//button_list[current_button].Ctl.Select();
					//					button_list[current_button].Ctl.Focus();
					button_list[current_button].Ctl.BackColor = Color.Aqua;
					button_list[previous_button].Ctl.BackColor = Color.White;
					AddMsg("down  " + button_list[current_button].Name);
					//textBox1.Text = "down  " + button_list[current_button].Name;
					break;
				case "NAV_CLICK":
					if(current_button > 1 && current_button < 6)
//					if (button_list[current_button].Name == "Set Time" || button_list[current_button].Name == "Get Time")
					{
						button_list[current_button].Ctl.PerformClick();
						AddMsg("click time");
						textBox1.Text = "time";
					}
					textBox1.Text = "click";
				
					break;
			}
		}
	}
}
