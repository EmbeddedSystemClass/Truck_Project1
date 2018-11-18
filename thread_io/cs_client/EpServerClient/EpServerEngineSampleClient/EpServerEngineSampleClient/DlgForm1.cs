using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.SqlClient;
using System.Xml;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EpLibrary.cs;
using EpServerEngine.cs;

namespace EpServerEngineSampleClient
{
    public partial class DlgForm1 : Form
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
        private System.Windows.Forms.BindingSource bindingSource1;
        string currentconnectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL-DB2.mdf;Integrated Security=True;Connect Timeout=30";
        string connectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL-DB2.mdf;Integrated Security=True;Connect Timeout=30";
        string connectionString2 = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL-DB3.mdf;Integrated Security=True;Connect Timeout=30";
        string connectionStringlt = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Dan_Laptop\\dev\\Client-SQL.mdf;Integrated Security=True;Connect Timeout=30";
        private bool use_main_odata;
        private bool use_laptop = false;
        public INetworkClient m_client;

        public string getStr()
        {
            if (str == null)
                return "test";
            return str;
        }
        private string str = null;

        public DlgForm1()
        {
            InitializeComponent();
            use_main_odata = true;
            btn_CloseDB.Enabled = false;
            btn_SendSelectedRecords.Enabled = false;
            bindingSource1 = new BindingSource();
            currentconnectionString = connectionString;
        }
        byte[] BytesFromString(String str)
        {
            byte[] bytes = new byte[str.Length * sizeof(char)];
            System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
            return bytes;
        }

        private void DlgForm1_Load(object sender, EventArgs e)
        {
            // TODO: This line of code loads data into the '_Client_SQL_DB2DataSet.O_DATA' table. You can move, or remove it, as needed.
            this.o_DATATableAdapter.Fill(this._Client_SQL_DB2DataSet.O_DATA);
//            Load_Grid(currentconnectionString);
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
                //                AddMsg(ex.Message.ToString());
            }
        }
        private void Update_Table(int port, string data)
        {
            SqlConnection sqlCnn;
            SqlCommandBuilder cmdBuilder;
            //string sql ="INSERT INTO dbo.O_DATA (label, port, onoff, polarity, type, time_delay, time_left, pulse_time, reset) VALUES(@label,@port,@onoff,@polarity,@type, @time_delay,@time_left,@reset)";
            string sql = null;
            //            SqlDataAdapter adapter = new SqlDataAdapter();
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

                for (i = 0; i < 40; i++)
                {
                    row = table.NewRow();
                    row["label"] = dataGridView1.Rows[i].Cells[0].Value.ToString();
                    row["port"] = dataGridView1.Rows[i].Cells[1].Value;
                    row["onoff"] = dataGridView1.Rows[i].Cells[2].Value;
                    row["input_port"] = dataGridView1.Rows[i].Cells[3].Value;
                    //                    row["polarity"] = dataGridView1.Rows[i].Cells[4].Value;
                    row["polarity"] = 0;
                    row["type"] = dataGridView1.Rows[i].Cells[4].Value;
                    row["time_delay"] = dataGridView1.Rows[i].Cells[5].Value;
                    //row["time_left"] = dataGridView1.Rows[i].Cells[7].Value;
                    //row["pulse_time"] = dataGridView1.Rows[i].Cells[8].Value;
                    //row["reset"] = dataGridView1.Rows[i].Cells[9].Value;
                    row["time_left"] = 0;
                    row["pulse_time"] = 0;
                    row["reset"] = 0;
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
        private void btnShow_Click(object sender, EventArgs e)
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
                //              tbConnectionString.Text = tconstr;
                adapter = new SqlDataAdapter(sql, sqlCnn);
                dataGridView1.DataSource = bindingSource1;

                table.Locale = System.Globalization.CultureInfo.InvariantCulture;
                table.Clear();
                adapter.Fill(table);        // fill the table from the data adapter
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
        private void btmXML_Click(object sender, EventArgs e)
        {
            try
            {
                XmlReader xmlFile = null;
                DataSet ds2 = new DataSet();
                if (use_laptop == true)
                    xmlFile = XmlReader.Create("C:\\Users\\Dan_Laptop\\dev\\odata.xml", new XmlReaderSettings());
                else
                    xmlFile = XmlReader.Create("C:\\Users\\Daniel\\dev\\odata.xml", new XmlReaderSettings());
                ds2.ReadXml(xmlFile);
                dataGridView1.DataSource = ds2.Tables[0];
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
        private void btn_CloseDB_Click(object sender, EventArgs e)
        {
            string cmd = null;
            //if (target_db_closed == false)  // db on target is open so close it
            //{
            //    cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.CLOSE_DB);
            //    target_db_closed = true;
            //    btn_CloseDB.Text = "Open Target DB";
            //}
            //else
            //{
            //    cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.OPEN_DB);
            //    target_db_closed = false;
            //    btn_CloseDB.Text = "Close Target DB";
            //}

            //Send_Cmd(cmd, 0);

        }
        private void btn_SendSelectedRecords_Click(object sender, EventArgs e)
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
                    //                    tbReceived.Clear();
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
                                //                                    AddMsg("type cannot be > 5");
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
                        //                        AddMsg(sb.ToString());
                        sb.Clear();
                        btn_SendSelectedRecords.Enabled = false;
                    }
                }
            }
        }
        private void btnSwitchTable_Click(object sender, EventArgs e)
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
        private void btn_NewTable_Click(object sender, EventArgs e)
        {
            Update_Table(0, "");
        }
        private void Btn_SaveDB_Click(object sender, EventArgs e)
        {
            //            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.SAVE_TO_DISK);
            //          Send_Cmd(cmd, 0);

        }
        private void btnDeleteO_DATA_Click(object sender, EventArgs e)
        {
            Delete_O_DATA();
        }

        private void UseLaptop_Click(object sender, EventArgs e)
        {
            if (use_laptop == false)
            {
                use_laptop = true;
                btn_Laptop.Text = "Desktop";
                currentconnectionString = connectionStringlt;
            }
            else
            {
                use_laptop = false;
                btn_Laptop.Text = "Laptop";
                currentconnectionString = connectionString;
            }
        }
    }
}