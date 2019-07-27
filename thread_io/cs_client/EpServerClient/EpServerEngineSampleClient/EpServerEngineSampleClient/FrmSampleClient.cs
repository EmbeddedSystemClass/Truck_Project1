using System;
using System.Collections.Generic;
using System.Xml;
using System.Data;
using System.Data.SqlClient;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using EpServerEngine.cs;
using System.Diagnostics;
using System.Globalization;
using EpLibrary.cs;
using System.IO;

namespace EpServerEngineSampleClient
{
    enum msg_types
    {
        SEND_MSG = 60,
        CURRENT_TIME,
        ENGINE_RUNTIME,
        SERVER_UPTIME,
        SEND_CONFIG,
        GET_TIME,
		SHUTDOWN,
		ENGINE_TEMP,
		SEND_RPM,
		SEND_MPH,
		SYSTEM_UP,
		SYSTEM_DOWN
    }
    public partial class FrmSampleClient : Form, INetworkClientCallback
    {
        public class CommonControls : IEquatable<CommonControls>
        {
            public string CtlName { get; set; }
            public int CtlSet { get; set; }
            public int Changed { get; set; }

            public override string ToString()
            {
                return "Name: " + CtlName + " set: " + CtlSet;
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
                return CtlSet;
            }
            public bool Equals(CommonControls other)
            {
                if (other == null) return false;
                return (this.CtlSet.Equals(other.CtlSet));
            }
        }
        ConfigParams cfg_params = new ConfigParams();
        ServerCmds svrcmd = new ServerCmds();
        INetworkClient m_client = new IocpTcpClient();
        private System.Collections.Generic.List<CommonControls> ctls;
		private bool system_up = true;
        public FrmSampleClient()
        {
            InitializeComponent();
            ctls = new List<CommonControls>();
            foreach (object item in cblistCommon.Items)
            {
                ctls.Add(new CommonControls() { CtlName = item.ToString(), CtlSet = 0, Changed = 0 });
            }
            //            use_main_odata = true;
            this.conn = new System.Data.SqlClient.SqlConnection(connectionString);
            //            this.cmd = new System.Data.SqlClient.SqlCommand("UPDATE O_DATA SET label=@label WHERE port=@recno", conn);
            svrcmd.SetClient(m_client);

            cblistCommon.Enabled = false;
            tbHostname.Enabled = true;
            tbReceived.Enabled = true;
            tbPort.Enabled = true;
            button3.Enabled = false;
            button2.Enabled = false;
            btnShutdown.Enabled = false;
            btnReboot.Enabled = false;
            btnStopSerial.Enabled = false;
            btn_SetTime.Enabled = false;
            btnGetTime.Enabled = false;
            //Upload_New.Enabled = false;
            //            btnSetParams.Enabled = false;
            tbConnected.Text = "not connected";
//            target_db_closed = false;
            //            use_laptop = true;
            tbReceived.Clear();
        }
        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (btnConnect.Text.Equals("Connect"))
            {
                string hostname = tbHostname.Text;
                string port = tbPort.Text;
                tbHostname.Enabled = false;
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
                svrcmd.Send_Cmd(cmd, 0);
                tbHostname.Enabled = true;
                tbPort.Enabled = true;
                btnConnect.Text = "Connect";
                btnShutdown.Enabled = false;
                btnReboot.Enabled = false;
                btnStopSerial.Enabled = false;
                btn_SetTime.Enabled = false;
                btnGetTime.Enabled = false;
                //Upload_New.Enabled = false;
                tbEngRunTime.Text = "";
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
        }
        public void OnConnected(INetworkClient client, ConnectStatus status)
        {
            tbConnected.Text = "connected";
            cblistCommon.Enabled = true;
            btnConnect.Text = "Disconnect";
            button3.Enabled = true;
            button2.Enabled = true;
            btnSetParams.Enabled = true;
        }
        public void OnDisconnect(INetworkClient client)
        {
            int i;
            tbHostname.Enabled = true;
            tbPort.Enabled = true;
            cblistCommon.Enabled = false;
            btnConnect.Text = "Connect";
            tbConnected.Text = "not connected";
            button3.Enabled = false;
            button2.Enabled = false;
            btnSetParams.Enabled = false;
            for (i = 0; i < ctls.Count; i++)
            {
                ctls[i].CtlSet = 0;
                ctls[i].Changed = 0;
            }
            for (i = 0; i < cblistCommon.Items.Count; i++)
            {
                cblistCommon.SetItemChecked(i, false);
            }
        }
        public void OnReceived(INetworkClient client, Packet receivedPacket)
        {
            Process_Msg(receivedPacket.PacketRaw);
        }
        private void Process_Msg(byte[] bytes)
        {
            int type_msg;
            string ret = null;
            int sec = 0;
            int mins = 0;
            int hours = 0;
            int cmd = 0;
            int i = 0;
            char[] chars = new char[bytes.Length / sizeof(char) + 2];
            char[] chars2 = new char[bytes.Length / sizeof(char)];
            // src srcoffset dest destoffset len
            System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
            type_msg = (int)chars[0];
            System.Buffer.BlockCopy(bytes, 2, chars2, 0, bytes.Length - 2);
            ret = new string(chars2);

            string str = Enum.GetName(typeof(msg_types), type_msg);
//            int type = (int)Enum.Parse(typeof(msg_types), str);
            //if (type_msg != 4)
            //{ 
            //    string[] words = ret.Split(' ');
            //    foreach (var word in words)
            //    {
            //        //                    temp = int.Parse(word);
            //        switch (i)
            //        {
            //            case 0:
            //                cmd = int.Parse(word);
            //                break;
            //            case 1:
            //                sec = int.Parse(word);
            //                break;
            //            case 2:
            //                mins = int.Parse(word);
            //                break;
            //            case 3:
            //                hours = int.Parse(word);
            //                break;
            //             case 4:
            //                break;
            //            default:
            //                break;
            //        }
            //        i++;
            //    }
            //}

            switch (str)
            {
                case "SEND_MSG":
                    AddMsg(ret);
                    if (ret == "SHUTDOWN")
					{
//		                tbEngineTemp.Text = "";
						tbRPM.Text = "0";
						tbMPH.Text = "0";
                        tbEngRunTime.Text = "";
					}
                    break;
                case "CURRENT_TIME":
//                    AddMsg(mins.ToString() + " " + sec.ToString() + " " + "curr" + " " + svrcmd.GetName(cmd));
                    //                    AddMsg(mins.ToString() + " " + sec.ToString());
                    break;
                case "SERVER_UPTIME":
                    //                    tbServerTime.Text = hours.ToString() + ':' + mins.ToString() + ':' + sec.ToString();
                    tbServerTime.Text = ret;
                    break;
                case "ENGINE_RUNTIME":
                    //    tbEngRunTime.Text = hours.ToString() + ':' + mins.ToString() + ':' + sec.ToString();
                    tbEngRunTime.Text = ret;
                    break;
				case "ENGINE_TEMP":
					tbEngineTemp.Text = ret;
					break;
                case "SEND_RPM":
					tbRPM.Text = ret;
					break;
                case "SEND_MPH":
					tbMPH.Text = ret;
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
                                cfg_params.blower_enabled  = int.Parse(word);
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
                    /*
                                        rpm_update_rate,
                                        mph_update_rate,
                                        fpga_xmit_rate
                                        high_rev_limit,
                                        low_rev_limit,
                                        cooling_fan_on,
                                        cooling_fan_off,
                                        blower_enabled,
                                        blower1_on,
                                        blower2_on,
                                        blower3_on,
                                        test_bank
                    */
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
					//Upload_New.Enabled = false;
					tbEngRunTime.Text = "";
//					tbEngineTemp.Text = "";
					tbServerTime.Text = "";
					tbRPM.Text = "0";
					tbMPH.Text = "0";
					if (m_client.IsConnectionAlive)
						m_client.Disconnect();

					AddMsg(ret);
					break;

				case "SYSTEM_UP":
					AddMsg(ret);
					break;

				case "SYSTEM_DOWN":
					AddMsg(ret);
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
            svrcmd.Send_Cmd(cmd, 0);
        }
        // shutdown engine
        private void button3_Click_1(object sender, EventArgs e)
        {
            string cmd = "SHUTDOWN";
            svrcmd.Send_Cmd(cmd, 0);
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
        private void cblist_SelValueChanged(object sender, EventArgs e)
        {
            int i;
            for (i = 0; i < cblistCommon.Items.Count; i++)
            {
                if (cblistCommon.Items[i].ToString() == ctls[i].CtlName)
                {
                    if (cblistCommon.GetItemChecked(i) == true && ctls[i].CtlSet == 0)
                    {
                        ctls[i].CtlSet = 1;
                        ctls[i].Changed = 1;
                    }
                    else if (cblistCommon.GetItemChecked(i) == false && ctls[i].CtlSet == 1)
                    {
                        ctls[i].CtlSet = 0;
                        ctls[i].Changed = 1;
                    }
                }
            }
            for (i = 0; i < ctls.Count; i++)
            {
                if (ctls[i].Changed == 1)
                {
                    ctls[i].Changed = 0;
//                    Send_Cmd(ctls[i].CtlName.ToString(), ctls[i].CtlSet);
                    svrcmd.Send_Cmd(ctls[i].CtlName.ToString(), ctls[i].CtlSet);
//                    AddMsg(ctls[i].CtlName);
                    if (ctls[i].CtlName == "STARTER" || ctls[i].CtlName == "TEST_LEFT_BLINKER"
                             || ctls[i].CtlName == "TEST_RIGHT_BLINKER" || ctls[i].CtlName == "SPECIAL_CMD" ||
                                 ctls[i].CtlName == "BLOWER1_ON" || ctls[i].CtlName == "BLOWER2_ON" || 
                                     ctls[i].CtlName == "BLOWER2_ON" || ctls[i].CtlName == "BLOWER3_ON" || 
                                     ctls[i].CtlName == "BLOWER_OFF")
                    {
                        ctls[i].CtlSet = 0;
                        cblistCommon.SetItemChecked(i, false);
                    }
                    
//                    MessageBox.Show(ctls[i].CtlName.ToString()+ " " + i.ToString());
                }
            }
        }
        private void ShutdownServer(object sender, EventArgs e)
        {
            string cmd = "SHUTDOWN_IOBOX";
            svrcmd.Send_Cmd(cmd, 0);
        }
        private void RebootServer(object sender, EventArgs e)
        {
            string cmd = "REBOOT_IOBOX";
            svrcmd.Send_Cmd(cmd, 0);
        }
        private void StopMbox(object sender, EventArgs e)
        {
			if(system_up)
			{
				string cmd = "STOP_MBOX_XMIT";
				svrcmd.Send_Cmd(cmd, 0);
				system_up = false;
				btnStopSerial.Text = "Start Mbox Xmit";
				AddMsg("System Down");
			}
			else
			{
				string cmd = "START_MBOX_XMIT";
				svrcmd.Send_Cmd(cmd, 0);
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
            svrcmd.Send_Cmd(cmd, 0);
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
    }
}

