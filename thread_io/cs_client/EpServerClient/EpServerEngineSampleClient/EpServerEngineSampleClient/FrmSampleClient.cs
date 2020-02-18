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
using System.Runtime.InteropServices;

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
        private DlgSetParams dlgsetparams = null;
        ServerCmds svrcmd = new ServerCmds();
		ServerCmds svrcmd2 = new ServerCmds();
		INetworkClient m_client = new IocpTcpClient();
		INetworkClient m_client2 = new IocpTcpClient();
		private string password = "";

		public System.Collections.Generic.List<ButtonList> button_list;
        private List<int> Exclude_From_buttons = new List<int>();
        private bool system_up = true;
        private PortSet2 psDlg = null;
        private PortSet2 psDlg2 = null;
        private PortSet2 psDlg3 = null;
        private PortSet2 psDlg4 = null;
        private PortSet2 psDlg5 = null;
        private PortSet2 psDlg6 = null;
        private PlayerDlg playdlg = null;
		private GPSForm gpsform = null;
        private Child_Scrolling_List slist = null;

        private List<ClientParams> client_params;
        private int current_button = 0;
        private int previous_button = 0;
        private int no_buttons = 0;
        private int i = 0;
        private int selected_address = 0;
        private int please_lets_disconnect = 0;
		private int disconnect_attempts = 0;
        private string m_hostname;
        private string m_portno;
        private bool m_engine_running = false;
        private int server_up_seconds = 0;
        //private string current_password = "testasdf";
        //private int password_length = 8;
        private int wait_before_starting = 0;
        //private int server_connection_attempts = 1;
        private bool client_connected = false;
		private bool home_svr_connected = false;
		// initially try for 20 seconds to connect, then 
		// give up until user hits 'Call Home' button
		private string timer_cmd = "SET_PARAMS";
		private int timer_offset;

        private string xml_dialog1_location = "c:\\Users\\daniel\\dev\\uiformat1.xml";
        private string xml_dialog2_location = "c:\\Users\\daniel\\dev\\uiformat2.xml";
        private string xml_dialog3_location = "c:\\Users\\daniel\\dev\\uiformat3.xml";
        private string xml_dialog4_location = "c:\\Users\\daniel\\dev\\uiformat4.xml";
        private string xml_dialog5_location = "c:\\Users\\daniel\\dev\\uiformat5.xml";
        private string xml_dialog6_location = "c:\\Users\\daniel\\dev\\uiformat6.xml";
        private string xml_params_location = "c:\\Users\\daniel\\dev\\ClientParams.xml";

        public FrmSampleClient()
        {
            InitializeComponent();
            button_list = new List<ButtonList>();
            //            use_main_odata = true;
            //this.conn = new System.Data.SqlClient.SqlConnection(connectionString);
            //            this.cmd = new System.Data.SqlClient.SqlCommand("UPDATE O_DATA SET label=@label WHERE port=@recno", conn);
            svrcmd.SetClient(m_client);
			svrcmd2.SetClient(m_client2);
			//dlgsetparams = new DlgSetParams(cfg_params);
            //dlgsetparams.SetClient(m_client);
            cbIPAdress.Enabled = true;
            tbReceived.Enabled = true;
            tbPort.Enabled = true;
            btnTestPorts.Enabled = true;    // these are normally set false
            btnStartEng.Enabled = false;
            btnStartEng.Text = "Start Engine";
            btnShutdown.Enabled = false;
            //btnReboot.Enabled = false;
            btnStopSerial.Enabled = false;
            btn_PlayList.Enabled = true;
            btnGetTime.Enabled = false;

            tbReceived.Clear();
            //psDlg = new PortSet2(File.Exists(dialog_one_location_laptop) ? dialog_one_location_laptop : dialog_one_location_desktop, m_client);
            psDlg = new PortSet2(xml_dialog1_location, m_client);
            psDlg.SetButtonLabels();
            //psDlg.Name = "Dialog One";
            //psDlg.Set_Type(false);

            //psDlg2 = new PortSet2(File.Exists(dialog_two_location_laptop) ? dialog_two_location_laptop : dialog_two_location_desktop, m_client);
            psDlg2 = new PortSet2(xml_dialog2_location, m_client);
            psDlg2.SetButtonLabels();
            //psDlg2.Name = "Dialog Two";
            //psDlg2.Set_Type(false);

            psDlg3 = new PortSet2(xml_dialog3_location, m_client);
            psDlg3.SetButtonLabels();
            psDlg3.Text = "test";
            //psDlg3.Name = "Test Ports";

            psDlg4 = new PortSet2(xml_dialog4_location, m_client);
            psDlg4.SetButtonLabels();
            //psDlg4.Name = "Dialog Three";

            psDlg5 = new PortSet2(xml_dialog5_location, m_client);
            psDlg5.SetButtonLabels();
            //psDlg5.Name = "Dialog Three";

            psDlg6 = new PortSet2(xml_dialog6_location, m_client);
            psDlg6.SetButtonLabels();
            //psDlg6.Name = "Dialog Three";

            playdlg = new PlayerDlg("c:\\users\\daniel\\dev\\player.xml", m_client);

			gpsform = new GPSForm("c:\\users\\daniel\\dev\\gps_list.xml",m_client);

			slist = new Child_Scrolling_List(m_client);
            slist.Enable_Dlg(false);

            client_params = new List<ClientParams>();
            ClientParams item = null;

            DataSet ds = new DataSet();
            //XmlReader xmlFile = XmlReader.Create(File.Exists(xml_file2_location_laptop) ? xml_file2_location_laptop : xml_file2_location_desktop);
            XmlReader xmlFile = XmlReader.Create(xml_params_location);
            ds.ReadXml(xmlFile);
            foreach (DataRow dr in ds.Tables[0].Rows)
            {
                item = new ClientParams();
                item.AutoConn = Convert.ToBoolean(dr.ItemArray[0]);
                item.IPAdress = dr.ItemArray[1].ToString();
                item.PortNo = Convert.ToUInt16(dr.ItemArray[2]);
                item.Primary = Convert.ToBoolean(dr.ItemArray[3]);
                item.AttemptsToConnect = Convert.ToInt16(dr.ItemArray[4]);
                client_params.Add(item);
                item = null;
            }

            bool found = false;
            i = 0;
            for (i = 0; i < client_params.Count(); i++)
            {

                //AddMsg(client_params[i].AutoConn.ToString() + " " + client_params[i].IPAdress + " " + client_params[i].PortNo.ToString() + " " + client_params[i].AttemptsToConnect.ToString());
                cbIPAdress.Items.Add(client_params[i].IPAdress);
                if (client_params[i].Primary)
                {
                    m_hostname = cbIPAdress.Text = client_params[i].IPAdress;
                    m_portno = tbPort.Text = client_params[i].PortNo.ToString();
                    selected_address = i;
                    found = true;
                }
            }
            if (!found)
            {
                AddMsg("no primary address found in xml file");
            }

            Control sCtl = this.btnStartEng;
            for (i = 0; i < this.Controls.Count; i++)
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
							AddMsg(btn.Name + " " + btn.TabOrder.ToString());
						}
			*/
            // Set the list of buttons to skip
            // over if in NAV mode (remote keypad)
            // some for obvious reasons, you don't want to 
            // execute the disconnect from server while
            // on the keypad or you have to open up the
            // laptop again and fix it

            Exclude_From_buttons.Add(10);
            Exclude_From_buttons.Add(11);
            Exclude_From_buttons.Add(12);
            Exclude_From_buttons.Add(13);
            Exclude_From_buttons.Add(14);
            Exclude_From_buttons.Add(15);

            no_buttons = 0;
            for (int j = 0; j < Exclude_From_buttons.Count(); j++)
            {
                for (i = 0; i < button_list.Count(); i++)
                {
                    if (button_list[i].TabOrder == Exclude_From_buttons[j])
                        button_list.RemoveAt(i);

                }
            }
            foreach (ButtonList btn in button_list)
            {
                no_buttons++;
                //AddMsg(btn.Name + " en: " + btn.Enabled.ToString());
            }
            timer1.Enabled = true;
			ListMsg("Hello!",true);
			//btnAVR_Click(new object(), new EventArgs());
			//AddMsg("buttons used: " + no_buttons.ToString());
			// start the connection to the home server by default
			//btnAVR_Click(new object(), new EventArgs());
			//AddMsg("connection timeout: " + m_client2.ConnectionTimeOut.ToString()); - this returns '0'
		}
        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (!client_connected)      // let's connect here! (see timer callback at end of file)
            {
                m_hostname = cbIPAdress.Items[selected_address].ToString();
                m_portno = tbPort.Text;
                AddMsg("trying to connect to:    " + m_hostname + ":" + m_portno.ToString() + "...");
                ClientOps ops = new ClientOps(this, m_hostname, m_portno);
				// set the timeout to 5ms - by default it's 0 which causes it to wait a long time
				// and slows down the UI
				ops.ConnectionTimeOut = 500;
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
			//AddMsg(client.HostName);
			if (client.HostName == m_hostname)
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

					btn_PlayList.Enabled = true;
					btnGetTime.Enabled = true;
					reevaluate_enabled_buttons();
				}
			}
			//else if (m_client2.IsConnectionAlive)
				//AddMsg("connection alive");

			//else AddMsg("home svr: " + client.HostName);
        }
        public void OnDisconnect(INetworkClient client)
        {
			if (client.HostName == m_hostname)
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
			}
		}
		protected override void OnClosed(EventArgs e)
        {
            if (m_client.IsConnectionAlive)
                please_lets_disconnect = 1;
            else
            {
                psDlg.Dispose();
                psDlg2.Dispose();
                psDlg3.Dispose();
                psDlg4.Dispose();
                psDlg5.Dispose();
                psDlg6.Dispose();
                playdlg.Dispose();
				gpsform.Dispose();
                base.OnClosed(e);
            }
        }
        public void OnReceived(INetworkClient client, Packet receivedPacket)
        {
			// anything that gets sent here gets sent to home server if it's up
			if (m_client2.IsConnectionAlive)
			{
				m_client2.Send(receivedPacket);
			}
			if (psDlg.Visible == true)
            {
                psDlg.Process_Msg(receivedPacket.PacketRaw);
            }
            else if (psDlg2.Visible == true)
            {
                psDlg2.Process_Msg(receivedPacket.PacketRaw);
            }
            else if (psDlg3.Visible == true)
            {
                psDlg3.Process_Msg(receivedPacket.PacketRaw);
            }
            else if (psDlg4.Visible == true)
            {
                psDlg4.Process_Msg(receivedPacket.PacketRaw);
            }
            else if (psDlg5.Visible == true)
            {
                psDlg5.Process_Msg(receivedPacket.PacketRaw);
            }
            else if (psDlg6.Visible == true)
            {
                psDlg6.Process_Msg(receivedPacket.PacketRaw);
            }
            else if (playdlg.Visible == true)
            {
                playdlg.Process_Msg(receivedPacket.PacketRaw);
            }
            else if (slist.Visible == true)
            {
                slist.Process_Msg(receivedPacket.PacketRaw);
            }
			else if (gpsform.Visible == true)
			{
				gpsform.Process_Msg(receivedPacket.PacketRaw);
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
            type_msg = chars[0];
            System.Buffer.BlockCopy(bytes, 2, chars2, 0, bytes.Length - 2);
            ret = new string(chars2);

            //            string str = Enum.GetName(typeof(msg_types), type_msg);
            string str = svrcmd.GetName(type_msg);
			//AddMsg(ret + " " + str + " " + type_msg.ToString() + bytes.Length.ToString());

            switch (str)
            {
				case "SEND_CONFIG2":
					AddMsg("send config2");
					cfg_params.rpm_update_rate = BitConverter.ToInt16(bytes, 2);
					cfg_params.mph_update_rate = BitConverter.ToInt16(bytes, 4);
					cfg_params.FPGAXmitRate = BitConverter.ToInt16(bytes, 6);
					cfg_params.high_rev_limit = BitConverter.ToInt16(bytes, 8);
					cfg_params.low_rev_limit= BitConverter.ToInt16(bytes, 10);
					cfg_params.fan_on = BitConverter.ToInt16(bytes, 12);
					cfg_params.fan_off = BitConverter.ToInt16(bytes, 14);
					cfg_params.blower_enabled = BitConverter.ToInt16(bytes, 16);
					cfg_params.blower1_on = BitConverter.ToInt16(bytes, 18);
					cfg_params.blower2_on = BitConverter.ToInt16(bytes, 20);
					cfg_params.blower3_on = BitConverter.ToInt16(bytes, 22);
					cfg_params.lights_on_delay = BitConverter.ToInt16(bytes, 24);
					cfg_params.engine_temp_limit = BitConverter.ToInt16(bytes, 26);
					cfg_params.battery_box_temp = BitConverter.ToInt16(bytes, 28);
					cfg_params.test_bank = BitConverter.ToInt16(bytes, 30);
					cfg_params.password_timeout = BitConverter.ToInt16(bytes, 32);
					cfg_params.password_retries = BitConverter.ToInt16(bytes, 34);
					// pass the same msg along to the linux server but just
					// change the cmd to "UPLOAD_CONFIG"
					bytes[0] = svrcmd.GetCmdIndexB("UPDATE_CONFIG");
					Packet packet = new Packet(bytes, 0, bytes.Count(), false);
					if (m_client.IsConnectionAlive)
					{
						m_client.Send(packet);
					}
					//AddMsg("hi rev: " + cfg_params.high_rev_limit.ToString());
					substr = dlgsetparams.get_temp_str(cfg_params.engine_temp_limit).ToString();
					AddMsg("temp limit: " + substr);
					AddMsg("");
					substr = dlgsetparams.get_temp_str(cfg_params.fan_on).ToString();
					AddMsg("fan on: " + substr);
					AddMsg("");
					substr = dlgsetparams.get_temp_str(cfg_params.fan_off).ToString();
					AddMsg("fan off: " + substr);
					AddMsg("");
					break;

				case "SVR_CMD":
					//AddMsg("str: " + str.Length.ToString());
					//AddMsg(str);
					//AddMsg("ret: " + ret.Length.ToString());
					int offset = svrcmd.GetCmdIndexI(ret);
					svrcmd.Send_Cmd(offset);
					break;

				case "SEND_MSG":
					//AddMsg("str: " + str + " " + str.Length.ToString());
					//AddMsg(ret + " " + str + " " + type_msg.ToString() + bytes.Length.ToString());
					Send_Svr_Cmd(ret);
					ListMsg(ret,false);
					switch (ret)
                    {
                        case "START_SEQ":
                            m_engine_running = true;
                            btnStartEng.Text = "Stop Engine";
                            IgnitionOnLabel.BackColor = Color.Aqua;
                            break;
                        case "SHUTDOWN":
                            m_engine_running = false;
                            tbEngRunTime.Text = "";
                            tbEngineTemp.Text = "";
                            //tbServerTime.Text = "";
                            tbRPM.Text = "0";
                            tbMPH.Text = "0";
                            btnStartEng.Text = "Start Engine";
                            IgnitionOnLabel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
                            reevaluate_enabled_buttons();
                            break;
                        case "ON_FAN":
                            CoolingFanLabel.BackColor = Color.Aqua;
                            break;
                        case "OFF_FAN":
                            CoolingFanLabel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
                            break;
                        case "ON_LIGHTS":
                            LightsOnLabel.BackColor = Color.Aqua;
                            break;
                        case "OFF_LIGHTS":
                            LightsOnLabel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
                            break;
                        case "ON_BRIGHTS":
                            BrightsLabel.BackColor = Color.Aqua;
                            break;
                        case "OFF_BRIGHTS":
                            BrightsLabel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
                            break;
                        case "ON_BRAKES":
                            BrakeLightsLabel.BackColor = Color.Aqua;
                            break;
                        case "OFF_BRAKES":
                            BrakeLightsLabel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
                            break;
                        case "ON_RUNNING_LIGHTS":
                            SideMarkerLabel.BackColor = Color.Aqua;
                            break;
                        case "OFF_RUNNING_LIGHTS":
                            SideMarkerLabel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
                            break;
                        default:
                            //AddMsg(str);
                            break;
                    }
                    break;

                case "CURRENT_TIME":
                    ListMsg(ret,true);
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

					if (server_up_seconds == 2)
						SetTime();

					if (client_params[selected_address].AutoConn == true && server_up_seconds == 4)
					{
						if (dlgsetparams == null)
						{
							// call the SET_PARAMS of the linux server
							// to send the param.conf data back to SEND_CONFIG
							timer_offset = svrcmd.GetCmdIndexI(timer_cmd);
							svrcmd.Send_Cmd(timer_offset);
							timer_cmd = "NEW_PASSWORD1";
							timer_offset = svrcmd.GetCmdIndexI(timer_cmd);
							svrcmd.Send_Cmd(timer_offset);
						}
						else btnStopSerial.Enabled = true;
					}
					if (client_params[selected_address].AutoConn == true && server_up_seconds == 6)
					{
						if(dlgsetparams == null)
						{
							AddMsg(cfg_params.engine_temp_limit.ToString());
							dlgsetparams = new DlgSetParams(cfg_params);
							dlgsetparams.SetParams(cfg_params);
							dlgsetparams.SetClient(m_client);
							//AddMsg(cfg_params.engine_temp_limit.ToString());
							//dlgsetparams.SetParams(cfg_params);
							//AddMsg("cfg_params in dlgsetparams set: " + dlgsetparams.GetSet());
							btnStopSerial.Enabled = true;
						}
					}
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

				case "INDOOR_TEMP":
					tbIndoorTemp.Text = ret;
					break;

				case "SEND_RPM":
                    tbRPM.Text = ret;           // comment out for debug
                    break;

                case "SEND_MPH":
                    tbMPH.Text = ret;           // comment out for debug
                    break;

				case "SEND_ADCS1":
					string[] words2 = ret.Split(' ');
					i = 0;
					foreach(var word in words2)
					{
						switch(i)
						{
							case 0:
								tbADC1.Text = word;
								break;
							case 1:
								tbADC2.Text = word;
								break;
							case 2:
								tbADC3.Text = word;
								break;
							case 3:
								tbADC4.Text = word;
								break;
							default:
								break;
						}
						i++;
					}
					break;

                case "SEND_CONFIG":
                    string[] words = ret.Split(' ');
                    i = 0;
					AddMsg("send config");
					//AddMsg(ret);
					
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
								AddMsg("hi rev: " + cfg_params.high_rev_limit.ToString());
                                break;
                            case 4:
                                cfg_params.low_rev_limit = int.Parse(word);
								AddMsg("lo rev: " + cfg_params.low_rev_limit.ToString());
								break;
                            case 5:
                                cfg_params.fan_on = int.Parse(word);
                                AddMsg("fan on: " + cfg_params.fan_on.ToString());
                                //substr = dlgsetparams.get_temp_str(cfg_params.fan_on).ToString();
                                //AddMsg("fan on: " + substr);
                                //AddMsg("");
                                break;
                            case 6:
                                cfg_params.fan_off = int.Parse(word);
                                AddMsg("fan off: " + cfg_params.fan_off.ToString());
                                //substr = dlgsetparams.get_temp_str(cfg_params.fan_off).ToString();
                                //AddMsg("fan off: " + substr);
                                //AddMsg("");
                                break;
                            case 7:
                                cfg_params.blower_enabled = int.Parse(word);
								AddMsg("blower en: " + cfg_params.blower_enabled.ToString());
								//substr = dlgsetparams.get_temp_str(cfg_params.blower_enabled).ToString();
								//AddMsg("blower en: " + substr);
								//AddMsg("");
								break;
                            case 8:
                                cfg_params.blower1_on = int.Parse(word);
								AddMsg("blower 1: " + cfg_params.blower1_on.ToString());
								//substr = dlgsetparams.get_temp_str(cfg_params.blower1_on).ToString();
								//AddMsg("blower1 on: " + substr);
								//AddMsg("");
								break;
                            case 9:
                                cfg_params.blower2_on = int.Parse(word);
								AddMsg("blower 2: " + cfg_params.blower2_on.ToString());
								//substr = dlgsetparams.get_temp_str(cfg_params.blower2_on).ToString();
								//AddMsg("blower2 on: " + substr);
								//AddMsg("");
								break;
                            case 10:
                                cfg_params.blower3_on = int.Parse(word);
								AddMsg("blower 3: " + cfg_params.blower3_on.ToString());
								//substr = dlgsetparams.get_temp_str(cfg_params.blower3_on).ToString();
								//AddMsg("blower3 on: " + substr);
								//AddMsg("");
								break;
                            case 11:
                                cfg_params.lights_on_delay = int.Parse(word);
                                AddMsg("lights on delay: " + cfg_params.lights_on_delay.ToString());
                                break;
                            case 12:
                                cfg_params.engine_temp_limit = int.Parse(word);
                                AddMsg("temp limit: " + cfg_params.engine_temp_limit.ToString());
                                //substr = dlgsetparams.get_temp_str(cfg_params.engine_temp_limit).ToString();
                                //AddMsg("temp limit: " + substr);
                                //AddMsg("");
                                break;
                            case 13:
                                cfg_params.battery_box_temp = int.Parse(word);
								//substr = dlgsetparams.get_temp_str(cfg_params.battery_box_temp).ToString();
								//AddMsg("battery box temp: " + substr);
								//AddMsg("");
								break;
                            case 14:
                                cfg_params.test_bank = int.Parse(word);
                                break;
							case 15:
								cfg_params.password_timeout = int.Parse(word);
								//AddMsg("pswd timeout: " + cfg_params.password_timeout.ToString());
								break;
							case 16:
								cfg_params.password_retries = int.Parse(word);
								//AddMsg("pswd retries: " + cfg_params.password_retries.ToString());
								break;

							case 17:
								cfg_params.baudrate3 = int.Parse(word);
								AddMsg("baudrate3: " + cfg_params.baudrate3.ToString());
								break;

							case 18:
								//AddMsg(word);
								cfg_params.password = word;
								AddMsg("password: " + cfg_params.password);
								break;
								
                            default:
                                break;
                        }
                        //                        MessageBox.Show(int.Parse(word).ToString());
                        i++;
                    }
					break;

                case "GET_TIME":
					ListMsg(ret,true);
                    break;

                case "SYSTEM_UP":
					ListMsg(ret,true);
                    break;

                case "SYSTEM_DOWN":
                    ListMsg(ret,true);
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
                    m_engine_running = false;
                    break;

                case "NAV_NUM":
                    AddMsg(ret);
                    break;

                case "SEND_STATUS":
                    AddMsg(ret);
                    break;

				case "NEW_PASSWORD1":
					password = ret;
					AddMsg(password);
					break;

				default:
                    break;
            }
        }
		/*
        void ProcessIOnums(byte[] bytes)
        {
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
		*/
        byte[] BytesFromString(String str)
        {
            byte[] bytes = new byte[str.Length * sizeof(char)];
            System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
            return bytes;
        }
        public void OnSent(INetworkClient client, SendStatus status, Packet sentPacket)
        {
			if (client == m_client2)
			{
				if (status != SendStatus.SUCCESS)
				{
					AddMsg("status: home svr not success");
					SetHomeSvrStatus(false);
				}else
				{
					if (!home_svr_connected)
					{
						SetHomeSvrStatus(true);
						AddMsg("home server success");
					}
				}
			}
			else
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
        // start/stop engine
        private void StartEng_Click(object sender, EventArgs e)
        {
            if (!m_engine_running)
            {
                // because of the way the database on the server works,
                // we can't try and restart it after doing a shutdown
                // within 10 seconds of doing a starter enable
                if (wait_before_starting < 11 && wait_before_starting != 0)
                    AddMsg("wait " + wait_before_starting.ToString() + " sec");
                else
                {
                    wait_before_starting = 8;
                    string cmd = "START_SEQ";
                    //AddMsg("start seq: " + cmd);
                    int offset = svrcmd.GetCmdIndexI(cmd);
                    svrcmd.Send_Cmd(offset);
                }
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
        private void ShutdownServer(object sender, EventArgs e)
        {
            ManageServer dlg = new ManageServer(m_client);
            if (dlg.ShowDialog(this) == DialogResult.OK)
            {
				if(client_params[selected_address].AutoConn == true)
					cfg_params = dlgsetparams.GetParams();
				please_lets_disconnect = 1;
            }
            else
            {
            }
        }
        private void RebootServer(object sender, EventArgs e)
        {
			string cmd = "REFRESH_LCD";
			int offset = svrcmd.GetCmdIndexI(cmd);
			svrcmd.Send_Cmd(offset);
		}
        private void StopMbox(object sender, EventArgs e)
        {
			//AddMsg(dlgsetparams.GetSet().ToString());
			if (m_client.IsConnectionAlive)
			{
				if (dlgsetparams.ShowDialog(this) == DialogResult.OK)
				{
					AddMsg("new password: " + cfg_params.password);
					cfg_params = dlgsetparams.GetParams();
					byte[] rpm = BitConverter.GetBytes(cfg_params.si_rpm_update_rate);
					byte[] mph = BitConverter.GetBytes(cfg_params.si_mph_update_rate);
					byte[] fpga = BitConverter.GetBytes(cfg_params.si_FPGAXmitRate);
					byte[] high_rev = BitConverter.GetBytes(cfg_params.si_high_rev_limit);
					byte[] low_rev = BitConverter.GetBytes(cfg_params.si_low_rev_limit);
					byte[] fan_on = BitConverter.GetBytes(cfg_params.fan_on);
					byte[] fan_off = BitConverter.GetBytes(cfg_params.fan_off);
					byte[] ben = BitConverter.GetBytes(cfg_params.blower_enabled);
					byte[] b1 = BitConverter.GetBytes(cfg_params.blower1_on);
					byte[] b2 = BitConverter.GetBytes(cfg_params.blower2_on);
					byte[] b3 = BitConverter.GetBytes(cfg_params.blower3_on);
					byte[] lights = BitConverter.GetBytes(cfg_params.si_lights_on_delay);
					byte[] limit = BitConverter.GetBytes(cfg_params.engine_temp_limit);
					byte[] batt = BitConverter.GetBytes(cfg_params.battery_box_temp);
					byte[] test = BitConverter.GetBytes(cfg_params.si_test_bank);
					byte[] pswd_time = BitConverter.GetBytes(cfg_params.si_password_timeout);
					byte[] pswd_retries = BitConverter.GetBytes(cfg_params.si_password_retries);
					byte[] baudrate3 = BitConverter.GetBytes(cfg_params.si_baudrate3);
					byte[] password = BytesFromString(cfg_params.password);

					byte[] bytes = new byte[rpm.Count() + mph.Count() + fpga.Count() + high_rev.Count()
						+ low_rev.Count() + fan_on.Count() + fan_off.Count() + ben.Count() + b1.Count()
						+ b2.Count() + b3.Count() + lights.Count() + limit.Count() + batt.Count()
						+ test.Count() + pswd_time.Count() + pswd_retries.Count() + baudrate3.Count() + password.Count() + 2];

					bytes[0] = svrcmd.GetCmdIndexB("UPDATE_CONFIG");
					//System.Buffer.BlockCopy(src, src_offset, dest, dest_offset,count)
					System.Buffer.BlockCopy(rpm, 0, bytes, 2, rpm.Count());
					System.Buffer.BlockCopy(mph, 0, bytes, 4, mph.Count());
					System.Buffer.BlockCopy(fpga, 0, bytes, 6, fpga.Count());
					System.Buffer.BlockCopy(high_rev, 0, bytes, 8, high_rev.Count());
					System.Buffer.BlockCopy(low_rev, 0, bytes, 10, low_rev.Count());
					System.Buffer.BlockCopy(fan_on, 0, bytes, 12, fan_on.Count());
					System.Buffer.BlockCopy(fan_off, 0, bytes, 14, fan_off.Count());
					System.Buffer.BlockCopy(ben, 0, bytes, 16, ben.Count());
					System.Buffer.BlockCopy(b1, 0, bytes, 18, b1.Count());
					System.Buffer.BlockCopy(b2, 0, bytes, 20, b2.Count());
					System.Buffer.BlockCopy(b3, 0, bytes, 22, b3.Count());
					System.Buffer.BlockCopy(lights, 0, bytes, 24, lights.Count());
					System.Buffer.BlockCopy(limit, 0, bytes, 26, limit.Count());
					System.Buffer.BlockCopy(batt, 0, bytes, 28, batt.Count());
					System.Buffer.BlockCopy(test, 0, bytes, 30, test.Count());
					System.Buffer.BlockCopy(pswd_time, 0, bytes, 32, pswd_time.Count());
					System.Buffer.BlockCopy(pswd_retries, 0, bytes, 34, pswd_retries.Count());
					System.Buffer.BlockCopy(baudrate3, 0, bytes, 36, baudrate3.Count());
					System.Buffer.BlockCopy(password, 0, bytes, 38, password.Count());

					Packet packet = new Packet(bytes, 0, bytes.Count(), false);
					m_client.Send(packet);
				}
			}
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
        private void DBMgmt(object sender, EventArgs e)
        {
			byte[] param = new byte[4];
			byte[] bytes = new byte[6];
			gpsform.Enable_Dlg(true);
			gpsform.StartPosition = FormStartPosition.Manual;
			gpsform.Location = new Point(100, 10);
			param = BitConverter.GetBytes(1);   // turn gps sending on
												//System.Buffer.BlockCopy(src, src_offset, dest, dest_offset,count)
			AddMsg(param.Length.ToString() + " " + bytes.Length.ToString());
			System.Buffer.BlockCopy(param, 0, bytes, 2, param.Count());
			bytes[0] = svrcmd.GetCmdIndexB("ENABLE_GPS_SEND_DATA");
			Packet packet = new Packet(bytes, 0, bytes.Count(), false);
			if (m_client.IsConnectionAlive)
			{
				m_client.Send(packet);
			}
			if (gpsform.ShowDialog(this) == DialogResult.OK)
			{
			}
			else
			{
			}
			gpsform.Enable_Dlg(false);
			param = BitConverter.GetBytes(0);// turn gps sending off
			System.Buffer.BlockCopy(param, 0, bytes, 2, param.Count());
			bytes[0] = svrcmd.GetCmdIndexB("ENABLE_GPS_SEND_DATA");
			packet = new Packet(bytes, 0, bytes.Count(), false);
			if (m_client.IsConnectionAlive)
			{
				m_client.Send(packet);
			}
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
            cmd = "GET_VERSION";
            offset = svrcmd.GetCmdIndexI(cmd);
            svrcmd.Send_Cmd(offset);
        }
        private void SetParamsClick(object sender, EventArgs e)
        {
			/*
			dlgsetparams = new DlgSetParams(cfg_params);
			dlgsetparams.SetParams(cfg_params);
			dlgsetparams.SetClient(m_client);
			AddMsg(cfg_params.engine_temp_limit.ToString());
			dlgsetparams.ShowDialog(this);
			*/
			int offset = svrcmd.GetCmdIndexI("SERVER_UP");
			svrcmd.Send_Cmd(offset);
		}
		private void btnAVR_Click(object sender, EventArgs e)
        {
			if (!home_svr_connected)
			{
				ClientOps ops = new ClientOps(this, "192.168.42.150", "8000");
				ops.ConnectionTimeOut = 5;
				m_client2.Connect(ops);
				//AddMsg("initial connect to home svr " + m_client2.IsConnectionAlive.ToString());
				SetHomeSvrStatus(true);
			}
			else
			{
				SetHomeSvrStatus(false);
				m_client2.Disconnect();
			}
			/*
			psDlg4.Enable_Dlg(true);
            psDlg4.StartPosition = FormStartPosition.Manual;
            psDlg4.Location = new Point(100, 10);
            psDlg4.ShowDialog(this);
            psDlg.Enable_Dlg(false);
			*/
		}
        private void Dialog1_Click(object sender, EventArgs e)
        {
            psDlg.Enable_Dlg(true);
            psDlg.StartPosition = FormStartPosition.Manual;
            psDlg.Location = new Point(100, 10);
            psDlg.ShowDialog(this);
            psDlg.Enable_Dlg(false);
        }
        private void Dialog2_Click(object sender, EventArgs e)
        {
            psDlg2.Name = "Dialog Two";
            psDlg2.Enable_Dlg(true);
            psDlg2.StartPosition = FormStartPosition.Manual;
            psDlg2.Location = new Point(100, 10);
            psDlg2.ShowDialog(this);
            psDlg2.Enable_Dlg(false);
        }
        private void Dialog3_Click(object sender, EventArgs e)
        {
            psDlg.Name = "Dialog One";
            psDlg.Enable_Dlg(true);
            psDlg.StartPosition = FormStartPosition.Manual;
            psDlg.Location = new Point(100, 10);
            psDlg.ShowDialog(this);
            psDlg.Enable_Dlg(false);
        }
        private void Dialog4_Click(object sender, EventArgs e)
        {
        }
        private void TestPorts_Click(object sender, EventArgs e)
        {
            psDlg3.Enable_Dlg(true);
            psDlg3.StartPosition = FormStartPosition.Manual;
            psDlg3.Location = new Point(100, 10);
            psDlg3.ShowDialog(this);
            psDlg3.Enable_Dlg(false);
        }
        private void button2_Click(object sender, EventArgs e)
        {
            //AddMsg(psDlg5.Name);
            psDlg5.Enable_Dlg(true);
            psDlg5.StartPosition = FormStartPosition.Manual;
            psDlg5.Location = new Point(100, 10);
            psDlg5.ShowDialog(this);
            psDlg5.Enable_Dlg(false);
            //AddMsg(psDlg5.Name);
            //AddMsg(psDlg5.Text);
        }
        private void btnSettingsFour_Click(object sender, EventArgs e)
        {
            psDlg6.Enable_Dlg(true);
            psDlg6.StartPosition = FormStartPosition.Manual;
            psDlg6.Location = new Point(100, 10);
            psDlg6.ShowDialog(this);
            psDlg6.Enable_Dlg(false);
        }
        private void reevaluate_enabled_buttons()
        {
            foreach (ButtonList btn in button_list)
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
                    //AddMsg("down  " + button_list[current_button].Name + " " 
                    //+ current_button.ToString() + " " + previous_button.ToString() + " " + button_list[current_button].TabOrder.ToString());
                    break;

                case "NAV_SIDE":
                    previous_button = current_button;
                    if (current_button > 4)
                        current_button -= 5;
                    else if (current_button < 5)
                        current_button += 5;
                    button_list[current_button].Ctl.BackColor = Color.Aqua;
                    button_list[previous_button].Ctl.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
                    //AddMsg("up  " + button_list[current_button].Name + " " + button_list[current_button].TabOrder.ToString());
                    //AddMsg("side  " + button_list[current_button].Name + " "
                    //+ current_button.ToString() + " " + previous_button.ToString() + " " + button_list[current_button].TabOrder.ToString());
                    //AddMsg("NAV_SIDE not working yet");
                    break;
                case "NAV_CLICK":
                    //if (button_list[current_button].Enabled && button_list[current_button].Name != "Stop Mbox Xmit" && 
                    //button_list[current_button].Name != "Connect Server")
                    if (button_list[current_button].Enabled)
                    {
                        int tab_order = button_list[current_button].TabOrder;
                        // don't do a shutdown, reboot, disconnect or stop mbox xmit while using keypad!
                        // it's like sawing off the branch your sitting on
                        // this should be taken care of by the exclude list
                        //if (tab_order != 0 && tab_order != 9 && tab_order != 7 && tab_order != 8)
                        {
                            button_list[current_button].Ctl.PerformClick();
                            //AddMsg("click  " + button_list[current_button].Name + " "
                            //+ button_list[current_button].TabOrder.ToString());

                        }
                        //else AddMsg("don't do this!");
                    }
                    else AddMsg("Not Enabled");
                    break;
            }
        }
        int drake = 0;
        private void myTimerTick(object sender, EventArgs e)
        {
			if (wait_before_starting != 0)
                wait_before_starting--;

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
                            btn_PlayList.Enabled = true;
                            btnGetTime.Enabled = true;
                            AddMsg("server connected");
                            // here we should get a message from the server
                            // data that tells us if the engine is already running
                            // or the lights are already on,etc.
                        }
                        else
                        {
							//AddMsg(server_up_seconds.ToString());
                            //tbServerUpTimeSeconds.Text = "server up seconds: " + 
                            //timer_server_up_seconds.ToString() + " " 
                            //+ "previous up seconds: " + previous_timer_server_up_seconds.ToString();
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
                            btn_PlayList.Enabled = false;
                            btnGetTime.Enabled = false;
                            //Upload_New.Enabled = false;

                            tbEngRunTime.Text = "";         /// comment the next 4 out in debug
							tbEngineTemp.Text = "";
                            tbRPM.Text = "";
                            tbMPH.Text = "";
                            tbServerTime.Text = "";
                            client_connected = false;
                            ListMsg("server disconnected",true);
							//tbServerUpTimeSeconds.Text = "";
							server_up_seconds = 0;
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
                                    btn_PlayList.Enabled = false;
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
                    please_lets_disconnect = 2;     // next time around disconnect anyway
                    break;
                case 2:         // then wait 1 second to see if it really did disconnect us
                    System.Media.SoundPlayer player;
                    string song = "";
                    switch (drake)
                    {
						case 0:
							AddMsg("killing aliens...");
							drake = 1;
							song = "c:\\users\\Daniel\\Music\\WavFiles\\alien_kill2.wav";
						break;
						case 1:
							AddMsg("Game over man...");
							drake = 2;
							song = "c:\\users\\Daniel\\Music\\WavFiles\\GameOverMan.wav";
							break;
						case 2:
							AddMsg("Drake, we are leaving...");
							drake = 0;
							song = "c:\\users\\Daniel\\Music\\WavFiles\\DRAKE.wav";
							break;
						default:
							break;
					}
					if (m_client.IsConnectionAlive)
                        m_client.Disconnect();
                    please_lets_disconnect = 0;
                    //drake_img.Visible = true;
                    player = new System.Media.SoundPlayer();
                    player.SoundLocation = song;
                    player.Play();
                    player.Dispose();
                    //drake_img.Visible = false;
                    break;
            }

//			if (giveup_home_svr > 0)
			//if(!home_svr_connected)
			/*
			if(true)
			{

				if (connection_timeout > 2 || !home_svr_connected)
				{
					tbHomeSvrConnAttempts.Visible = true;
					lbHomeSvrConnAttempts.Visible = true;
					tbHomeSvrConnAttempts.Text = (connection_timeout - 2).ToString() + " " + connection_timeout.ToString();
					btnHomeSvr.Text = "Call Home";
					if (!home_svr_connected)
					{
						ClientOps ops = new ClientOps(this, "192.168.42.150", "8000");
						ops.ConnectionTimeOut = 2000;
						m_client2.Connect(ops);
					}
					if (--giveup_home_svr < 1)
					{
						connection_timeout = 0;
						tbHomeSvrConnAttempts.Visible = false;
						lbHomeSvrConnAttempts.Visible = false;
						//AddMsg("give up home svr");
					}
					if (giveup_home_svr == 1)
						lbHomeSvrConnAttempts.Text = "giving up";
					//AddMsg(giveup_home_svr.ToString());
				}
				else
				{
					tbHomeSvrConnAttempts.Visible = false;
					lbHomeSvrConnAttempts.Visible = false;
					btnHomeSvr.Text = "Hangup Home";
				}
				if(!home_svr_connected)
					connection_timeout++;
			}
			*/
			Send_Svr_Cmd("CLIENT_CONNECTED");
		}
		private void SetHomeSvrStatus(bool logged_in)
		{
			if (logged_in)
			{
				home_svr_connected = true;
				//AddMsg("avr svr connect");
				btnHomeSvr.Text = "Hangup Home";
			}
			else
			{
				home_svr_connected = false;
				btnHomeSvr.Text = "Call Home";
				//AddMsg("avr svr disconnect");
			}

		}
		private void Send_Svr_Cmd(string cmd2)
		{
			if (m_client2.IsConnectionAlive)
			{
				byte[] bytes1;
				string cmd = "SEND_MSG";
				bytes1 = BytesFromString(cmd2);
				byte[] bytes = new byte[bytes1.Count() - 1 + 2];
				bytes[0] = svrcmd.GetCmdIndexB(cmd);
				System.Buffer.BlockCopy(bytes1, 0, bytes, 2, bytes1.Count() - 1);
				Packet packet = new Packet(bytes, 0, bytes.Count(), false);
				m_client2.Send(packet);
			}
		}
		private void IPAddressChanged(object sender, EventArgs e)
        {
            if (!client_connected)
            {
                selected_address = cbIPAdress.SelectedIndex;
                m_hostname = client_params[selected_address].IPAdress;
                tbPort.Text = m_portno = client_params[selected_address].PortNo.ToString();
            }
        }
        private void FrmSampleClient_Load(object sender, EventArgs e)
        {
            this.StartPosition = FormStartPosition.Manual;
            this.Location = new Point(100, 10);
        }
        private void Btn_PlayList_Click(object sender, EventArgs e)
        {
            playdlg.Enable_Dlg(true);

            playdlg.StartPosition = FormStartPosition.Manual;
            playdlg.Location = new Point(100, 10);

            if (playdlg.ShowDialog(this) == DialogResult.OK)
            {
            }
            else
            {
                //                this.txtResult.Text = "Cancelled";
            }
            playdlg.Enable_Dlg(false);
        }
        private void SetTime()
        {
			if (m_client.IsConnectionAlive)
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
			}
        }
		private void ListMsg(string msg, bool show_date)
		{
			string temp = "";
			DateTime localDate = DateTime.Now;
			String cultureName = "en-US";
			var culture = new CultureInfo(cultureName);
			temp = localDate.ToString(culture);
			if (show_date)
				AddMsg(msg + " " + temp);
			else
			{
				int index = temp.IndexOf(' ');
				//AddMsg(index.ToString());
				temp = temp.Substring(index);
				AddMsg(msg + " " + temp);
			}
		}
	}
}