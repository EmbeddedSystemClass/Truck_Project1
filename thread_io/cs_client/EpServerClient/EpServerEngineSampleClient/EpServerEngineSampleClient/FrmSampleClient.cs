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
            START_SEQ,
            SHUTDOWN,
            SHUTDOWN_IOBOX,
            TEMP,
            REBOOT_IOBOX,
            SEND_IDATA,
            SEND_ODATA,
            EDIT_IDATA,
            EDIT_ODATA,
            EDIT_IDATA2,
            EDIT_ODATA2,
            SEND_ALL_IDATA,
            SEND_ALL_ODATA,
            RECV_ALL_IDATA,
            RECV_ALL_ODATA,
            SHOW_IDATA,
            SHOW_ODATA,
            SEND_SERIAL,
            CLOSE_SOCKET,
            CLEAR_SCREEN,
            SAVE_TO_DISK,
            TOGGLE_OUTPUTS,
            GET_DIR,
            LCD_SHIFT_RIGHT,
            LCD_SHIFT_LEFT,
            SCROLL_UP,
            SCROLL_DOWN,
            ENABLE_LCD,
            SET_TIME,
            GET_TIME,
            TOTAL_UP_TIME,
            UPLOAD_NEW,
            GET_DEBUG_INFO,
            GET_DEBUG_INFO2,
            NEW_PASSWORD1,
            SET_SERIAL_RECV_ON,
            SET_SERIAL_RECV_OFF,
            TEST_ALL_IO,
            TEST_LEFT_BLINKER,
            TEST_RIGHT_BLINKER,
            RE_ENTER_PASSWORD,
            CLOSE_DB,
            OPEN_DB,
            BAD_MSG,
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
            btn_SendData.Enabled = false;
            btn_CloseDB.Enabled = false;
            button3.Enabled = false;
            button2.Enabled = false;
            tbConnected.Text = "not connected";
            target_db_closed = false;
            Load_Grid(currentconnectionString);
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
            btn_SendData.Enabled = true;
            btn_CloseDB.Enabled = true;
            btnConnect.Text = "Disconnect";
            button3.Enabled = true;
            button2.Enabled = true;
            cbSelectRecord.Enabled = true;
        }
        public void OnDisconnect(INetworkClient client)
        {
            int i;
            tbHostname.Enabled = true;
            tbPort.Enabled = true;
            cblistCommon.Enabled = false;
            btnConnect.Text = "Connect";
            btn_SendData.Enabled = false;
            btn_CloseDB.Enabled = false;
            tbConnected.Text = "not connected";
            button3.Enabled = false;
            button2.Enabled = false;
            cbSelectRecord.Enabled = false;
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
            int recno;
            int type_msg;
            string ret = null;
            char[] chars = new char[bytes.Length / sizeof(char) + 2];
            char[] chars2 = new char[bytes.Length / sizeof(char)];
            // src srcoffset dest destoffset len
            System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
            recno = (int)chars[0];
            type_msg = (int)chars[1];
            string str_recno = recno.ToString();
            //            AddMsg(recno.ToString() + " " + type_msg.ToString());
            System.Buffer.BlockCopy(bytes, 4, chars2, 0, bytes.Length - 4);
            ret = new string(chars2);
            //            MessageBox.Show(ret);
            //            AddMsg(ret + "\r\n");
            MessageBox.Show(bytes.Length.ToString() + " " + type_msg.ToString());
            //            if (type_msg == 1)  // sent single record (port, onoff, type, time_delay)
            switch (type_msg)
            {
                case 0:
                    int i = 0;
                    int port = 0;
                    int onoff = 0;
                    int type = 0;
                    int time_delay = 0;
                    string label = "";
                    string[] words = ret.Split(' ');
                    foreach (var word in words)
                    {
                        //                    temp = int.Parse(word);
                        switch (i)
                        {
                            case 0:
                                label = (string)word;
                                break;
                            case 1:
                                port = int.Parse(word);
                                break;
                            case 2:
                                onoff = int.Parse(word);
                                break;
                            case 3:
                                type = int.Parse(word);
                                break;
                            case 4:
                                time_delay = int.Parse(word);
                                Update_Record(port, label, onoff, type, time_delay);
                                break;
                            default:
                                break;
                        }
                        i++;
                    }
                    break;
                case 1:
                    AddMsg("msg 1");
                    break;
                case 2:
                    AddMsg("msg 2");
                    break;
                case 3:
                    AddMsg("msg 3");
                    break;
                case 4:
                    AddMsg("msg 4");
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
                Delete_O_DATA();
                table = MakeNamesTable();
                sqlCnn.Open();
                //              MessageBox.Show(ds2.DataSetName);
                for (i = 0; i < 40; i++)
                {
                    row = table.NewRow();
                    row["label"] = "asdf";
                    row["onoff"] = i + 6;
                    row["polarity"] = i + 5;
                    row["type"] = i + 4;
                    row["time_delay"] = i + 3;
                    row["time_left"] = i + 2;
                    row["pulse_time"] = i + 1;
                    row["reset"] = i;
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
                xmlFile = XmlReader.Create("C:\\Users\\Dan_Laptop\\dev\\odata.xml", new XmlReaderSettings());
                //xmlFile = XmlReader.Create("C:\\Users\\Daniel\\dev\\odata.xml", new XmlReaderSettings());
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
                adapter.Fill(ds2);

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
        private void btn_Update_Click(object sender, EventArgs e)
        {
            try
            {
                this.Validate();
                bindingSource1.EndEdit();
                //Delete_O_DATA();
                //                da.Update((DataTable)bindingSource1.DataSource);
                //AddMsg(da.Container.Components.Count.ToString());
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
                sql = "Select label, port, onoff, polarity, type, time_delay, time_left, pulse_time, reset from O_DATA";
            else
                sql = "Select label, port, onoff, polarity, type, time_delay, time_left, pulse_time, reset from O_DATA2";

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
                dataGridView1.Columns["port"].ReadOnly = true;
                //                dataGridView1.Columns["label"].MinimumWidth = 150;
                dataGridView1.Columns["label"].Width = 150;
                dataGridView1.Columns["port"].Width = 50;
                dataGridView1.Columns["onoff"].Width = 50;
                dataGridView1.Columns["polarity"].Width = 50;
                dataGridView1.Columns["type"].Width = 50;
                dataGridView1.Columns["time_delay"].Width = 50;
                dataGridView1.Columns["time_left"].Width = 50;
                dataGridView1.Columns["pulse_time"].Width = 50;
                dataGridView1.Columns["reset"].Width = 50;

                sqlCnn.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

        }
        private void Btn_SendData_Click(object sender, EventArgs e)
        {
            int i, j, k;
            String str;

            //            da.Fill(ds);

            for (i = 0; i < 40; i++)
            {
                str = ds.Tables[0].Rows[i].ItemArray[0].ToString();
                byte[] bytes = BytesFromString(str);
                Packet packet = new Packet(bytes, 0, bytes.Count(), false);
                m_client.Send(packet);

                for (j = 1; j < 9; j++)
                {
                    str = ds.Tables[0].Rows[i].ItemArray[j].ToString();
                    //MessageBox.Show(str);
                    bytes = BytesFromString(str);
                    packet = new Packet(bytes, 0, bytes.Count(), false);
                    m_client.Send(packet);
                }
            }
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
                default:
                    return;
            }
            bytes.SetValue((byte)sendcmd, 0);
            AddMsg(Enum.GetName(typeof(Server_cmds), sendcmd));
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
        // send a single record just consisting of port, onoff, type & time_delay
        private void cbSelectRecordChanged(object sender, EventArgs e)
        {
            int i;
            i = cbSelectRecord.SelectedIndex;
            //DataGridViewCellCollection cell = new DataGridViewCellCollection(dataGridView1.Rows[i]);
            //MessageBox.Show(dataGridView1.Rows[i].Cells[2].Value.ToString());
            int onoff = (int)dataGridView1.Rows[i].Cells[2].Value;
            int type = (int)dataGridView1.Rows[i].Cells[4].Value;
            int time_delay = (int)dataGridView1.Rows[i].Cells[5].Value;
            //MessageBox.Show(onoff.ToString() + " " + type.ToString() + " " + time_delay.ToString());
            string sendstr = i.ToString() + " " + onoff.ToString() + " " + type.ToString() + " " + time_delay.ToString();
            byte[] bytes = BytesFromString(sendstr);
            Packet packet = new Packet(bytes, 0, bytes.Count(), false);
            m_client.Send(packet);
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
            btn_Update_Click(sender, e);
        }
        private void sendData(object sender, EventArgs e)
        {
            Btn_SendData_Click(sender, e);
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
    }
}