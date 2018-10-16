namespace EpServerEngineSampleClient
{
    partial class FrmSampleClient
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.tbReceived = new System.Windows.Forms.TextBox();
            this.btnConnect = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.tbPort = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tbHostname = new System.Windows.Forms.TextBox();
            this.btnXML = new System.Windows.Forms.Button();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.ds = new System.Data.DataSet();
            this.bindingSource1 = new System.Windows.Forms.BindingSource(this.components);
            this.table = new System.Data.DataTable();
            this.btnShow = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.btn_SendData = new System.Windows.Forms.Button();
            this.tbConnected = new System.Windows.Forms.TextBox();
            this.btnClrScr = new System.Windows.Forms.Button();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.connectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.readXMLToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.writeXMLToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newDBToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.restoreMainDBToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.clearDataSetToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.optionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.showDBToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.updateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sendDataToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.label3 = new System.Windows.Forms.Label();
            this.tbConnectionString = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.cbSelectRecord = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.cblistCommon = new System.Windows.Forms.CheckedListBox();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ds)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.table)).BeginInit();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tbReceived
            // 
            this.tbReceived.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbReceived.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbReceived.Location = new System.Drawing.Point(4, 219);
            this.tbReceived.Multiline = true;
            this.tbReceived.Name = "tbReceived";
            this.tbReceived.ReadOnly = true;
            this.tbReceived.Size = new System.Drawing.Size(327, 122);
            this.tbReceived.TabIndex = 21;
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(210, 32);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(75, 23);
            this.btnConnect.TabIndex = 9;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(378, 7);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(28, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "port:";
            // 
            // tbPort
            // 
            this.tbPort.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbPort.Location = new System.Drawing.Point(412, 7);
            this.tbPort.Name = "tbPort";
            this.tbPort.Size = new System.Drawing.Size(42, 20);
            this.tbPort.TabIndex = 4;
            this.tbPort.Text = "8000";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(205, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "hostname:";
            // 
            // tbHostname
            // 
            this.tbHostname.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbHostname.Location = new System.Drawing.Point(261, 6);
            this.tbHostname.Name = "tbHostname";
            this.tbHostname.Size = new System.Drawing.Size(92, 20);
            this.tbHostname.TabIndex = 2;
            this.tbHostname.Text = "192.168.42.103";
            // 
            // btnXML
            // 
            this.btnXML.Location = new System.Drawing.Point(306, 33);
            this.btnXML.Name = "btnXML";
            this.btnXML.Size = new System.Drawing.Size(75, 23);
            this.btnXML.TabIndex = 10;
            this.btnXML.Text = "Read XML";
            this.btnXML.UseVisualStyleBackColor = true;
            this.btnXML.Click += new System.EventHandler(this.btn_XML_Click);
            // 
            // dataGridView1
            // 
            this.dataGridView1.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Location = new System.Drawing.Point(368, 62);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size(589, 353);
            this.dataGridView1.TabIndex = 19;
            // 
            // ds
            // 
            this.ds.DataSetName = "NewDataSet";
            // 
            // btnShow
            // 
            this.btnShow.Location = new System.Drawing.Point(402, 32);
            this.btnShow.Name = "btnShow";
            this.btnShow.Size = new System.Drawing.Size(75, 23);
            this.btnShow.TabIndex = 11;
            this.btnShow.Text = "ShowDB";
            this.btnShow.UseVisualStyleBackColor = true;
            this.btnShow.Click += new System.EventHandler(this.btn_Show_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(498, 32);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 12;
            this.button1.Text = "Update";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.btn_Update_Click);
            // 
            // btn_SendData
            // 
            this.btn_SendData.Location = new System.Drawing.Point(594, 33);
            this.btn_SendData.Name = "btn_SendData";
            this.btn_SendData.Size = new System.Drawing.Size(75, 23);
            this.btn_SendData.TabIndex = 13;
            this.btn_SendData.Text = "Send Data";
            this.btn_SendData.UseVisualStyleBackColor = true;
            this.btn_SendData.Click += new System.EventHandler(this.Btn_SendData_Click);
            // 
            // tbConnected
            // 
            this.tbConnected.Location = new System.Drawing.Point(727, 7);
            this.tbConnected.Name = "tbConnected";
            this.tbConnected.Size = new System.Drawing.Size(100, 20);
            this.tbConnected.TabIndex = 8;
            this.tbConnected.Text = "not connected";
            // 
            // btnClrScr
            // 
            this.btnClrScr.Location = new System.Drawing.Point(690, 32);
            this.btnClrScr.Name = "btnClrScr";
            this.btnClrScr.Size = new System.Drawing.Size(75, 23);
            this.btnClrScr.TabIndex = 14;
            this.btnClrScr.Text = "clrscr";
            this.btnClrScr.UseVisualStyleBackColor = true;
            this.btnClrScr.Click += new System.EventHandler(this.btn_ClrScr_Click);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.optionsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(970, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.connectToolStripMenuItem,
            this.readXMLToolStripMenuItem,
            this.writeXMLToolStripMenuItem,
            this.newDBToolStripMenuItem,
            this.restoreMainDBToolStripMenuItem,
            this.clearDataSetToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // connectToolStripMenuItem
            // 
            this.connectToolStripMenuItem.Name = "connectToolStripMenuItem";
            this.connectToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
            this.connectToolStripMenuItem.Text = "Connect";
            this.connectToolStripMenuItem.Click += new System.EventHandler(this.connect);
            // 
            // readXMLToolStripMenuItem
            // 
            this.readXMLToolStripMenuItem.Name = "readXMLToolStripMenuItem";
            this.readXMLToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
            this.readXMLToolStripMenuItem.Text = "Read XML";
            this.readXMLToolStripMenuItem.Click += new System.EventHandler(this.readXML);
            // 
            // writeXMLToolStripMenuItem
            // 
            this.writeXMLToolStripMenuItem.Name = "writeXMLToolStripMenuItem";
            this.writeXMLToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
            this.writeXMLToolStripMenuItem.Text = "Write XML";
            this.writeXMLToolStripMenuItem.Click += new System.EventHandler(this.writeXML);
            // 
            // newDBToolStripMenuItem
            // 
            this.newDBToolStripMenuItem.Name = "newDBToolStripMenuItem";
            this.newDBToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
            this.newDBToolStripMenuItem.Text = "Load temp DB";
            this.newDBToolStripMenuItem.Click += new System.EventHandler(this.newDB);
            // 
            // restoreMainDBToolStripMenuItem
            // 
            this.restoreMainDBToolStripMenuItem.Name = "restoreMainDBToolStripMenuItem";
            this.restoreMainDBToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
            this.restoreMainDBToolStripMenuItem.Text = "Restore main DB";
            this.restoreMainDBToolStripMenuItem.Click += new System.EventHandler(this.restoreMainDBToolStripMenuItem_Click);
            // 
            // clearDataSetToolStripMenuItem
            // 
            this.clearDataSetToolStripMenuItem.Name = "clearDataSetToolStripMenuItem";
            this.clearDataSetToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
            this.clearDataSetToolStripMenuItem.Text = "Clear DataSet";
            this.clearDataSetToolStripMenuItem.Click += new System.EventHandler(this.clearDataSetToolStripMenuItem_Click);
            // 
            // optionsToolStripMenuItem
            // 
            this.optionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.showDBToolStripMenuItem,
            this.updateToolStripMenuItem,
            this.sendDataToolStripMenuItem});
            this.optionsToolStripMenuItem.Name = "optionsToolStripMenuItem";
            this.optionsToolStripMenuItem.Size = new System.Drawing.Size(61, 20);
            this.optionsToolStripMenuItem.Text = "Options";
            // 
            // showDBToolStripMenuItem
            // 
            this.showDBToolStripMenuItem.Name = "showDBToolStripMenuItem";
            this.showDBToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
            this.showDBToolStripMenuItem.Text = "Show DB";
            this.showDBToolStripMenuItem.Click += new System.EventHandler(this.showDB);
            // 
            // updateToolStripMenuItem
            // 
            this.updateToolStripMenuItem.Name = "updateToolStripMenuItem";
            this.updateToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
            this.updateToolStripMenuItem.Text = "Update";
            this.updateToolStripMenuItem.Click += new System.EventHandler(this.Update);
            // 
            // sendDataToolStripMenuItem
            // 
            this.sendDataToolStripMenuItem.Name = "sendDataToolStripMenuItem";
            this.sendDataToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
            this.sendDataToolStripMenuItem.Text = "Send Data";
            this.sendDataToolStripMenuItem.Click += new System.EventHandler(this.sendData);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(680, 10);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(38, 13);
            this.label3.TabIndex = 7;
            this.label3.Text = "status:";
            // 
            // tbConnectionString
            // 
            this.tbConnectionString.Location = new System.Drawing.Point(540, 7);
            this.tbConnectionString.Name = "tbConnectionString";
            this.tbConnectionString.Size = new System.Drawing.Size(129, 20);
            this.tbConnectionString.TabIndex = 6;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(471, 9);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(60, 13);
            this.label4.TabIndex = 5;
            this.label4.Text = "connection";
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(786, 32);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 15;
            this.button2.Text = "button2";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click_1);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(882, 32);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 23);
            this.button3.TabIndex = 16;
            this.button3.Text = "button3";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // cbSelectRecord
            // 
            this.cbSelectRecord.FormattingEnabled = true;
            this.cbSelectRecord.Items.AddRange(new object[] {
            "STARTER",
            "ACCON",
            "FUELPUMP",
            "COOLINGFAN",
            "LHEADLAMPS",
            "LBRIGHTS",
            "RHEADLAMP",
            "RBRIGHTS",
            "LEFTBLINKER",
            "RIGHTBLINKER",
            "RUNNINGLIGHTS",
            "RBRAKELIGHT",
            "LBRAKELIGHT",
            "BATTERYCHARGERRELAY",
            "DASHHEATER",
            "BATTERTCOMPHEATER",
            "CRANKCASEHEATER",
            "ESTOPSWITCH",
            "TRAILERBRAKES",
            "TRAILERLEFTBLINKER",
            "TRAILERRIGHTBLINKER",
            "INTRUDERALARM",
            "LIGHTBAR",
            "BLINKINDICATOR",
            "ALARMSPEAKER",
            "BACKUPLIGHTS"});
            this.cbSelectRecord.Location = new System.Drawing.Point(131, 62);
            this.cbSelectRecord.Name = "cbSelectRecord";
            this.cbSelectRecord.Size = new System.Drawing.Size(222, 21);
            this.cbSelectRecord.TabIndex = 18;
            this.cbSelectRecord.SelectedIndexChanged += new System.EventHandler(this.cbSelectRecordChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(13, 65);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(112, 13);
            this.label5.TabIndex = 17;
            this.label5.Text = "Update Single Record";
            // 
            // cblistCommon
            // 
            this.cblistCommon.CheckOnClick = true;
            this.cblistCommon.FormattingEnabled = true;
            this.cblistCommon.Items.AddRange(new object[] {
            "STARTER",
            "IGNITION",
            "FUELPUMP",
            "COOLINGFAN",
            "LIGHTS",
            "BRIGHTS"});
            this.cblistCommon.Location = new System.Drawing.Point(16, 108);
            this.cblistCommon.Name = "cblistCommon";
            this.cblistCommon.Size = new System.Drawing.Size(109, 94);
            this.cblistCommon.TabIndex = 20;
            this.cblistCommon.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.cblist_ItemCheck);
            this.cblistCommon.Click += new System.EventHandler(this.cblist_Clicked);
            this.cblistCommon.SelectedValueChanged += new System.EventHandler(this.cblist_SelValueChanged);
            // 
            // FrmSampleClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(970, 424);
            this.Controls.Add(this.cblistCommon);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.cbSelectRecord);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.tbConnectionString);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.btnClrScr);
            this.Controls.Add(this.tbConnected);
            this.Controls.Add(this.btn_SendData);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.btnShow);
            this.Controls.Add(this.dataGridView1);
            this.Controls.Add(this.btnXML);
            this.Controls.Add(this.tbHostname);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.tbReceived);
            this.Controls.Add(this.btnConnect);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tbPort);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "FrmSampleClient";
            this.Text = "SampleClient";
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ds)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.table)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbReceived;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbPort;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbHostname;
        private System.Windows.Forms.Button btnXML;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Data.DataSet ds;
        private System.Data.SqlClient.SqlDataAdapter da;
        private System.Windows.Forms.BindingSource bindingSource1;
        private System.Windows.Forms.Button btnShow;
        private System.Data.DataTable table;
        private System.Data.SqlClient.SqlConnection conn;
        private System.Data.SqlClient.SqlCommand cmd;


        // desktop
        //string connectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL-DB.mdf;Integrated Security=True;Connect Timeout=30";
        // laptop
        string currentconnectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL-DB3.mdf;Integrated Security=True;Connect Timeout=30";
        string connectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\dev\\Client-SQL-DB3.mdf;Integrated Security=True;Connect Timeout=30";
        string connectionString2 = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Daniel\\Documents\\Client-SQL-DB2.mdf;Integrated Security=True;Connect Timeout=30";
/*
        string currentconnectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Dan_Laptop\\dev\\Client-SQL-DB.mdf;Integrated Security=True;Connect Timeout=30";
        string connectionString = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Dan_Laptop\\dev\\Client-SQL-DB.mdf;Integrated Security=True;Connect Timeout=30";
        string connectionString2 = "Data Source = (LocalDB)\\MSSQLLocalDB;AttachDbFilename=C:\\Users\\Dan_Laptop\\dev\\Client-SQL.mdf;Integrated Security=True;Connect Timeout=30";
*/
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button btn_SendData;
        private System.Windows.Forms.TextBox tbConnected;
        private System.Windows.Forms.Button btnClrScr;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem optionsToolStripMenuItem;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ToolStripMenuItem connectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem readXMLToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem writeXMLToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newDBToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem showDBToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem updateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sendDataToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem restoreMainDBToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem clearDataSetToolStripMenuItem;
        private System.Windows.Forms.TextBox tbConnectionString;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.ComboBox cbSelectRecord;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckedListBox cblistCommon;
    }
}

