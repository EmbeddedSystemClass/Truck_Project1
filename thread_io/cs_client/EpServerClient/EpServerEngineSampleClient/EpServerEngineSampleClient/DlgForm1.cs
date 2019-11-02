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
        private System.Windows.Forms.BindingSource bindingSource1;
        //string currentconnectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL2.mdf;Integrated Security=True;Connect Timeout=30";
        //string connectionString =        "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL2.mdf;Integrated Security=True;Connect Timeout=30";
        //string connstr_prefix = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=";
        string pathName = @"c:\Program Files (x86)\Microsoft SQL Server\Mssql.1\Mssql\Data";
        string fileName1 = "Client-SQL.mdf";
        //string fileName2 = "Client-SQL2.mdf";
        string connstr_prefix = "Data Source = NEWWINDOWS\\SQLEXPRESS;AttachDbFilename=";
        string connstr_suffix = ";Integrated Security = True; Connect Timeout = 30";
        string currentconnectionString = "";
        string connectionString = "";
        string curr_table_name = "O_DATA";

        private INetworkClient m_client;
        ServerCmds svrcmd = new ServerCmds();

        public DlgForm1()
        {
            InitializeComponent();
            btn_SendSelectedRecords.Enabled = true;
            bindingSource1 = new BindingSource();
            currentconnectionString = connstr_prefix + pathName + fileName1 + connstr_suffix;
            connectionString = currentconnectionString;
            tbCurTable.Text = curr_table_name;
            AddMsg(curr_table_name);
            //            currentconnectionString = connectionStringlt;
            //if(m_client.IsConnectionAlive)
            //    svrcmd.SetClient(m_client);
        }
        delegate void AddMsg_Involk(string message);
        public void AddMsg(string message)
        {
            if (tbStatus.InvokeRequired)
            {
                AddMsg_Involk CI = new AddMsg_Involk(AddMsg);
                tbStatus.Invoke(CI, message);
            }
            else
            {
                //tbReceived.Text += message + "\r\n";
                tbStatus.AppendText(message + "\r\n");
            }
        }
        byte[] BytesFromString(String str)
        {
            byte[] bytes = new byte[str.Length * sizeof(char)];
            System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
            return bytes;
        }

        public void SetClient(INetworkClient client)
        {
            m_client = client;
            svrcmd.SetClient(m_client);
        }
        // this gives an error when LoadDB is called (Open doesn't work) so I hid this button
        private void btn_ListDB_Click(object sender, EventArgs e)
        {
            ChangeDB testDialog = new ChangeDB(currentconnectionString);
            // Show testDialog as a modal dialog and determine if DialogResult = OK.
            if (testDialog.ShowDialog(this) == DialogResult.OK)
            {
                string str = testDialog.returnStr;
                currentconnectionString = connstr_prefix + str + connstr_suffix;
                AddMsg(currentconnectionString);
            }
            else
            {
                currentconnectionString = connstr_prefix + pathName + fileName1 + connstr_suffix;
            }
            //            MessageBox.Show(currentconnectionString);
            testDialog.Dispose();
        }
        private void btnListTables_Click(object sender, EventArgs e)
        {
            SwitchTable swtables = new SwitchTable(currentconnectionString);
            if(swtables.ShowDialog(this) == DialogResult.OK)
            {
                LoadDb(swtables.returnStr);
                curr_table_name = swtables.returnStr;
                tbCurTable.Text = curr_table_name;
            }
        }
        private void DlgForm1_Load(object sender, EventArgs e)
        {
            // TODO: This line of code loads data into the '_Client_SQL2DataSet1.O_DATA' table. You can move, or remove it, as needed.
            this.o_DATATableAdapter.Fill(this._Client_SQL2DataSet1.O_DATA);
            AddMsg(this._Client_SQL2DataSet1.O_DATA.ToString());
            //            this.o_DATATableAdapter1.Fill(this._Client_SQL_DBDataSet.O_DATA);		// comment this out when running on desktop
            //this.o_DATATableAdapter.Fill(this._Client_SQL_DB2DataSet.O_DATA);
        }
        private void Update_Record(int port, string label, int onoff, int type, int time_delay)
        {
            try
            {
                using (SqlConnection conn =
                    new SqlConnection(currentconnectionString))
                {
                    conn.Open();
                    System.Data.SqlClient.SqlCommand cmd;
                        cmd = new System.Data.SqlClient.SqlCommand("UPDATE O_DATA SET label=@label, onoff=@onoff, polarity=@polarity, type=@type, time_delay= @time_delay,time_left=@time_left, reset=@reset WHERE port=@port", conn);
//                        cmd = new System.Data.SqlClient.SqlCommand("UPDATE O_DATA2 SET label=@label, onoff=@onoff, polarity=@polarity, type=@type, time_delay= @time_delay,time_left=@time_left, reset=@reset WHERE port=@port", conn);
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
            //            sql = "select * from O_DATA";
            sql = "select * from " + curr_table_name;
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
        private void LoadDb(string str_table)
        {
            SqlConnection sqlCnn;
            SqlCommand sqlCmd;
            SqlDataAdapter adapter = new SqlDataAdapter();
            DataSet ds2 = new DataSet();
            string sql = null;
            sql = "Select * from " + str_table;
            sqlCnn = new SqlConnection(currentconnectionString);
            tbCurTable.Text = sql;
            try
            {
                sqlCnn.Open();
                sqlCmd = new SqlCommand(sql, sqlCnn);
                adapter.SelectCommand = sqlCmd;
                adapter.Fill(ds2);  // fill dataset with records
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
                currentconnectionString = connectionString;
                AddMsg(currentconnectionString);
            }
        }

        private void btnShow_Click(object sender, EventArgs e)
        {
            LoadDb(curr_table_name);
            //SqlConnection sqlCnn;
            //SqlCommand sqlCmd;
            //SqlDataAdapter adapter = new SqlDataAdapter();
            //DataSet ds2 = new DataSet();
            //string sql = null;
            //if (use_main_odata)
            //    sql = "Select * from O_DATA";
            //else
            //    sql = "Select * from O_DATA2";
            //sqlCnn = new SqlConnection(currentconnectionString);
            //try
            //{
            //    sqlCnn.Open();
            //    sqlCmd = new SqlCommand(sql, sqlCnn);
            //    adapter.SelectCommand = sqlCmd;
            //    adapter.Fill(ds2);  // fill dataset with records

            //    //MessageBox.Show(ds2.DataSetName);
            //    dataGridView1.DataSource = ds2.Tables[1];
            //    //for (i = 0; i <= ds2.Tables[0].Rows.Count - 1; i++)
            //    //{
            //    //    MessageBox.Show(ds2.Tables[0].Rows[i].ItemArray[0] + " " + ds2.Tables[0].Rows[i].ItemArray[1]);
            //    //}
            //    adapter.Dispose();
            //    sqlCmd.Dispose();
            //    sqlCnn.Close();
            //}
            //catch (Exception ex)
            //{
            //    MessageBox.Show(ex.Message);
            //}
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
                sql = "delete " + curr_table_name;

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
                dataGridView1.DataSource = ds2.Tables[0];
				//ds2.Tables[0].Rows[]
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
            string label = "";

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
//                        System.Text.StringBuilder sb = new System.Text.StringBuilder();
                        for (int i = 0; i < selectedRowCount; i++)
                        {
                            //                            sb.Append("Row: ");
                            //                            sb.Append(dataGridView1.SelectedRows[i].Cells[0].Value.ToString());
                            cell = dataGridView1.SelectedCells[i].RowIndex;
                            dataGridView1.SelectedCells[i].Selected = false;
                            //MessageBox.Show(i.ToString() + " " + cell.ToString());
                            label = (string)dataGridView1.Rows[cell].Cells[0].Value;
//                            MessageBox.Show(label);
                            port = (int)dataGridView1.Rows[cell].Cells[1].Value;
                            onoff = (int)dataGridView1.Rows[cell].Cells[2].Value;
                            if (onoff > 0)
                                onoff = 1;
                            input_port = (int)dataGridView1.Rows[cell].Cells[3].Value;
                            type = (int)dataGridView1.Rows[cell].Cells[4].Value;
                            if (type > 5)
                            {
                                //                                    AddMsg("type cannot be > 5");
                                type = 0;
                            }
                            time_delay = (int)dataGridView1.Rows[cell].Cells[5].Value;

                            string sendstr = port.ToString() + '{' + onoff.ToString() + '|' +
                                type.ToString() + '}' + time_delay.ToString() + '~' + input_port.ToString() + 'z' + label + "\0" + " ";

                            //sb.Append("port: " + port.ToString() + " onoff: " + onoff.ToString() + " type: " 
                            //    + type.ToString() + " td: " + time_delay.ToString() + " input: " + input_port.ToString());
                            //sb.Append(Environment.NewLine);
                            //MessageBox.Show(sb.ToString());
                            byte[] bytes = BytesFromString(sendstr);
                            byte[] bytes2 = new byte[bytes.Count() + 2];
                            System.Buffer.BlockCopy(bytes, 0, bytes2, 2, bytes.Length - 2);
                            string str = "SEND_ODATA";
                            //            bytes2[0] = (byte)Server_cmds.SET_TIME;
                            bytes2[0] = svrcmd.GetCmdIndexB(str);
                            Packet packet = new Packet(bytes2, 0, bytes2.Count(), false);
                            m_client.Send(packet);
//                            MessageBox.Show(input_port.ToString());
                        }
                        //                        AddMsg(sb.ToString());
                        //sb.Clear();
                        btn_SendSelectedRecords.Enabled = false;
                    }
                }
            }
        }
        private void btn_NewTable_Click(object sender, EventArgs e)
        {
            Update_Table(0, "");
        }
        private void Btn_SaveDB_Click(object sender, EventArgs e)
        {
            //            string cmd = Enum.GetName(typeof(Server_cmds), Server_cmds.SAVE_TO_DISK);
            string cmd = "SAVE_TO_DISK";

            //if (m_client.IsConnectionAlive)
                //svrcmd.Send_Cmd(cmd, 0);
        }
        private void btnDeleteO_DATA_Click(object sender, EventArgs e)
        {
            Delete_O_DATA();
        }
        private void SelectionChanged(object sender, EventArgs e)
        {
            btn_SendSelectedRecords.Enabled = true;
        }
    }
}