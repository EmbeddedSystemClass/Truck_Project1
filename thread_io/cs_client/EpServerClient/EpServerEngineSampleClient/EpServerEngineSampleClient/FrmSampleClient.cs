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
        private bool target_db_closed;
        private bool use_main_odata;
        private bool use_laptop;
        public FrmSampleClient()
        {
            InitializeComponent();
            use_main_odata = true;
            this.conn = new System.Data.SqlClient.SqlConnection(connectionString);
            //            this.cmd = new System.Data.SqlClient.SqlCommand("UPDATE O_DATA SET label=@label WHERE port=@recno", conn);
            ctls = new List<CommonControls>();
            foreach (object item in cblistCommon.Items)
                ctls.Add(new CommonControls() { CtlName = item.ToString(), CtlSet = 0, Changed = 0 });
            cblistCommon.Enabled = false;
            tbHostname.Enabled = true;
            tbPort.Enabled = true;
            cblistCommon.Enabled = false;
            btn_CloseDB.Enabled = false;
            button3.Enabled = false;
            button2.Enabled = false;
            btn_SendSelectedRecords.Enabled = false;
            tbConnected.Text = "not connected";
            target_db_closed = false;
            //            use_laptop = true;
            use_laptop = false;
            currentconnectionString = connectionStringlt;
            Load_Grid(currentconnectionString);
      
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
            btn_CloseDB.Enabled = true;
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
            btn_CloseDB.Enabled = false;
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
                    AddMsg("msg 3 " + sec.ToString() + " " + mins.ToString());
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
        private void Update_Record(int port, string label, int onoff, int type, int time_delay)
        {
            try
            {
                using (SqlConnection conn =
                    new SqlConnection(connectionString))
                {
                    conn.Open();
                    System.Data.SqlClient.SqlCommand cmd;
                    if (use_main_odata)
                    {
                        cmd = new System.Data.SqlClient.SqlCommand("UPDATE O_DATA SET label=@label, onoff=@onoff, polarity=@polarity, type=@type, time_delay= @time_delay,time_left=@time_left, reset=@reset WHERE port=@port", conn);
                    }
                    else
                    {
                        cmd = new System.Data.SqlClient.SqlCommand("UPDATE O_DATA2 SET label=@label, onoff=@onoff, polarity=@polarity, type=@type, time_delay= @time_delay,time_left=@time_left, reset=@reset WHERE port=@port", conn);
                    }
                    cmd.Parameters.AddWithValue("@label", label);
                    cmd.Parameters.AddWithValue("@port", port.ToString());
                    cmd.Parameters.AddWithValue("@onoff", onoff.ToString());
                    cmd.Parameters.AddWithValue("@polarity", "0");
                    cmd.Parameters.AddWithValue("@type", type.ToString());
                    cmd.Parameters.AddWithValue("@time_delay", time_delay.ToString());
                    cmd.Parameters.AddWithValue("@time_left", "0");
                    cmd.Parameters.AddWithValue("@pulse_time", "0");
                    cmd.Parameters.AddWithValue("@reset", "0");
                    int rows = cmd.ExecuteNonQuery();
                    //                    AddMsg(rows.ToString() + "updated: " + port.ToString());
                }
            }
            catch (SqlException ex)
            {
                AddMsg(ex.Message.ToString());
            }
        }
        private void Update_Table(int port, string data)
        {
            SqlConnection sqlCnn;
            SqlCommandBuilder cmdBuilder;
            //string sql ="INSERT INTO dbo.O_DATA (label, port, onoff, polarity, type, time_delay, time_left, pulse_time, reset) VALUES(@label,@port,@onoff,@polarity,@type, @time_delay,@time_left,@reset)";
            string sql = null;
            //            SqlDataAdapter adapter = new SqlDataAdapter();
            DataSet ds2 = new DataSet();
            int i = 0;
            if (use_main_odata)
                sql = "select * from O_DATA";
            else
                sql = "select * from O_DATA2";
            sqlCnn = new SqlConnection(currentconnectionString);
            //            adapter.SelectCommand = sqlCmd;
            try
            {
                DataTable table;
                DataRow row;
                //Delete_O_DATA();
                table = MakeNamesTable();
                sqlCnn.Open();
                //              MessageBox.Show(ds2.DataSetName);
                for (i = 0; i < 40; i++)
                {
                    row = table.NewRow();
                    row["label"] = dataGridView1.Rows[i].Cells[0].Value.ToString();
                    row["port"] = dataGridView1.Rows[i].Cells[1].Value;
                    row["onoff"] = dataGridView1.Rows[i].Cells[2].Value;
                    row["input_port"] = dataGridView1.Rows[i].Cells[3].Value;
                    row["polarity"] = dataGridView1.Rows[i].Cells[4].Value;
                    row["type"] = dataGridView1.Rows[i].Cells[5].Value;
                    row["time_delay"] = dataGridView1.Rows[i].Cells[6].Value;
                    row["time_left"] = dataGridView1.Rows[i].Cells[7].Value;
                    row["pulse_time"] = dataGridView1.Rows[i].Cells[8].Value;
                    row["reset"] = dataGridView1.Rows[i].Cells[9].Value;
                    table.Rows.Add(row);
                }
                dataGridView1.DataSource = bindingSource1;
                bindingSource1.DataSource = table;
                SqlDataAdapter adapter = new SqlDataAdapter(sql, sqlCnn);
                cmdBuilder = new SqlCommandBuilder(adapter);
                adapter.Update(table);
                //adapter.UpdateCommand = sqlCnn.CreateCommand();
                //adapter.UpdateCommand.CommandText = sql;
                //adapter.UpdateCommand.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
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
        private void btn_XML_Click(object sender, EventArgs e)
        {
            try
            {
                XmlReader xmlFile;
                if(use_laptop == true)
                    xmlFile = XmlReader.Create("C:\\Users\\Dan_Laptop\\dev\\odata.xml", new XmlReaderSettings());
                else
                    xmlFile = XmlReader.Create("C:\\Users\\Daniel\\dev\\odata.xml", new XmlReaderSettings());
                ds.ReadXml(xmlFile);
                dataGridView1.DataSource = ds.Tables[0];
                bindingSource1.DataSource = ds.Tables[0];
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
        private void btn_Show_Click(object sender, EventArgs e)
        {
            SqlConnection sqlCnn;
            SqlCommand sqlCmd;
            SqlDataAdapter adapter = new SqlDataAdapter();
            DataSet ds2 = new DataSet();
            string sql = null;
            if (use_main_odata)
                sql = "Select * from O_DATA";
            else
                sql = "Select * from O_DATA2";
            sqlCnn = new SqlConnection(currentconnectionString);
            try
            {
                sqlCnn.Open();
                sqlCmd = new SqlCommand(sql, sqlCnn);
                adapter.SelectCommand = sqlCmd;
                adapter.Fill(ds2);  // fill dataset with records

                //MessageBox.Show(ds2.DataSetName);
                dataGridView1.DataSource = ds2.Tables[0];
                //for (i = 0; i <= ds2.Tables[0].Rows.Count - 1; i++)
                //{
                //    MessageBox.Show(ds2.Tables[0].Rows[i].ItemArray[0] + " " + ds2.Tables[0].Rows[i].ItemArray[1]);
                //}
                adapter.Dispose();
                sqlCmd.Dispose();
                sqlCnn.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void Load_Grid(string connection_string)
        {
            SqlConnection sqlCnn;
            SqlDataAdapter adapter = new SqlDataAdapter();
            int i = 0;
            string sql = null;
            if (use_main_odata)
                //                sql = "Select label, port, onoff, input_port, polarity, type, time_delay, time_left, pulse_time, reset from O_DATA";
                sql = "Select label, port, onoff, input_port, type, time_delay from O_DATA";
            else
                //                sql = "Select label, port, onoff, input_port, polarity, type, time_delay, time_left, pulse_time, reset from O_DATA2";
                sql = "Select label, port, onoff, input_port, type, time_delay from O_DATA2";

            sqlCnn = new SqlConnection(connection_string);
            try
            {
                sqlCnn.Open();
                string tconstr = sqlCnn.ConnectionString;

                tconstr = tconstr.Substring(tconstr.IndexOf("Client"), 20);
                tconstr = tconstr.Substring(0, tconstr.IndexOf(';'));
                tbConnectionString.Text = tconstr;
                adapter = new SqlDataAdapter(sql, sqlCnn);
                dataGridView1.DataSource = bindingSource1;

                table.Locale = System.Globalization.CultureInfo.InvariantCulture;
                table.Clear();
                adapter.Fill(table);
                bindingSource1.DataSource = table;
                SqlCommandBuilder builder = new SqlCommandBuilder(adapter);
                adapter.DeleteCommand = builder.GetDeleteCommand();
                adapter.UpdateCommand = builder.GetUpdateCommand();
                //                da.InsertCommand = builder.GetInsertCommand();
                dataGridView1.DataSource = table;
                
                sqlCnn.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            //dataGridView1.Columns["port"].ReadOnly = true;
            //dataGridView1.Columns["label"].Width = 100;
            //dataGridView1.Columns["port"].Width = 50;
            //dataGridView1.Columns["onoff"].Width = 50;
            //dataGridView1.Columns["input_port"].Width = 50;
            ////                dataGridView1.Columns["polarity"].Width = 50;
            //dataGridView1.Columns["polarity"].Visible = false;
            //dataGridView1.Columns["type"].Width = 50;
            //dataGridView1.Columns["time_delay"].Width = 50;
            ////dataGridView1.Columns["time_left"].Width = 50;
            ////dataGridView1.Columns["pulse_time"].Width = 50;
            ////dataGridView1.Columns["reset"].Width = 50;
            //dataGridView1.Columns["time_left"].Visible = false;
            //dataGridView1.Columns["pulse_time"].Visible = false;
            //dataGridView1.Columns["reset"].Visible = false;
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
                case "SEND_NL":
                    sendcmd = (int)Server_cmds.CURRENT_TIME;
                    break;
                case "GET_TIME":
                    sendcmd = (int)Server_cmds.GET_TIME;
                    break;
                case "SHOW_ODATA":
                    sendcmd = (int)Server_cmds.SHOW_ODATA;
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
        private DataTable MakeNamesTable()
        {
            // Create a new DataTable titled 'Names.'
            DataTable namesTable = new DataTable("NewTable");

            DataColumn labelColumn = new DataColumn();
            labelColumn.DataType = System.Type.GetType("System.String");
            labelColumn.ColumnName = "label";
            namesTable.Columns.Add(labelColumn);

            DataColumn portColumn = new DataColumn();
            portColumn.DataType = System.Type.GetType("System.Int32");
            portColumn.AutoIncrement = true;
            portColumn.ColumnName = "port";
            namesTable.Columns.Add(portColumn);

            DataColumn onoffColumn = new DataColumn();
            onoffColumn.DataType = System.Type.GetType("System.Int32");
            onoffColumn.ColumnName = "onoff";
            namesTable.Columns.Add(onoffColumn);

            DataColumn input_portColumn = new DataColumn();
            input_portColumn.DataType = System.Type.GetType("System.Int32");
            input_portColumn.ColumnName = "input_port";
            namesTable.Columns.Add(input_portColumn);

            DataColumn polarityColumn = new DataColumn();
            polarityColumn.DataType = System.Type.GetType("System.Int32");
            polarityColumn.ColumnName = "polarity";
            namesTable.Columns.Add(polarityColumn);

            DataColumn typeColumn = new DataColumn();
            typeColumn.DataType = System.Type.GetType("System.Int32");
            typeColumn.ColumnName = "type";
            namesTable.Columns.Add(typeColumn);

            DataColumn time_delayColumn = new DataColumn();
            time_delayColumn.DataType = System.Type.GetType("System.Int32");
            time_delayColumn.ColumnName = "time_delay";
            namesTable.Columns.Add(time_delayColumn);

            DataColumn time_leftColumn = new DataColumn();
            time_leftColumn.DataType = System.Type.GetType("System.Int32");
            time_leftColumn.ColumnName = "time_left";
            namesTable.Columns.Add(time_leftColumn);

            DataColumn pulse_timeColumn = new DataColumn();
            pulse_timeColumn.DataType = System.Type.GetType("System.Int32");
            pulse_timeColumn.ColumnName = "pulse_time";
            namesTable.Columns.Add(pulse_timeColumn);

            DataColumn resetColumn = new DataColumn();
            resetColumn.DataType = System.Type.GetType("System.Int32");
            resetColumn.ColumnName = "reset";
            namesTable.Columns.Add(resetColumn);

            // Create an array for DataColumn objects.
            DataColumn[] keys = new DataColumn[1];
            keys[0] = portColumn;
            namesTable.PrimaryKey = keys;

            // Return the new DataTable.
            return namesTable;
        }
        private void Delete_O_DATA()
        {
            SqlConnection sqlCnn;
            SqlDataAdapter adapter = new SqlDataAdapter();

            string sql = null;
            if (use_main_odata)
                sql = "delete O_DATA";
            else sql = "delete O_DATA2";

            sqlCnn = new SqlConnection(currentconnectionString);
            try
            {
                sqlCnn.Open();
                adapter.DeleteCommand = sqlCnn.CreateCommand();
                adapter.DeleteCommand.CommandText = sql;
                adapter.DeleteCommand.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void btn_ClrScr_Click(object sender, EventArgs e)
        {
            tbReceived.Clear();
        }
        private void readXML(object sender, EventArgs e)
        {
            btn_XML_Click(sender, e);
        }
        private void connect(object sender, EventArgs e)
        {
            btnConnect_Click(sender, e);
        }
        private void writeXML(object sender, EventArgs e)
        {

        }
        private void newDB(object sender, EventArgs e)
        {
            currentconnectionString = connectionString2;
            Load_Grid(currentconnectionString);
            use_main_odata = true;
        }
        private void showDB(object sender, EventArgs e)
        {
            btn_Show_Click(sender, e);
        }
        private void Update(object sender, EventArgs e)
        {
        }
        private void restoreMainDBToolStripMenuItem_Click(object sender, EventArgs e)
        {
            currentconnectionString = connectionString;
            Load_Grid(currentconnectionString);
        }
        private void clearDataSetToolStripMenuItem_Click(object sender, EventArgs e)
        {
            dataGridView1.SelectAll();
            foreach (DataGridViewRow item in dataGridView1.SelectedRows)
                dataGridView1.Rows.RemoveAt(item.Index);
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
        private void OpenDB_Click(object sender, EventArgs e)
        {

        }
        private void shutdownServerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.SHUTDOWN_IOBOX);
            Send_Cmd(cmd, 0);
        }
        private void rebootServerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.REBOOT_IOBOX);
            Send_Cmd(cmd, 0);
        }
        private void setServerTimeToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
        private void btn_CloseDB_Click(object sender, EventArgs e)
        {
            string cmd = null;
            if (target_db_closed == false)  // db on target is open so close it
            {
                cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.CLOSE_DB);
                target_db_closed = true;
                btn_CloseDB.Text = "Open Target DB";
            }
            else
            {
                cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.OPEN_DB);
                target_db_closed = false;
                btn_CloseDB.Text = "Close Target DB";
            }

            Send_Cmd(cmd, 0);
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
        private void SwitchTable_Click(object sender, EventArgs e)
        {
            if (use_main_odata)
            {
                use_main_odata = false;
                tbCurrentTable.Text = "O_DATA2";
            }
            else
            {
                use_main_odata = true;
                tbCurrentTable.Text = "O_DATA";
            }
        }
        private void DeleteO_DATA_Click(object sender, EventArgs e)
        {
            Delete_O_DATA();
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

        private void Btn_SaveTargetDB_Click(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.SAVE_TO_DISK);
            Send_Cmd(cmd, 0);
//            AddMsg(cmd);
        }

        private void Btn_NewTable_Click(object sender, EventArgs e)
        {
            Update_Table(0, "");
        }

        private void SendSelectedRecords(object sender, EventArgs e)
        {
            int cell = 0;
            int port, onoff, type, time_delay, input_port;
            dataGridView1.Columns["port"].ReadOnly = true;
            dataGridView1.Columns["label"].Width = 100;
            dataGridView1.Columns["port"].Width = 50;
            dataGridView1.Columns["onoff"].Width = 50;
            dataGridView1.Columns["input_port"].Width = 50;
            //                dataGridView1.Columns["polarity"].Width = 50;
            dataGridView1.Columns["polarity"].Visible = false;
            dataGridView1.Columns["type"].Width = 50;
            dataGridView1.Columns["time_delay"].Width = 50;
            //dataGridView1.Columns["time_left"].Width = 50;
            //dataGridView1.Columns["pulse_time"].Width = 50;
            //dataGridView1.Columns["reset"].Width = 50;
            dataGridView1.Columns["time_left"].Visible = false;
            dataGridView1.Columns["pulse_time"].Visible = false;
            dataGridView1.Columns["reset"].Visible = false;

            Int32 selectedCellCount =
             dataGridView1.GetCellCount(DataGridViewElementStates.Selected);
            if (selectedCellCount > 0)
            {
                if (dataGridView1.AreAllCellsSelected(true))
                {
                    MessageBox.Show("All cells are selected", "Selected Cells");
                    btn_SendSelectedRecords.Enabled = false;
                }
                else
                {
                    tbReceived.Clear();
                    Int32 selectedRowCount =
                        dataGridView1.Rows.GetRowCount(DataGridViewElementStates.Selected);
                    if (selectedRowCount > 0)
                    {
                        System.Text.StringBuilder sb = new System.Text.StringBuilder();
                        for (int i = 0; i < selectedRowCount; i++)
                        {
                            //                            sb.Append("Row: ");
                            //                            sb.Append(dataGridView1.SelectedRows[i].Cells[0].Value.ToString());
                            cell = dataGridView1.SelectedCells[i].RowIndex;
                            dataGridView1.SelectedCells[i].Selected = false;
                            //MessageBox.Show(i.ToString() + " " + cell.ToString());
                            port = (int)dataGridView1.Rows[cell].Cells[1].Value;
                            onoff = (int)dataGridView1.Rows[cell].Cells[2].Value;
                            if (onoff > 0)
                                onoff = 1;
                            input_port = (int)dataGridView1.Rows[cell].Cells[3].Value;
                            type = (int)dataGridView1.Rows[cell].Cells[5].Value;
                            if (type > 5)
                            {
                                AddMsg("type cannot be > 5");
                                type = 0;
                            }
                            time_delay = (int)dataGridView1.Rows[cell].Cells[6].Value;
                            string sendstr = port.ToString() + '{' + onoff.ToString() + '|' + type.ToString() + '}' + time_delay.ToString() + '~' + input_port.ToString() + 'z';
                                                        sb.Append("port: " + port.ToString() + " onoff: " + onoff.ToString() + " type: " + type.ToString() + " td: " + time_delay.ToString() + " input: " + input_port.ToString());
                                                        sb.Append(Environment.NewLine);
                            //MessageBox.Show(sb.ToString());
                            byte[] bytes = BytesFromString(sendstr);
                            byte[] bytes2 = new byte[bytes.Count() + 2];
                            System.Buffer.BlockCopy(bytes, 0, bytes2, 2, bytes.Length - 2);
                            bytes2[0] = (byte)Server_cmds.SEND_ODATA;
                            Packet packet = new Packet(bytes2, 0, bytes2.Count(), false);
                            m_client.Send(packet);
                        }
                        AddMsg(sb.ToString());
                        sb.Clear();
                        btn_SendSelectedRecords.Enabled = false;
                    }
                }
            }
        }

        private void CellSelectChanged(object sender, EventArgs e)
        {
            btn_SendSelectedRecords.Enabled = true;
        }

        private void sendNLToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void getServerTimeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.GET_TIME);
            Send_Cmd(cmd, 0);
        }

        private void btn_Laptop_Click(object sender, EventArgs e)
        {
            if (use_laptop == false)
            {
                use_laptop = true;
                btn_laptop.Text = "Desktop";
                currentconnectionString = connectionStringlt;
            }
            else
            {
                use_laptop = false;
                btn_laptop.Text = "Laptop";
                currentconnectionString = connectionString;
            }
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
    }
}
