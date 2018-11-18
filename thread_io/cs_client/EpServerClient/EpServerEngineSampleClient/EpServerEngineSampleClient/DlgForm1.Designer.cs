namespace EpServerEngineSampleClient
{
    partial class DlgForm1
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
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.oDATABindingSource = new System.Windows.Forms.BindingSource(this.components);
            this._Client_SQL_DB2DataSet = new EpServerEngineSampleClient._Client_SQL_DB2DataSet();
            this.o_DATATableAdapter = new EpServerEngineSampleClient._Client_SQL_DB2DataSetTableAdapters.O_DATATableAdapter();
            this.btn_SendSelectedRecords = new System.Windows.Forms.Button();
            this.btn_NewTable = new System.Windows.Forms.Button();
            this.Btn_SaveDB = new System.Windows.Forms.Button();
            this.btnDeleteO_DATA = new System.Windows.Forms.Button();
            this.btnSwitchTable = new System.Windows.Forms.Button();
            this.btn_CloseDB = new System.Windows.Forms.Button();
            this.btnShow = new System.Windows.Forms.Button();
            this.btnXML = new System.Windows.Forms.Button();
            this.table = new System.Data.DataTable();
            this.tbCurrentTable = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.labelDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.portDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.onoffDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.inputportDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.typeDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.timedelayDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.btn_Laptop = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.oDATABindingSource)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this._Client_SQL_DB2DataSet)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.table)).BeginInit();
            this.SuspendLayout();
            // 
            // dataGridView1
            // 
            this.dataGridView1.AutoGenerateColumns = false;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.labelDataGridViewTextBoxColumn,
            this.portDataGridViewTextBoxColumn,
            this.onoffDataGridViewTextBoxColumn,
            this.inputportDataGridViewTextBoxColumn,
            this.typeDataGridViewTextBoxColumn,
            this.timedelayDataGridViewTextBoxColumn});
            this.dataGridView1.DataSource = this.oDATABindingSource;
            this.dataGridView1.Location = new System.Drawing.Point(15, 70);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size(573, 388);
            this.dataGridView1.TabIndex = 2;
            // 
            // oDATABindingSource
            // 
            this.oDATABindingSource.DataMember = "O_DATA";
            this.oDATABindingSource.DataSource = this._Client_SQL_DB2DataSet;
            // 
            // _Client_SQL_DB2DataSet
            // 
            this._Client_SQL_DB2DataSet.DataSetName = "_Client_SQL_DB2DataSet";
            this._Client_SQL_DB2DataSet.SchemaSerializationMode = System.Data.SchemaSerializationMode.IncludeSchema;
            // 
            // o_DATATableAdapter
            // 
            this.o_DATATableAdapter.ClearBeforeFill = true;
            // 
            // btn_SendSelectedRecords
            // 
            this.btn_SendSelectedRecords.Location = new System.Drawing.Point(459, 12);
            this.btn_SendSelectedRecords.Name = "btn_SendSelectedRecords";
            this.btn_SendSelectedRecords.Size = new System.Drawing.Size(129, 23);
            this.btn_SendSelectedRecords.TabIndex = 35;
            this.btn_SendSelectedRecords.Text = "Send Selected Records";
            this.btn_SendSelectedRecords.UseVisualStyleBackColor = true;
            this.btn_SendSelectedRecords.Click += new System.EventHandler(this.btn_SendSelectedRecords_Click);
            // 
            // btn_NewTable
            // 
            this.btn_NewTable.Location = new System.Drawing.Point(193, 41);
            this.btn_NewTable.Name = "btn_NewTable";
            this.btn_NewTable.Size = new System.Drawing.Size(75, 23);
            this.btn_NewTable.TabIndex = 34;
            this.btn_NewTable.Text = "new table";
            this.btn_NewTable.UseVisualStyleBackColor = true;
            this.btn_NewTable.Click += new System.EventHandler(this.btn_NewTable_Click);
            // 
            // Btn_SaveDB
            // 
            this.Btn_SaveDB.Location = new System.Drawing.Point(274, 41);
            this.Btn_SaveDB.Name = "Btn_SaveDB";
            this.Btn_SaveDB.Size = new System.Drawing.Size(131, 23);
            this.Btn_SaveDB.TabIndex = 33;
            this.Btn_SaveDB.Text = "Save DB on Target";
            this.Btn_SaveDB.UseVisualStyleBackColor = true;
            this.Btn_SaveDB.Click += new System.EventHandler(this.Btn_SaveDB_Click);
            // 
            // btnDeleteO_DATA
            // 
            this.btnDeleteO_DATA.Location = new System.Drawing.Point(411, 41);
            this.btnDeleteO_DATA.Name = "btnDeleteO_DATA";
            this.btnDeleteO_DATA.Size = new System.Drawing.Size(75, 23);
            this.btnDeleteO_DATA.TabIndex = 32;
            this.btnDeleteO_DATA.Text = "Delete Current O_DATA";
            this.btnDeleteO_DATA.UseVisualStyleBackColor = true;
            this.btnDeleteO_DATA.Click += new System.EventHandler(this.btnDeleteO_DATA_Click);
            // 
            // btnSwitchTable
            // 
            this.btnSwitchTable.Location = new System.Drawing.Point(492, 41);
            this.btnSwitchTable.Name = "btnSwitchTable";
            this.btnSwitchTable.Size = new System.Drawing.Size(96, 23);
            this.btnSwitchTable.TabIndex = 31;
            this.btnSwitchTable.Text = "Switch Table";
            this.btnSwitchTable.UseVisualStyleBackColor = true;
            this.btnSwitchTable.Click += new System.EventHandler(this.btnSwitchTable_Click);
            // 
            // btn_CloseDB
            // 
            this.btn_CloseDB.Location = new System.Drawing.Point(355, 12);
            this.btn_CloseDB.Name = "btn_CloseDB";
            this.btn_CloseDB.Size = new System.Drawing.Size(94, 23);
            this.btn_CloseDB.TabIndex = 30;
            this.btn_CloseDB.Text = "Close Target DB";
            this.btn_CloseDB.UseVisualStyleBackColor = true;
            this.btn_CloseDB.Click += new System.EventHandler(this.btn_CloseDB_Click);
            // 
            // btnShow
            // 
            this.btnShow.Location = new System.Drawing.Point(274, 11);
            this.btnShow.Name = "btnShow";
            this.btnShow.Size = new System.Drawing.Size(75, 23);
            this.btnShow.TabIndex = 29;
            this.btnShow.Text = "ShowDB";
            this.btnShow.UseVisualStyleBackColor = true;
            this.btnShow.Click += new System.EventHandler(this.btnShow_Click);
            // 
            // btnXML
            // 
            this.btnXML.Location = new System.Drawing.Point(193, 11);
            this.btnXML.Name = "btnXML";
            this.btnXML.Size = new System.Drawing.Size(75, 23);
            this.btnXML.TabIndex = 28;
            this.btnXML.Text = "Read XML";
            this.btnXML.UseVisualStyleBackColor = true;
            this.btnXML.Click += new System.EventHandler(this.btmXML_Click);
            // 
            // tbCurrentTable
            // 
            this.tbCurrentTable.Location = new System.Drawing.Point(72, 14);
            this.tbCurrentTable.Name = "tbCurrentTable";
            this.tbCurrentTable.Size = new System.Drawing.Size(115, 20);
            this.tbCurrentTable.TabIndex = 36;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(12, 17);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(69, 13);
            this.label7.TabIndex = 38;
            this.label7.Text = "current table:";
            // 
            // labelDataGridViewTextBoxColumn
            // 
            this.labelDataGridViewTextBoxColumn.DataPropertyName = "label";
            this.labelDataGridViewTextBoxColumn.HeaderText = "label";
            this.labelDataGridViewTextBoxColumn.MaxInputLength = 30;
            this.labelDataGridViewTextBoxColumn.Name = "labelDataGridViewTextBoxColumn";
            this.labelDataGridViewTextBoxColumn.Width = 200;
            // 
            // portDataGridViewTextBoxColumn
            // 
            this.portDataGridViewTextBoxColumn.DataPropertyName = "port";
            this.portDataGridViewTextBoxColumn.HeaderText = "port";
            this.portDataGridViewTextBoxColumn.MaxInputLength = 5;
            this.portDataGridViewTextBoxColumn.Name = "portDataGridViewTextBoxColumn";
            this.portDataGridViewTextBoxColumn.ReadOnly = true;
            this.portDataGridViewTextBoxColumn.Width = 60;
            // 
            // onoffDataGridViewTextBoxColumn
            // 
            this.onoffDataGridViewTextBoxColumn.DataPropertyName = "onoff";
            this.onoffDataGridViewTextBoxColumn.HeaderText = "onoff";
            this.onoffDataGridViewTextBoxColumn.MaxInputLength = 5;
            this.onoffDataGridViewTextBoxColumn.Name = "onoffDataGridViewTextBoxColumn";
            this.onoffDataGridViewTextBoxColumn.Width = 60;
            // 
            // inputportDataGridViewTextBoxColumn
            // 
            this.inputportDataGridViewTextBoxColumn.DataPropertyName = "input_port";
            this.inputportDataGridViewTextBoxColumn.HeaderText = "input_port";
            this.inputportDataGridViewTextBoxColumn.MaxInputLength = 5;
            this.inputportDataGridViewTextBoxColumn.Name = "inputportDataGridViewTextBoxColumn";
            this.inputportDataGridViewTextBoxColumn.Width = 60;
            // 
            // typeDataGridViewTextBoxColumn
            // 
            this.typeDataGridViewTextBoxColumn.DataPropertyName = "type";
            this.typeDataGridViewTextBoxColumn.HeaderText = "type";
            this.typeDataGridViewTextBoxColumn.MaxInputLength = 10;
            this.typeDataGridViewTextBoxColumn.Name = "typeDataGridViewTextBoxColumn";
            this.typeDataGridViewTextBoxColumn.Width = 60;
            // 
            // timedelayDataGridViewTextBoxColumn
            // 
            this.timedelayDataGridViewTextBoxColumn.DataPropertyName = "time_delay";
            this.timedelayDataGridViewTextBoxColumn.HeaderText = "time_delay";
            this.timedelayDataGridViewTextBoxColumn.MaxInputLength = 10;
            this.timedelayDataGridViewTextBoxColumn.Name = "timedelayDataGridViewTextBoxColumn";
            this.timedelayDataGridViewTextBoxColumn.Width = 90;
            // 
            // btn_Laptop
            // 
            this.btn_Laptop.Location = new System.Drawing.Point(112, 41);
            this.btn_Laptop.Name = "btn_Laptop";
            this.btn_Laptop.Size = new System.Drawing.Size(75, 23);
            this.btn_Laptop.TabIndex = 39;
            this.btn_Laptop.Text = "Laptop";
            this.btn_Laptop.UseVisualStyleBackColor = true;
            this.btn_Laptop.Click += new System.EventHandler(this.UseLaptop_Click);
            // 
            // DlgForm1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(609, 474);
            this.Controls.Add(this.btn_Laptop);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.tbCurrentTable);
            this.Controls.Add(this.btn_SendSelectedRecords);
            this.Controls.Add(this.btn_NewTable);
            this.Controls.Add(this.Btn_SaveDB);
            this.Controls.Add(this.btnDeleteO_DATA);
            this.Controls.Add(this.btnSwitchTable);
            this.Controls.Add(this.btn_CloseDB);
            this.Controls.Add(this.btnShow);
            this.Controls.Add(this.dataGridView1);
            this.Controls.Add(this.btnXML);
            this.Name = "DlgForm1";
            this.Text = "DlgForm1";
            this.Load += new System.EventHandler(this.DlgForm1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.oDATABindingSource)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this._Client_SQL_DB2DataSet)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.table)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.DataGridView dataGridView1;
        private _Client_SQL_DB2DataSet _Client_SQL_DB2DataSet;
        private System.Windows.Forms.BindingSource oDATABindingSource;
        private _Client_SQL_DB2DataSetTableAdapters.O_DATATableAdapter o_DATATableAdapter;
        private System.Windows.Forms.Button btn_SendSelectedRecords;
        private System.Windows.Forms.Button btn_NewTable;
        private System.Windows.Forms.Button Btn_SaveDB;
        private System.Windows.Forms.Button btnDeleteO_DATA;
        private System.Windows.Forms.Button btnSwitchTable;
        private System.Windows.Forms.Button btn_CloseDB;
        private System.Windows.Forms.Button btnShow;
        private System.Windows.Forms.Button btnXML;
        private System.Data.DataTable table;
        private System.Windows.Forms.TextBox tbCurrentTable;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.DataGridViewTextBoxColumn labelDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn portDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn onoffDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn inputportDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn typeDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn timedelayDataGridViewTextBoxColumn;
        private System.Windows.Forms.Button btn_Laptop;
    }
}