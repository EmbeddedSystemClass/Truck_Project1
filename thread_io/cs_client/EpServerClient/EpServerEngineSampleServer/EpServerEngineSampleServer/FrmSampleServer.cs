using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EpServerEngine.cs;
using System.Diagnostics;

namespace EpServerEngineSampleServer
{
    public partial class FrmSampleServer : Form, INetworkServerAcceptor, INetworkServerCallback, INetworkSocketCallback
    {
		private INetworkServer m_server = null;
		private ServerCmds svrcmd = null;
		private ParamsForm params_form = null;
		private ConfigParams cfg_params = null;
		int server_up_seconds = 0;
		bool lights, running_lights, cooling_fan, brights;
		int blower, wipers;
		
		public FrmSampleServer()
        {
            InitializeComponent();
			lights = running_lights = cooling_fan = brights = false;
			blower = wipers = 0;
			m_server = new IocpTcpServer();
			svrcmd = new ServerCmds();
			cfg_params = new ConfigParams();
			cfg_params.set = false;
			params_form = new ParamsForm(cfg_params);
			btnConnect_Click(new object(), new EventArgs());
		}

		private void btnConnect_Click(object sender, EventArgs e)
        {
            if (btnConnect.Text.Equals("Start"))
            {
                tbPort.Enabled = false;
                btnConnect.Text = "Stop";
				string port = tbPort.Text;
				ServerOps ops = new ServerOps(this, port,this);
                m_server.StartServer(ops);
            }
            else
            {
                tbPort.Enabled = true;
                btnConnect.Text = "Start";
				if (m_server.IsServerStarted)
                    m_server.StopServer();
            }

        }
        public void OnServerStarted(INetworkServer server, StartStatus status)
        {
            if (status == StartStatus.FAIL_ALREADY_STARTED || status == StartStatus.SUCCESS)
            {
            }
            else
                MessageBox.Show("Unknown Error occurred");
            
        }

        public bool OnAccept(INetworkServer server, IPInfo ipInfo)
        {
            return true;
        }

        public INetworkSocketCallback GetSocketCallback()
        {
            return this;
        }

        public void OnServerAccepted(INetworkServer server, INetworkSocket socket)
        {
        }
        public void OnServerStopped(INetworkServer server)
        {
        }

        List<INetworkSocket> m_socketList = new List<INetworkSocket>();
        public void OnNewConnection(INetworkSocket socket)
        {
            m_socketList.Add(socket);
            String sendString = "** New user(" + socket.IPInfo.IPAddress + ") connected!";
            AddMsg(sendString);
			tbPort.Enabled = false;
			btnConnect.Text = "Stop";
			//tbSend.Enabled = true;
			//btnSend.Enabled = true;
			btnLights.Enabled = true;
			btnFan.Enabled = true;
			btnRunningLights.Enabled = true;
			btnBlower.Enabled = true;
			btnWiper.Enabled = true;
			btnSpecial.Enabled = true;
			//btnShutdown.Enabled = true;

			/*
			byte[] sendBuff = BytesFromString(sendString);

            foreach (var socketObj in m_socketList)
            {
                if (socketObj != socket)
                {
                    socketObj.Send(new Packet(sendBuff,0, sendBuff.Count(), false));
                }
            }
			*/
        }

        public void OnReceived(INetworkSocket socket, Packet receivedPacket)
        {
            string sendString = socket.IPInfo.IPAddress;
            //AddMsg(sendString);
			Process_Msg(receivedPacket.PacketRaw);
			/*
            foreach (var socketObj in m_socketList)
            {
                if (socketObj != socket)
                {
                    socketObj.Send(receivedPacket);
                }
            }
			*/
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
			
			//AddMsg(svrcmd.GetName(type_msg) + " " + type_msg.ToString());
			System.Buffer.BlockCopy(bytes, 2, chars2, 0, bytes.Length - 2);
			ret = new string(chars2);
			string str = svrcmd.GetName(type_msg);
			//AddMsg(ret + " " + str + " " + type_msg.ToString() + bytes.Length.ToString());

			switch (str)
			{
				case "SEND_MSG":
					//AddMsg(str);
					AddMsg(ret);
					switch (ret)
					{
						case "START_SEQ":
							btnShutdown.Enabled = true;
							break;
						case "SHUTDOWN":
							btnShutdown.Enabled = false;
							break;
						case "ON_FAN":
							break;
						case "OFF_FAN":
							break;
						case "ON_LIGHTS":
							break;
						case "OFF_LIGHTS":
							break;
						case "ON_BRIGHTS":
							break;
						case "OFF_BRIGHTS":
							break;
						case "ON_BRAKES":
							break;
						case "OFF_BRAKES":
							break;
						case "ON_RUNNING_LIGHTS":
							break;
						case "OFF_RUNNING_LIGHTS":
							break;
						default:
							//AddMsg(str);
							break;
					}
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
					//tbRPM.Text = ret;           // comment out for debug
					break;
				case "SEND_MPH":
					//tbMPH.Text = ret;           // comment out for debug
					break;
				case "SEND_CONFIG":
					string[] words = ret.Split(' ');
					i = 0;
					//AddMsg(ret);
					foreach (var word in words)
					{
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
								//AddMsg("fan on: " + cfg_params.fan_on.ToString());
								substr = params_form.get_temp_str(cfg_params.fan_on).ToString();
								AddMsg("fan on: " + substr);
								AddMsg("");
								break;
							case 6:
								cfg_params.fan_off = int.Parse(word);
								//AddMsg("fan off: " + cfg_params.fan_off.ToString());
								substr = params_form.get_temp_str(cfg_params.fan_off).ToString();
								AddMsg("fan off: " + substr);
								AddMsg("");
								break;
							case 7:
								cfg_params.blower_enabled = int.Parse(word);
								substr = params_form.get_temp_str(cfg_params.blower_enabled).ToString();
								AddMsg("blower en: " + substr);
								AddMsg("");
								break;
							case 8:
								cfg_params.blower1_on = int.Parse(word);
								substr = params_form.get_temp_str(cfg_params.blower1_on).ToString();
								AddMsg("blower1 on: " + substr);
								AddMsg("");
								break;
							case 9:
								cfg_params.blower2_on = int.Parse(word);
								substr = params_form.get_temp_str(cfg_params.blower2_on).ToString();
								AddMsg("blower2 on: " + substr);
								AddMsg("");
								break;
							case 10:
								cfg_params.blower3_on = int.Parse(word);
								substr = params_form.get_temp_str(cfg_params.blower3_on).ToString();
								AddMsg("blower3 on: " + substr);
								AddMsg("");
								break;
							case 11:
								cfg_params.lights_on_delay = int.Parse(word);
								AddMsg("lights on delay: " + cfg_params.lights_on_delay.ToString());
								break;
							case 12:
								cfg_params.engine_temp_limit = int.Parse(word);
								//AddMsg("temp limit: " + cfg_params.engine_temp_limit.ToString());
								substr = params_form.get_temp_str(cfg_params.engine_temp_limit).ToString();
								AddMsg("temp limit: " + substr);
								AddMsg("");
								break;
							case 13:
								cfg_params.battery_box_temp = int.Parse(word);
								substr = params_form.get_temp_str(cfg_params.battery_box_temp).ToString();
								AddMsg("battery box temp: " + substr);
								AddMsg("");
								break;
							case 14:
								cfg_params.test_bank = int.Parse(word);
								break;
							default:
								break;
						}
						i++;
					}
					//params_form.Set_Params(cfg_params);
					break;
				case "GET_TIME":
					AddMsg(ret);
					break;

				case "SYSTEM_UP":
					AddMsg(ret);
					break;

				case "SYSTEM_DOWN":
					AddMsg(ret);
					break;

				case "NAV_UP":
				case "NAV_DOWN":
				case "NAV_SIDE":
				case "NAV_CLICK":
				case "NAV_CLOSE":
					break;

				case "ESTOP_SIGNAL":
					AddMsg("ESTOP: " + ret);
					break;

				case "NAV_NUM":
					AddMsg(ret);
					break;

				case "SEND_STATUS":
					AddMsg(ret);
					break;

				default:
					break;
			}
		}

		public void OnSent(INetworkSocket socket, SendStatus status, Packet sentPacket)
        {
            switch (status)
            {
                case SendStatus.SUCCESS:
                    Debug.WriteLine("SEND Success");
                    break;
                case SendStatus.FAIL_CONNECTION_CLOSING:
                    Debug.WriteLine("SEND failed due to connection closing");
                    break;
                case SendStatus.FAIL_INVALID_PACKET:
                    Debug.WriteLine("SEND failed due to invalid socket");
                    break;
                case SendStatus.FAIL_NOT_CONNECTED:
                    Debug.WriteLine("SEND failed due to no connection");
                    break;
                case SendStatus.FAIL_SOCKET_ERROR:
                    Debug.WriteLine("SEND Socket Error");
                    break;
            }
            
        }

        public void OnDisconnect(INetworkSocket socket)
        {
            m_socketList.Remove(socket);

            String sendString = "** User(" + socket.IPInfo.IPAddress + ") disconnected!";
            AddMsg(sendString);
			//tbSend.Enabled = false;
			//btnSend.Enabled = false;
			btnLights.Enabled = false;
			btnFan.Enabled = false;
			btnRunningLights.Enabled = false;
			btnBlower.Enabled = false;
			btnWiper.Enabled = false;
			btnSpecial.Enabled = false;
			btnShutdown.Enabled = false;
			tbEngRunTime.Text = "";
			tbServerTime.Text = "";

			/*
			byte[] sendBuff= BytesFromString(sendString);
            foreach (var socketObj in m_socketList)
            {
                if (socketObj != socket)
                {
                    socketObj.Send(new Packet(sendBuff,0,sendBuff.Count(),false));
                }
            }
			*/
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

		private void btnConfigParams_Click(object sender, EventArgs e)
		{
			params_form.Set_Params(cfg_params);
			/*
			AddMsg(params_form.get_temp_str(cfg_params.engine_temp_limit));
			AddMsg("");
			AddMsg(cfg_params.engine_temp_limit.ToString());
			AddMsg("");
			AddMsg(cfg_params.si_engine_temp_limit.ToString());
			AddMsg("");
			*/
			if (params_form.ShowDialog(this) == DialogResult.OK)
			{
				//cfg_params = params_form.GetParams();
				//AddMsg(cfg_params.engine_temp_limit.ToString());
				//AddMsg(cfg_params.si_engine_temp_limit.ToString());
			}
			else
			{
				//                this.txtResult.Text = "Cancelled";
			}
			params_form.Hide();
		}

		private void btnFan_Click(object sender, EventArgs e)
		{
			if (cooling_fan)
			{
				Send_Cmd("OFF_FAN");
				cooling_fan = false;
				btnFan.Text = "Fan On";
			}
			else
			{
				Send_Cmd("ON_FAN");
				cooling_fan = true;
				btnFan.Text = "Fan Off";
			}
		}

		private void btnRunningLights_Click(object sender, EventArgs e)
		{
			if (running_lights)
			{
				Send_Cmd("OFF_RUNNING_LIGHTS");
				running_lights = false;
				btnRunningLights.Text = "RunLights On";
			}
			else
			{
				Send_Cmd("ON_RUNNING_LIGHTS");
				running_lights = true;
				btnRunningLights.Text = "RunLights Off";
			}
		}

		private void btnBlower_Click(object sender, EventArgs e)
		{
			switch(blower)
			{
				case 0:
					Send_Cmd("BLOWER1");
					blower = 1;
					btnBlower.Text = "Blower2";
					break;
				case 1:
					Send_Cmd("BLOWER2");
					blower = 2;
					btnBlower.Text = "Blower3";
					break;
				case 2:
					Send_Cmd("BLOWER3");
					blower = 3;
					btnBlower.Text = "Blower Off";
					break;
				case 3:
					Send_Cmd("BLOWER_OFF");
					blower = 0;
					btnBlower.Text = "Blower On";
					break;
				default:
					Send_Cmd("BLOWER_OFF");
					blower = 0;
					btnBlower.Text = "Blower On";
					break;
			}
		}

		// lights
		private void btnSendCmd_Click(object sender, EventArgs e)
		{
			if (lights)
			{
				Send_Cmd("OFF_LIGHTS");
				lights = false;
				btnLights.Text = "Lights On";
				btnBrights.Enabled = false;
				if (brights)
				{
					btnBrights.Text = "Brights Off";
					Send_Cmd("OFF_BRIGHTS");
				}
			}
			else
			{
				Send_Cmd("ON_LIGHTS");
				lights = true;
				btnLights.Text = "Lights Off";
				btnBrights.Enabled = true;
			}
		}

		private void btnWiper_Click(object sender, EventArgs e)
		{
			switch (wipers)
			{
				case 0:
					wipers = 1;
					Send_Cmd("WIPER1");
					btnWiper.Text = "Wiper 1";
					break;
				case 1:
					wipers = 2;
					Send_Cmd("WIPER2");
					btnWiper.Text = "Wiper 2";
					break;
				case 2:
					wipers = 0;
					Send_Cmd("WIPER_OFF");
					btnWiper.Text = "Wiper Off";
					break;
				default:
					wipers = 0;
					Send_Cmd("WIPER_OFF");
					btnWiper.Text = "Wiper off";
					break;
			}
		}

		private void btnSpecial_Click(object sender, EventArgs e)
		{
			Send_Cmd("SPECIAL_CMD");
		}

		private void btnBrights_Click(object sender, EventArgs e)
		{
			if (brights)
			{
				Send_Cmd("OFF_BRIGHTS");
				brights = false;
				btnBrights.Text = "Bright On";
			}
			else
			{
				Send_Cmd("ON_BRIGHTS");
				brights = true;
				btnBrights.Text = "Bright Off";
			}
		}

		private void btnGetParams_Click(object sender, EventArgs e)
		{
			Send_Cmd("SET_PARAMS");
		}

		private void btnUploadParams_Click(object sender, EventArgs e)
		{
			cfg_params = params_form.GetParams();
			//AddMsg(cfg_params.engine_temp_limit.ToString());
			string cmd = "SEND_CONFIG2";

			// some of these have to be sent as 'si' or selected index
			// because that's how they are stored on disk on the server (param.conf)
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

			byte[] bytes = new byte[rpm.Count() + mph.Count() + fpga.Count() + high_rev.Count() 
				+ low_rev.Count() + fan_on.Count() + fan_off.Count() + ben.Count() + b1.Count() 
				+ b2.Count() + b3.Count() + lights.Count() + limit.Count() + batt.Count() + test.Count() + 2];

			bytes[0] = svrcmd.GetCmdIndexB(cmd);
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
			AddMsg(bytes.Length.ToString());
			
			Packet packet = new Packet(bytes, 0, bytes.Count(), false);
			m_server.Broadcast(packet);
		}

		private void btnShutdown_Click(object sender, EventArgs e)
		{
			Send_Cmd("SHUTDOWN");
			tbEngRunTime.Text = "";
		}

		private void Send_Cmd(string cmd2)
		{
			byte[] bytes1;
			string cmd = "SVR_CMD";
			bytes1 = BytesFromString(cmd2);
			byte[] bytes = new byte[bytes1.Count() - 1 + 2];
			bytes[0] = svrcmd.GetCmdIndexB(cmd);
			System.Buffer.BlockCopy(bytes1, 0, bytes, 2, bytes1.Count() - 1);
			Packet packet = new Packet(bytes, 0, bytes.Count(), false);
			m_server.Broadcast(packet);
		}
		String StringFromByteArr(byte[] bytes)
		{
			char[] chars = new char[bytes.Length / sizeof(char)];
			System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
			return new string(chars);
		}
		byte[] BytesFromString(String str)
		{
			byte[] bytes = new byte[str.Length * sizeof(char)];
			System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
			return bytes;
		}
	}
}
