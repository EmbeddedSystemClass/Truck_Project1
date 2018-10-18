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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.connectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.readXMLToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.writeXMLToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newDBToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.restoreMainDBToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.clearDataSetToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openDBToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openXMLToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.optionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.showDBToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.updateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sendDataToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.shutdownServerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.rebootServerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setServerTimeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.label3 = new System.Windows.Forms.Label();
            this.tbConnectionString = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.cbSelectRecord = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.cblistCommon = new System.Windows.Forms.CheckedListBox();
            this.btn_SetTime = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.tbServerTime = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.btn_CloseDB = new System.Windows.Forms.Button();
            this.tbCurrentTable = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.btnSwitchTable = new System.Windows.Forms.Button();
            this.btnDeleteO_DATA = new System.Windows.Forms.Button();
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
            this.tbReceived.Location = new System.Drawing.Point(16, 220);
            this.tbReceived.Multiline = true;
            this.tbReceived.Name = "tbReceived";
            this.tbReceived.ReadOnly = true;
            this.tbReceived.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbReceived.Size = new System.Drawing.Size(332, 385);
            this.tbReceived.TabIndex = 18;
            this.tbReceived.TabStop = false;
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(15, 33);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(75, 23);
            this.btnConnect.TabIndex = 0;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(283, 7);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(28, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "port:";
            // 
            // tbPort
            // 
            this.tbPort.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbPort.Location = new System.Drawing.Point(317, 4);
            this.tbPort.Name = "tbPort";
            this.tbPort.Size = new System.Drawing.Size(43, 20);
            this.tbPort.TabIndex = 12;
            this.tbPort.Text = "5193";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(128, 7);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "hostname:";
            // 
            // tbHostname
            // 
            this.tbHostname.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbHostname.Location = new System.Drawing.Point(184, 4);
            this.tbHostname.Name = "tbHostname";
            this.tbHostname.Size = new System.Drawing.Size(93, 20);
            this.tbHostname.TabIndex = 11;
            this.tbHostname.Text = "192.168.42.145";
            // 
            // btnXML
            // 
            this.btnXML.Location = new System.Drawing.Point(445, 33);
            this.btnXML.Name = "btnXML";
            this.btnXML.Size = new System.Drawing.Size(75, 23);
            this.btnXML.TabIndex = 5;
            this.btnXML.Text = "Read XML";
            this.btnXML.UseVisualStyleBackColor = true;
            this.btnXML.Click += new System.EventHandler(this.btn_XML_Click);
            // 
            // dataGridView1
            // 
            this.dataGridView1.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Location = new System.Drawing.Point(368, 94);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size(787, 511);
            this.dataGridView1.TabIndex = 15;
            this.dataGridView1.TabStop = false;
            // 
            // ds
            // 
            this.ds.DataSetName = "NewDataSet";
            // 
            // btnShow
            // 
            this.btnShow.Location = new System.Drawing.Point(531, 32);
            this.btnShow.Name = "btnShow";
            this.btnShow.Size = new System.Drawing.Size(75, 23);
            this.btnShow.TabIndex = 6;
            this.btnShow.Text = "ShowDB";
            this.btnShow.UseVisualStyleBackColor = true;
            this.btnShow.Click += new System.EventHandler(this.btn_Show_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(617, 32);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 7;
            this.button1.Text = "Update";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.btn_Update_Click);
            // 
            // btn_SendData
            // 
            this.btn_SendData.Location = new System.Drawing.Point(359, 34);
            this.btn_SendData.Name = "btn_SendData";
            this.btn_SendData.Size = new System.Drawing.Size(75, 23);
            this.btn_SendData.TabIndex = 4;
            this.btn_SendData.Text = "Send Data";
            this.btn_SendData.UseVisualStyleBackColor = true;
            this.btn_SendData.Click += new System.EventHandler(this.Btn_SendData_Click);
            // 
            // tbConnected
            // 
            this.tbConnected.Location = new System.Drawing.Point(617, 6);
            this.tbConnected.Name = "tbConnected";
            this.tbConnected.ReadOnly = true;
            this.tbConnected.Size = new System.Drawing.Size(84, 20);
            this.tbConnected.TabIndex = 13;
            this.tbConnected.TabStop = false;
            this.tbConnected.Text = "not connected";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.optionsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1167, 24);
            this.menuStrip1.TabIndex = 16;
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
            this.clearDataSetToolStripMenuItem,
            this.openDBToolStripMenuItem,
            this.openXMLToolStripMenuItem});
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
            // openDBToolStripMenuItem
            // 
            this.openDBToolStripMenuItem.Name = "openDBToolStripMenuItem";
            this.openDBToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
            this.openDBToolStripMenuItem.Text = "Open DB";
            this.openDBToolStripMenuItem.Click += new System.EventHandler(this.OpenDB_Click);
            // 
            // openXMLToolStripMenuItem
            // 
            this.openXMLToolStripMenuItem.Name = "openXMLToolStripMenuItem";
            this.openXMLToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
            this.openXMLToolStripMenuItem.Text = "Open XML";
            // 
            // optionsToolStripMenuItem
            // 
            this.optionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.showDBToolStripMenuItem,
            this.updateToolStripMenuItem,
            this.sendDataToolStripMenuItem,
            this.shutdownServerToolStripMenuItem,
            this.rebootServerToolStripMenuItem,
            this.setServerTimeToolStripMenuItem});
            this.optionsToolStripMenuItem.Name = "optionsToolStripMenuItem";
            this.optionsToolStripMenuItem.Size = new System.Drawing.Size(61, 20);
            this.optionsToolStripMenuItem.Text = "Options";
            // 
            // showDBToolStripMenuItem
            // 
            this.showDBToolStripMenuItem.Name = "showDBToolStripMenuItem";
            this.showDBToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.showDBToolStripMenuItem.Text = "Show DB";
            this.showDBToolStripMenuItem.Click += new System.EventHandler(this.showDB);
            // 
            // updateToolStripMenuItem
            // 
            this.updateToolStripMenuItem.Name = "updateToolStripMenuItem";
            this.updateToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.updateToolStripMenuItem.Text = "Update";
            this.updateToolStripMenuItem.Click += new System.EventHandler(this.Update);
            // 
            // sendDataToolStripMenuItem
            // 
            this.sendDataToolStripMenuItem.Name = "sendDataToolStripMenuItem";
            this.sendDataToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.sendDataToolStripMenuItem.Text = "Send Data";
            this.sendDataToolStripMenuItem.Click += new System.EventHandler(this.sendData);
            // 
            // shutdownServerToolStripMenuItem
            // 
            this.shutdownServerToolStripMenuItem.Name = "shutdownServerToolStripMenuItem";
            this.shutdownServerToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.shutdownServerToolStripMenuItem.Text = "Shutdown Server";
            this.shutdownServerToolStripMenuItem.Click += new System.EventHandler(this.shutdownServerToolStripMenuItem_Click);
            // 
            // rebootServerToolStripMenuItem
            // 
            this.rebootServerToolStripMenuItem.Name = "rebootServerToolStripMenuItem";
            this.rebootServerToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.rebootServerToolStripMenuItem.Text = "Reboot Server";
            this.rebootServerToolStripMenuItem.Click += new System.EventHandler(this.rebootServerToolStripMenuItem_Click);
            // 
            // setServerTimeToolStripMenuItem
            // 
            this.setServerTimeToolStripMenuItem.Name = "setServerTimeToolStripMenuItem";
            this.setServerTimeToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.setServerTimeToolStripMenuItem.Text = "Set Server Time";
            this.setServerTimeToolStripMenuItem.Click += new System.EventHandler(this.setServerTimeToolStripMenuItem_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(563, 8);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(38, 13);
            this.label3.TabIndex = 7;
            this.label3.Text = "status:";
            // 
            // tbConnectionString
            // 
            this.tbConnectionString.Location = new System.Drawing.Point(440, 4);
            this.tbConnectionString.Name = "tbConnectionString";
            this.tbConnectionString.ReadOnly = true;
            this.tbConnectionString.Size = new System.Drawing.Size(111, 20);
            this.tbConnectionString.TabIndex = 12;
            this.tbConnectionString.TabStop = false;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(374, 7);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(60, 13);
            this.label4.TabIndex = 5;
            this.label4.Text = "connection";
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
            this.cbSelectRecord.Location = new System.Drawing.Point(933, 34);
            this.cbSelectRecord.Name = "cbSelectRecord";
            this.cbSelectRecord.Size = new System.Drawing.Size(222, 21);
            this.cbSelectRecord.TabIndex = 10;
            this.cbSelectRecord.TabStop = false;
            this.cbSelectRecord.SelectedIndexChanged += new System.EventHandler(this.cbSelectRecordChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(815, 37);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(112, 13);
            this.label5.TabIndex = 17;
            this.label5.Text = "Update Single Record";
            // 
            // cblistCommon
            // 
            this.cblistCommon.CheckOnClick = true;
            this.cblistCommon.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
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
            this.cblistCommon.Size = new System.Drawing.Size(138, 106);
            this.cblistCommon.TabIndex = 9;
            this.cblistCommon.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.cblist_ItemCheck);
            this.cblistCommon.Click += new System.EventHandler(this.cblist_Clicked);
            this.cblistCommon.SelectedValueChanged += new System.EventHandler(this.cblist_SelValueChanged);
            // 
            // btn_SetTime
            // 
            this.btn_SetTime.Location = new System.Drawing.Point(273, 34);
            this.btn_SetTime.Name = "btn_SetTime";
            this.btn_SetTime.Size = new System.Drawing.Size(75, 23);
            this.btn_SetTime.TabIndex = 3;
            this.btn_SetTime.Text = "Set Time";
            this.btn_SetTime.UseVisualStyleBackColor = true;
            this.btn_SetTime.Click += new System.EventHandler(this.btn_SetTime_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(187, 34);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 23);
            this.button3.TabIndex = 2;
            this.button3.Text = "Off Engine";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click_1);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(101, 34);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 1;
            this.button2.Text = "Start Engine";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // tbServerTime
            // 
            this.tbServerTime.Location = new System.Drawing.Point(1055, 0);
            this.tbServerTime.Name = "tbServerTime";
            this.tbServerTime.ReadOnly = true;
            this.tbServerTime.Size = new System.Drawing.Size(100, 20);
            this.tbServerTime.TabIndex = 19;
            this.tbServerTime.TabStop = false;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(988, 1);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(61, 13);
            this.label6.TabIndex = 20;
            this.label6.Text = "server time:";
            // 
            // btn_CloseDB
            // 
            this.btn_CloseDB.Location = new System.Drawing.Point(698, 32);
            this.btn_CloseDB.Name = "btn_CloseDB";
            this.btn_CloseDB.Size = new System.Drawing.Size(94, 23);
            this.btn_CloseDB.TabIndex = 8;
            this.btn_CloseDB.Text = "Close Target DB";
            this.btn_CloseDB.UseVisualStyleBackColor = true;
            this.btn_CloseDB.Click += new System.EventHandler(this.btn_CloseDB_Click);
            // 
            // tbCurrentTable
            // 
            this.tbCurrentTable.Location = new System.Drawing.Point(469, 62);
            this.tbCurrentTable.Name = "tbCurrentTable";
            this.tbCurrentTable.Size = new System.Drawing.Size(69, 20);
            this.tbCurrentTable.TabIndex = 21;
            this.tbCurrentTable.Text = "O_DATA";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(394, 65);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(69, 13);
            this.label7.TabIndex = 22;
            this.label7.Text = "current table:";
            // 
            // btnSwitchTable
            // 
            this.btnSwitchTable.Location = new System.Drawing.Point(554, 62);
            this.btnSwitchTable.Name = "btnSwitchTable";
            this.btnSwitchTable.Size = new System.Drawing.Size(75, 23);
            this.btnSwitchTable.TabIndex = 23;
            this.btnSwitchTable.Text = "Switch Table";
            this.btnSwitchTable.UseVisualStyleBackColor = true;
            this.btnSwitchTable.Click += new System.EventHandler(this.SwitchTable_Click);
            // 
            // btnDeleteO_DATA
            // 
            this.btnDeleteO_DATA.Location = new System.Drawing.Point(648, 62);
            this.btnDeleteO_DATA.Name = "btnDeleteO_DATA";
            this.btnDeleteO_DATA.Size = new System.Drawing.Size(75, 23);
            this.btnDeleteO_DATA.TabIndex = 24;
            this.btnDeleteO_DATA.Text = "Delete Current O_DATA";
            this.btnDeleteO_DATA.UseVisualStyleBackColor = true;
            this.btnDeleteO_DATA.Click += new System.EventHandler(this.DeleteO_DATA_Click);
            // 
            // FrmSampleClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1167, 617);
            this.Controls.Add(this.btnDeleteO_DATA);
            this.Controls.Add(this.btnSwitchTable);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.tbCurrentTable);
            this.Controls.Add(this.btn_CloseDB);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.tbServerTime);
            this.Controls.Add(this.btn_SetTime);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.cblistCommon);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.cbSelectRecord);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.tbConnectionString);
            this.Controls.Add(this.label3);
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
//        private System.Data.SqlClient.SqlCommand cmd;


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
        private System.Windows.Forms.ComboBox cbSelectRecord;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckedListBox cblistCommon;
        private System.Windows.Forms.ToolStripMenuItem openDBToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openXMLToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem shutdownServerToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem rebootServerToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem setServerTimeToolStripMenuItem;
        private System.Windows.Forms.Button btn_SetTime;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.TextBox tbServerTime;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btn_CloseDB;
        private System.Windows.Forms.TextBox tbCurrentTable;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button btnSwitchTable;
        private System.Windows.Forms.Button btnDeleteO_DATA;
    }
}

