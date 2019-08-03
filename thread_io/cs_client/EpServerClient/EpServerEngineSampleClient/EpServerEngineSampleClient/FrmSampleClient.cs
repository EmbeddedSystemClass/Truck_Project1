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
using System.Timers;


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
		public System.Collections.Generic.List<CommonControls> ctls3;
		public System.Collections.Generic.List<ButtonList> button_list;
		private bool system_up = true;
		private PortSet2 psDlg = new PortSet2();
		private PortSet2 psDlg2 = new PortSet2();
		private PortSet2 psDlg3 = new PortSet2();
		private List<UIFormat> ui_format;
		private List<ClientParams> client_params;
		private int current_button = 0;
		private int previous_button = 0;
		private int no_buttons = 14;
		private int i = 0;
		private int selected_address = 0;
		private bool avr_running = false;
		private int please_lets_disconnect = 0;
		private int disconnect_attempts = 0;
		private string m_hostname;
		private string m_portno;
		private bool m_engine_running = false;
		private int server_up_seconds = 0;
		private int timer_server_up_seconds = 0;
		private int previous_timer_server_up_seconds = 0;
		//private int server_connection_attempts = 1;
		private bool client_connected = false;
		private string xml_file_location_desktop = "c:\\users\\daniel\\other_dev\\EpServerClient\\EpServerEngineSampleClient\\uiformat.xml";
		private string xml_file_location_laptop = "c:\\Users\\Dan_Laptop\\dev\\uiformat.xml";
		private string xml_file2_location_desktop = "c:\\users\\daniel\\other_dev\\EpServerClient\\EpServerEngineSampleClient\\ClientParams.xml";
		private string xml_file2_location_laptop = "c:\\Users\\Dan_Laptop\\dev\\ClientParams.xml";

		public FrmSampleClient()
		{
			InitializeComponent();
			ctls = new List<CommonControls>();
			ctls2 = new List<CommonControls>();
			ctls3 = new List<CommonControls>();
			button_list = new List<ButtonList>();
			//            use_main_odata = true;
			this.conn = new System.Data.SqlClient.SqlConnection(connectionString);
			//            this.cmd = new System.Data.SqlClient.SqlCommand("UPDATE O_DATA SET label=@label WHERE port=@recno", conn);
			svrcmd.SetClient(m_client);

			cbIPAdress.Enabled = true;
			tbReceived.Enabled = true;
			tbPort.Enabled = true;
			/*
									shutdown.Enabled = true;		// these have to be set true to use in debugger
									StartEng.Enabled = true;
									btnShutdown.Enabled = true;
									btnReboot.Enabled = true;
									btnStopSerial.Enabled = true;
									btn_SetTime.Enabled = true;
									btnGetTime.Enabled = true;
			*/
			btnTestPorts.Enabled = true;    // these are normally set false
			btnStartEng.Enabled = false;
			btnStartEng.Text = "Start Engine";
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
			psDlg3.SetClient(m_client);

			i = 0;
			ui_format = new List<UIFormat>();
			XmlTextReader xmlReader = new XmlTextReader(File.Exists(xml_file_location_laptop) ? xml_file_location_laptop : xml_file_location_desktop);
			// this doesn't work if the string 'xml_file_location... are not private
			UIFormat item = null;
			
			while(xmlReader.Read())
			{
				if (xmlReader.NodeType == XmlNodeType.Text)
				{
					switch (i)
					{
						case 0:
							item = new UIFormat();
							item.Label = xmlReader.Value.ToString();
							//AddMsg(item.Label);
							i++;
							break;
						case 1:
							item.Command = xmlReader.ReadContentAsInt();
							//AddMsg(item.Command.ToString());
							i++;
							break;
						case 2:
							item.Length = xmlReader.ReadContentAsInt();
							i = 0;
							ui_format.Add(item);
							item = null;
							break;
					}
				}
			}
			ui_format.Reverse();

			foreach (UIFormat ui in ui_format)
			{
				AddMsg(ui.Label + " " + ui.Command.ToString() + " " + ui.Length.ToString());
			}

			client_params = new List<ClientParams>();
			XmlTextReader xmlReader2 = new XmlTextReader(File.Exists(xml_file2_location_laptop) ? xml_file2_location_laptop : xml_file2_location_desktop);
			// this doesn't work if the string 'xml_file_location... are not private
			ClientParams item2 = null;

			i = 0;
			while (xmlReader2.Read())
			{
				if (xmlReader2.NodeType == XmlNodeType.Text)
				{
					switch (i)
					{
						case 0:
							item2 = new ClientParams();
							item2.AutoConn = xmlReader2.ReadContentAsBoolean();
							//AddMsg(item2.AutoConn.ToString());
							i++;
							break;
						case 1:
							item2.IPAdress = xmlReader2.Value.ToString();
							//AddMsg(item2.IPAdress);
							i++;
							break;
						case 2:
							item2.PortNo = xmlReader2.ReadContentAsInt();
							//AddMsg(item2.PortNo.ToString());
							i++;
							break;
						case 3:
							item2.Primary = xmlReader2.ReadContentAsBoolean();
							i++;
							break;
						case 4:
							item2.AttemptsToConnect = xmlReader2.ReadContentAsInt();
							client_params.Add(item2);
							item2 = null;
							i = 0;
							break;
					}
				}
			}

			bool found = false;
			for (i = 0;i < client_params.Count();i++)
			{
				
				//AddMsg(client_params[i].AutoConn.ToString() + " " + client_params[i].IPAdress + " " + client_params[i].PortNo.ToString() + " " + client_params[i].AttemptsToConnect.ToString());
				cbIPAdress.Items.Add(client_params[i].IPAdress);
				if(client_params[i].Primary)
				{
					m_hostname = cbIPAdress.Text = client_params[i].IPAdress;
					m_portno = tbPort.Text = client_params[i].PortNo.ToString();
					cbAutoConnecct.Checked = client_params[i].AutoConn;
					tbAttemptsToConnect.Text = client_params[i].AttemptsToConnect.ToString();
					selected_address = i;
					found = true;
				}
			}
			if(!found)
			{
				AddMsg("no primary address found in xml file");
			}

			i = 0;
			// there must be exactly 10 buttons in each dialog and 10 entries
			// of each Dlg_no in the xml file or this won't work
			// Dlg_no = 0 for 1st dialog, 1 for next and so on
			foreach (Button btn in psDlg.Controls.OfType<Button>())
			{
				// not getting the 1st one unless either of these are commented out
				//if (btn.Enabled && ui_format[i].Dlg_no == 0)
				//if (ui_format[i].Dlg_no == 0)
				{
					//AddMsg(btn.TabIndex.ToString() + " " + btn.Name + " " + btn.Text);
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
			}
			ctls.Reverse();
			psDlg.SetList(ctls);    // AFAICT the buttons are loaded starting with the highest tab order
									// first so let't reverse them and hope for the best
			psDlg.SetButtonLabels();
			psDlg.Name = "Dialog One";
			psDlg.Set_Type(true);

			foreach (Button btn in psDlg2.Controls.OfType<Button>())
			{
				//if (btn.Enabled && ui_format[i].Dlg_no == 1)
				//if (ui_format[i].Dlg_no == 1)
				{
					//AddMsg(btn.TabIndex.ToString() + " " + btn.Name + " " + btn.Text);
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
			ctls2.Reverse();
			psDlg2.SetList(ctls2);
			psDlg2.SetButtonLabels();
			psDlg.Name = "Dialog Two";
			psDlg.Set_Type(true);

			foreach (Button btn in psDlg3.Controls.OfType<Button>())
			{
				//if (btn.Enabled && ui_format[i].Dlg_no == 1)
				//if (ui_format[i].Dlg_no == 1)
				{
					//AddMsg(btn.TabIndex.ToString() + " " + btn.Name + " " + btn.Text);
					ctls3.Add(new CommonControls()
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
			ctls3.Reverse();
			psDlg3.SetList(ctls3);
			psDlg3.SetButtonLabels();
			psDlg.Name = "Dialog Three";
			psDlg.Set_Type(false);

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
			timer1.Enabled = true;
/*
			foreach (ButtonList btn in button_list)
			{
				AddMsg("Name: " + btn.Name + " en: " + btn.Enabled.ToString());
			}
*/
		}
		private void btnConnect_Click(object sender, EventArgs e)
		{
			if(!client_connected)		// let's connect here! (see timer callback at end of file)
			{
				m_hostname = cbIPAdress.Items[selected_address].ToString();
				m_portno = tbPort.Text;
				AddMsg("trying to connect to:    " + m_hostname + "   Port: " + m_portno.ToString() + "...");
				ClientOps ops = new ClientOps(this, m_hostname, m_portno);
				m_client.Connect(ops);
				please_lets_disconnect = 0;
				disconnect_attempts++;
			}
			else
			{
				please_lets_disconnect = 1; // let's disconnect here!
				disconnect_attempts = 0;
			}
			reevaluate_enabled_buttons();
		}
		public void OnConnected(INetworkClient client, ConnectStatus status)
		{

			if (m_client.IsConnectionAlive)
			{
				tbConnected.Text = "connected";     // comment all these out in debug
													//            cblistCommon.Enabled = true;      this one stays commneted out
				btnConnect.Text = "Disconnect";
				btnTestPorts.Enabled = true;             // shutdown engine button
				btnStartEng.Enabled = true;             // start engine
				//btnStartEng.Text = "Stop Engine";
				btnSetParams.Enabled = true;
				cbIPAdress.Enabled = false;     /// from here to MPH should be commented out when in debugger
				tbPort.Enabled = false;

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
				reevaluate_enabled_buttons();
			}
		}
		public void OnDisconnect(INetworkClient client)
		{
			cbIPAdress.Enabled = true;
			tbPort.Enabled = true;
			btnConnect.Text = "Connect";
			tbConnected.Text = "not connected";
			btnShutdown.Enabled = false;
			btnStartEng.Enabled = false;
			//btnStartEng.Text = "Stop Engine";
			btnSetParams.Enabled = false;
			reevaluate_enabled_buttons();
			timer_server_up_seconds = 0;
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
			}
			else if (psDlg2.Visible == true)
			{
				psDlg2.Process_Msg(receivedPacket.PacketRaw);
			}
			else
				Process_Msg(receivedPacket.PacketRaw);
		}
		private void Process_Msg(byte[] bytes)
		{
			string substr;
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
						tbRPM.Text = "0";
						tbMPH.Text = "0";
					}
					AddMsg(ret);
					break;
				case "CURRENT_TIME":
					AddMsg(ret);
					break;
				case "SERVER_UPTIME":
					substr = ret.Substring(0, 2);
					server_up_seconds++;
					if (substr == "0h")
					{
						substr = ret.Substring(3, ret.Length - 3);
						tbServerTime.Text = substr;
					}
					else
						tbServerTime.Text = ret;
					break;

				case "ENGINE_RUNTIME":
					substr = ret.Substring(0, 2);
					if (substr == "0h")
					{
						substr = ret.Substring(3, ret.Length - 3);
						tbEngRunTime.Text = substr;
					}
					else
						tbEngRunTime.Text = ret;
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

				case "START_SEQ":
					m_engine_running = true;
					AddMsg(ret);
					btnStartEng.Text = "Stop Engine";
					break;

				case "SHUTDOWN":		// msg from server saying engine has been shutdown
					m_engine_running = false;
					tbEngRunTime.Text = "";
					tbEngineTemp.Text = "";
					//tbServerTime.Text = "";
					tbRPM.Text = "0";
					tbMPH.Text = "0";
					btnStartEng.Text = "Start Engine";
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
				case "NAV_DOWN":
				case "NAV_SIDE":
				case "NAV_CLICK":
				case "NAV_CLOSE":
					navigate_buttons(str);
					break;

				case "ESTOP_SIGNAL":
					AddMsg("ESTOP: " + ret);
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
		private void StartEng_Click(object sender, EventArgs e)
		{
			if (!m_engine_running)
			{
				string cmd = "START_SEQ";
				//AddMsg("start seq: " + cmd);
				int offset = svrcmd.GetCmdIndexI(cmd);
				svrcmd.Send_Cmd(offset);
				//btnStartEng.Text = "Stop Engine";
			}
			else
			{
				string cmd = "SHUTDOWN";
				//AddMsg("start seq: " + cmd);
				int offset = svrcmd.GetCmdIndexI(cmd);
				svrcmd.Send_Cmd(offset);
				//btnStartEng.Text = "Start Engine";
			}
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
		private void ShutdownServer(object sender, EventArgs e)
		{
			string cmd = "SHUTDOWN_IOBOX";
			int offset = svrcmd.GetCmdIndexI(cmd);
			svrcmd.Send_Cmd(offset);
			please_lets_disconnect = 1;
		}
		private void RebootServer(object sender, EventArgs e)
		{
			string cmd = "REBOOT_IOBOX";
			int offset = svrcmd.GetCmdIndexI(cmd); 
			svrcmd.Send_Cmd(offset);
			please_lets_disconnect = 1;
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

		private void btnAVR_Click(object sender, EventArgs e)
		{
			string cmd = "";
			int offset = 0;

			if (avr_running)
			{
				cmd = "STOP_AVR_XMIT";
				offset = svrcmd.GetCmdIndexI(cmd);
				svrcmd.Send_Cmd(offset);
				avr_running = false;
				AddMsg("Set LCD Off");
				btnAVR.Text = "Set LCD On";
			}
			else
			{
				cmd = "START_AVR_XMIT";
				offset = svrcmd.GetCmdIndexI(cmd);
				svrcmd.Send_Cmd(offset);
				avr_running = true;
				AddMsg("Set LCD On");
				btnAVR.Text = "Set LCD Off";
			}
		}

		private void Dialog1_Click(object sender, EventArgs e)
		{
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
		private void Dialog2_Click(object sender, EventArgs e)
		{

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

		private void TestPorts_Click(object sender, EventArgs e)
		{
			psDlg3.Enable_Dlg(true);
			if (psDlg3.ShowDialog(this) == DialogResult.OK)
			{
				//                AddMsg("dlg = OK");
			}
			else
			{
				//                this.txtResult.Text = "Cancelled";
			}
			psDlg3.Enable_Dlg(false);
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
			switch (str)
			{
				case "NAV_UP":
					previous_button = current_button;
					current_button--;
					if (current_button < 0)
						current_button = no_buttons - 1;
					button_list[current_button].Ctl.BackColor = Color.Aqua;
					button_list[previous_button].Ctl.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));

					//AddMsg("up  " + button_list[current_button].Name + " " + button_list[current_button].TabOrder.ToString());
					//textBox1.Text = "up  " + button_list[current_button].Name;
					//AddMsg("up  " + button_list[current_button].Name + " " 
						//+ current_button.ToString() + " " + previous_button.ToString()+ " " + button_list[current_button].TabOrder.ToString());
					break;

				case "NAV_DOWN":
					previous_button = current_button;
					current_button++;
					if (current_button > no_buttons - 1)
						current_button = 0;
					button_list[current_button].Ctl.BackColor = Color.Aqua;
					button_list[previous_button].Ctl.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
//					AddMsg("up  " + button_list[current_button].Name + " " + button_list[current_button].TabOrder.ToString());
					//AddMsg("down  " + button_list[current_button].Name + " " 
						//+ current_button.ToString() + " " + previous_button.ToString() + " " + button_list[current_button].TabOrder.ToString());
					break;

				case "NAV_SIDE":
					previous_button = current_button;
					if (current_button > 6)
						current_button -= 7;
					else if(current_button < 7)
						current_button += 7;
					button_list[current_button].Ctl.BackColor = Color.Aqua;
					button_list[previous_button].Ctl.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
					//					AddMsg("up  " + button_list[current_button].Name + " " + button_list[current_button].TabOrder.ToString());
					//AddMsg("side  " + button_list[current_button].Name + " "
						//+ current_button.ToString() + " " + previous_button.ToString() + " " + button_list[current_button].TabOrder.ToString());
					break;
				case "NAV_CLICK":

					//if (button_list[current_button].Enabled && button_list[current_button].Name != "Stop Mbox Xmit" && 
					//button_list[current_button].Name != "Connect Server")
					if (button_list[current_button].Enabled)
					{
						int tab_order = button_list[current_button].TabOrder;
						// don't do a shutdown, reboot, disconnect or stop mbox xmit while using keypad!
						// it's like sawing off the branch your sitting on
						if (tab_order != 0 && tab_order != 9 && tab_order != 7 && tab_order != 8)
						{
							button_list[current_button].Ctl.PerformClick();
							AddMsg("click  " + button_list[current_button].Name + " "
								+ button_list[current_button].TabOrder.ToString());
							
						}
						else AddMsg("don't do this!");
					}
					else AddMsg("Not Enabled");
					break;
			}
		}

		private void myTimerTick(object sender, EventArgs e)
		{
			switch (please_lets_disconnect)            // send the msg to server saying we are disconnecting
			{
				case 0:
					if (m_client.IsConnectionAlive)
					{
						if (client_connected == false)
						{
							client_connected = true;
							cbIPAdress.Enabled = false;
							tbPort.Enabled = false;
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
							AddMsg("server connected");
						}else
						{
							previous_timer_server_up_seconds = timer_server_up_seconds;
							timer_server_up_seconds = server_up_seconds;
							tbServerUpTimeSeconds.Text = "server up seconds: " + 
								timer_server_up_seconds.ToString() + " " 
									+ "previous up seconds: " + previous_timer_server_up_seconds.ToString();
							if(timer_server_up_seconds > 2 && timer_server_up_seconds < 6)
								Btn_SetTime_Click(new object(),new EventArgs());
						}
					}
					else // if not alive
					{
						if (client_connected == true)
						{
							client_connected = false;
							cbIPAdress.Enabled = true;
							tbPort.Enabled = true;
							btnConnect.Text = "Connect";
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
							client_connected = false;
							AddMsg("server disconnected");
							tbServerUpTimeSeconds.Text = "";
						}
						else
						{
							if (disconnect_attempts > 0)
							{
								disconnect_attempts++;
								// this doesn't work as intended because it hangs for so long
								// before even going into this code
								//AddMsg("attempt to connect: " + disconnect_attempts.ToString());
								// this won't show because the app hangs while trying to connect
								if (disconnect_attempts > client_params[selected_address].AttemptsToConnect)
								{
									AddMsg("too many disconnect attempts");
									AddMsg("giving up trying to connect");
									disconnect_attempts = 0;
									please_lets_disconnect = 0;
									client_connected = false;
									cbIPAdress.Enabled = true;
									tbPort.Enabled = true;
									btnShutdown.Enabled = false;
									btnReboot.Enabled = false;
									btnStopSerial.Enabled = false;
									btn_SetTime.Enabled = false;
									btnGetTime.Enabled = false;
									client_connected = false;
								}
							}
						}
					}
					break;
				case 1:
					string cmd = "DISCONNECT";
					AddMsg("asking server to disconnect...");
					int offset = svrcmd.GetCmdIndexI(cmd);
					svrcmd.Send_Cmd(offset);
					please_lets_disconnect = 2;		// next time around disconnect anyway
					break;
				case 2:         // then wait 1 second to see if it really did disconnect us
					AddMsg("Drake, we are leaving...");
					if (m_client.IsConnectionAlive)
						m_client.Disconnect();
					please_lets_disconnect = 0;
					break;
			}
		}

		private void IPAddressChanged(object sender, EventArgs e)
		{
			if (!client_connected)
			{
				selected_address = cbIPAdress.SelectedIndex;
				m_hostname = client_params[selected_address].IPAdress;
				tbPort.Text = m_portno = client_params[selected_address].PortNo.ToString();
				cbAutoConnecct.Checked = client_params[selected_address].AutoConn;
				tbAttemptsToConnect.Text = client_params[selected_address].AttemptsToConnect.ToString();
			}
		}

		private void TimeoutChanged(object sender, EventArgs e)
		{
			if (tbAttemptsToConnect.Text != "")
			{
				client_params[selected_address].AttemptsToConnect = (int)int.Parse(tbAttemptsToConnect.Text);
				//AddMsg("timeout changed to:" + "    " + client_params[selected_address].AttemptsToConnect.ToString());
			}
		}

	}
}
 