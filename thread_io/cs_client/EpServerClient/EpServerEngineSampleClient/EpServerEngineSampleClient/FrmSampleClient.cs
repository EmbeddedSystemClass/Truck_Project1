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
    public partial class FrmSampleClient : Form, INetworkClientCallback
    {
        enum Server_cmds
        {
            ENABLE_START = 1,
            STARTER_OFF,
            ON_ACC,
            OFF_ACC,
            ON_FUEL_PUMP,
            OFF_FUEL_PUMP,
            ON_FAN,
            OFF_FAN,
            ON_LIGHTS,
            OFF_LIGHTS,
            ON_BRIGHTS,
            OFF_BRIGHTS,
            ON_BRAKES,
            OFF_BRAKES,
            ON_RUNNING_LIGHTS,
            OFF_RUNNING_LIGHTS,
            START_SEQ,              // 17
            SHUTDOWN,
            SHUTDOWN_IOBOX,
            REBOOT_IOBOX,
            SEND_ODATA,
            EDIT_ODATA,
            EDIT_ODATA2,
            SEND_ALL_ODATA,
            RECV_ALL_ODATA,
            SHOW_ODATA,
            SAVE_TO_DISK,
            GET_DIR,
            LCD_SHIFT_RIGHT,
            LCD_SHIFT_LEFT,
            SCROLL_UP,
            SCROLL_DOWN,
            ENABLE_LCD,
            SET_TIME,
            GET_TIME,
            UPLOAD_NEW,
            NEW_PASSWORD1,
            SET_SERIAL_RECV_ON,
            SET_SERIAL_RECV_OFF,
            TEST_LEFT_BLINKER,
            TEST_RIGHT_BLINKER,
            RE_ENTER_PASSWORD,
            DISCONNECT,
            STOP_MBOX_RECV,
            CLOSE_DB,
            OPEN_DB,
            BAD_MSG,
            CURRENT_TIME,
            EXIT_PROGRAM
        }
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

        INetworkClient m_client = new IocpTcpClient();
          private System.Collections.Generic.List<CommonControls> ctls;
        public FrmSampleClient()
        {
            InitializeComponent();
            ctls = new List<CommonControls>();
            foreach (object item in cblistCommon.Items)
                ctls.Add(new CommonControls() { CtlName = item.ToString(), CtlSet = 0, Changed = 0 });
            //            use_main_odata = true;
            this.conn = new System.Data.SqlClient.SqlConnection(connectionString);
            //            this.cmd = new System.Data.SqlClient.SqlCommand("UPDATE O_DATA SET label=@label WHERE port=@recno", conn);
            cblistCommon.Enabled = false;
            tbHostname.Enabled = true;
            tbPort.Enabled = true;
            cblistCommon.Enabled = false;
            button3.Enabled = false;
            button2.Enabled = false;
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
                ClientOps ops = new ClientOps(this, hostname, port);
                m_client.Connect(ops);
            }
            else
            {
                string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.DISCONNECT);
                Send_Cmd(cmd, 0);
                tbHostname.Enabled = true;
                tbPort.Enabled = true;
                btnConnect.Text = "Connect";
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
//            btn_CloseDB.Enabled = true;
            btnConnect.Text = "Disconnect";
            button3.Enabled = true;
            button2.Enabled = true;
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

            string[] words = ret.Split(' ');
            foreach (var word in words)
            {
                //                    temp = int.Parse(word);
                switch (i)
                {
                    case 0:
                        cmd = int.Parse(word);
                        break;
                    case 1:
                        sec = int.Parse(word);
                        break;
                    case 2:
                        mins = int.Parse(word);
                        break;
                    case 3:
                        hours = int.Parse(word);
                        break;
                    case 4:
                        break;
                    default:
                        break;
                }
                i++;
            }

            switch (type_msg)
            {
                case 0:
                    //int i = 0;
                    //int port = 0;
                    //int onoff = 0;
                    //int type = 0;
                    //int time_delay = 0;
                    //string label = "";
                    //string[] words = ret.Split(' ');
                    //foreach (var word in words)
                    //{
                    //    //                    temp = int.Parse(word);
                    //    switch (i)
                    //    {
                    //        case 0:
                    //            label = (string)word;
                    //            break;
                    //        case 1:
                    //            port = int.Parse(word);
                    //            break;
                    //        case 2:
                    //            onoff = int.Parse(word);
                    //            break;
                    //        case 3:
                    //            type = int.Parse(word);
                    //            break;
                    //        case 4:
                    //            time_delay = int.Parse(word);
                    //            Update_Record(port, label, onoff, type, time_delay);
                    //            break;
                    //        default:
                    //            break;
                    //    }
                    //    i++;
                    //}
                    break;
                case 1:
                    AddMsg(mins.ToString() + " " + sec.ToString() + " " + Enum.GetName(typeof(Server_cmds), cmd));
                    break;
                case 2:
                    tbServerTime.Text = hours.ToString() + ':' + mins.ToString() + ':' + sec.ToString();
                    break;
                case 3:
                    tbEngRunTime.Text = hours.ToString() + ':' + mins.ToString() + ':' + sec.ToString();
                    break;
                case 4:
                    AddMsg("msg 4 " + sec.ToString());
                    break;
                default:
                    break;
            }
        }
        void ProcessIOnums2(string nums)
        {
            int temp;

            string[] words = nums.Split(' ');
            foreach (var word in words)
            {
                if (word == "xx")
                    AddMsg("nl");
                else
                {
                    temp = int.Parse(word);
                    AddMsg(temp.ToString());
                }
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
            foreach (var word in words)
            {
                if (word == "xx")
                    AddMsg("nl");
                else
                {
                    temp = int.Parse(word);
                    AddMsg(temp.ToString());
                }
            }
        }
        byte[] BytesFromString(String str)
        {
            byte[] bytes = new byte[str.Length * sizeof(char)];
            System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
            return bytes;
        }
        void ProcessByteArr(byte[] bytes)
        {
            char[] chars = new char[bytes.Length / sizeof(char)];
            System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
            if (chars[0] == 0xAA)
                MessageBox.Show("AA");
            else if (chars[0] == 0x55)
                MessageBox.Show("55");
            else MessageBox.Show(chars[0].ToString());
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
        private void Send_Cmd(string cmd, int onoff)
        {
            string test = " ";
            int sendcmd = 0;
            byte[] bytes = BytesFromString(test);

            //            MessageBox.Show(cmd);
            switch (cmd)
            {
                case "STARTER":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ENABLE_START;
                    else sendcmd = (int)Server_cmds.STARTER_OFF;
                    //                    cblistCommon.SetSelected(0, false);
                    break;
                case "IGNITION":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_ACC;
                    else sendcmd = (int)Server_cmds.OFF_ACC;
                    break;
                case "FUELPUMP":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_FUEL_PUMP;
                    else sendcmd = (int)Server_cmds.OFF_FUEL_PUMP;
                    break;
                case "COOLINGFAN":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_FAN;
                    else sendcmd = (int)Server_cmds.OFF_FAN;
                    break;
                case "LIGHTS":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_LIGHTS;
                    else sendcmd = (int)Server_cmds.OFF_LIGHTS;
                    break;
                case "BRIGHTS":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_BRIGHTS;
                    else sendcmd = (int)Server_cmds.OFF_BRIGHTS;
                    break;
                case "START_SEQ":
                    sendcmd = (int)Server_cmds.START_SEQ;
                    break;
                case "SHUTDOWN":
                    sendcmd = (int)Server_cmds.SHUTDOWN;
                    break;
                case "SHUTDOWN_IOBOX":
                    sendcmd = (int)Server_cmds.SHUTDOWN_IOBOX;
                    break;
                case "REBOOT_IOBOX":
                    sendcmd = (int)Server_cmds.REBOOT_IOBOX;
                    break;
                case "CLOSE_DB":
                    sendcmd = (int)Server_cmds.CLOSE_DB;
                    break;
                case "OPEN_DB":
                    sendcmd = (int)Server_cmds.OPEN_DB;
                    break;
                case "TEST_LEFT_BLINKER":
                    sendcmd = (int)Server_cmds.TEST_LEFT_BLINKER;
                    break;
                case "TEST_RIGHT_BLINKER":
                    sendcmd = (int)Server_cmds.TEST_RIGHT_BLINKER;
                    break;
                case "UPLOAD_NEW":
                    sendcmd = (int)Server_cmds.UPLOAD_NEW;
                    break;
                case "BRAKES":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_BRAKES;
                    else sendcmd = (int)Server_cmds.OFF_BRAKES;
                    break;
                case "RUNNING_LIGHTS":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_RUNNING_LIGHTS;
                    else sendcmd = (int)Server_cmds.OFF_RUNNING_LIGHTS;
                    break;
                case "SAVE_TO_DISK":
                    sendcmd = (int)Server_cmds.SAVE_TO_DISK;
                    break;
                case "CURRENT_TIME":
                    sendcmd = (int)Server_cmds.CURRENT_TIME;
                    break;
                case "GET_TIME":
                    sendcmd = (int)Server_cmds.GET_TIME;
                    break;
                case "SHOW_ODATA":
                    sendcmd = (int)Server_cmds.SHOW_ODATA;
                    break;
                case "DISCONNECT":
                    sendcmd = (int)Server_cmds.DISCONNECT;
                    break;
                case "STOP_MBOX_RECV":
                    sendcmd = (int)Server_cmds.STOP_MBOX_RECV;
                    break;
                default:
                    return;
            }
            bytes.SetValue((byte)sendcmd, 0);
            //            AddMsg(Enum.GetName(typeof(Server_cmds), sendcmd));
            //MessageBox.Show(send_cmd.ToString());
            Packet packet = new Packet(bytes, 0, bytes.Count(), false);
            m_client.Send(packet);

        }
        private void btn_ClrScr_Click(object sender, EventArgs e)
        {
            tbReceived.Clear();
        }
        private void connect(object sender, EventArgs e)
        {
            btnConnect_Click(sender, e);
        }
        private void shutdownServerToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }
        private void rebootServerToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }
        private void setServerTimeToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
        // start engine
        private void button2_Click(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.START_SEQ);
            //            AddMsg("start seq: " + cmd);
            Send_Cmd(cmd, 0);
        }
        // shutdown engine
        private void button3_Click_1(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.SHUTDOWN);
            Send_Cmd(cmd, 0);
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
            bytes2[0] = (byte)Server_cmds.SET_TIME;
            Packet packet = new Packet(bytes2, 0, bytes2.Count(), false);
            m_client.Send(packet);

        }
        private void CellSelectChanged(object sender, EventArgs e)
        {
//            btn_SendSelectedRecords.Enabled = true;
        }
        private void sendNLToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
        private void getServerTimeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.GET_TIME);
            Send_Cmd(cmd, 0);
        }
        private void ClearScreen_Click(object sender, EventArgs e)
        {
            tbReceived.Clear();
        }
        private void showODATAToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.SHOW_ODATA);
            Send_Cmd(cmd, 0);
        }
        private void stopMBoxRecvToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }
        private void testDlgToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //            int promptValue = Prompt.ShowDialog("Test", "123");
        }
        public void ShowMyDialogBox()
        {
            DlgForm1 testDialog = new DlgForm1();
            testDialog.m_client = m_client;
            // Show testDialog as a modal dialog and determine if DialogResult = OK.
            if (testDialog.ShowDialog(this) == DialogResult.OK)
            {
                AddMsg(testDialog.getStr());
                // Read the contents of testDialog's TextBox.
                //this..Text = testDialog.TextBox1.Text;
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
                    Send_Cmd(ctls[i].CtlName.ToString(), ctls[i].CtlSet);
                }
            }
        }

        private void ShutdownServer(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.SHUTDOWN_IOBOX);
            Send_Cmd(cmd, 0);
        }

        private void RebootServer(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.REBOOT_IOBOX);
            Send_Cmd(cmd, 0);
        }

        private void StopMbox(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.STOP_MBOX_RECV);
            Send_Cmd(cmd, 0);
        }

        private void DBMgmt(object sender, EventArgs e)
        {
            ShowMyDialogBox();
        }
    }
}

