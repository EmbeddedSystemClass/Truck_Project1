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
            this.btn_SendSelectedRecords = new System.Windows.Forms.Button();
            this.btn_NewTable = new System.Windows.Forms.Button();
            this.Btn_SaveDB = new System.Windows.Forms.Button();
            this.btnDeleteO_DATA = new System.Windows.Forms.Button();
            this.btn_ListDB = new System.Windows.Forms.Button();
            this.btnShow = new System.Windows.Forms.Button();
            this.btnXML = new System.Windows.Forms.Button();
            this.table = new System.Data.DataTable();
            this.btnListTables = new System.Windows.Forms.Button();
            this.tbCurTable = new System.Windows.Forms.TextBox();
            this._Client_SQL2DataSet1 = new EpServerEngineSampleClient._Client_SQL2DataSet();
            this._Client_SQLDataSet1 = new EpServerEngineSampleClient._Client_SQLDataSet();
            this.bindingSource2 = new System.Windows.Forms.BindingSource(this.components);
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.o_DATATableAdapter = new EpServerEngineSampleClient._Client_SQL2DataSetTableAdapters.O_DATATableAdapter();
            this.tbStatus = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.table)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this._Client_SQL2DataSet1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this._Client_SQLDataSet1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.SuspendLayout();
            // 
            // btn_SendSelectedRecords
            // 
            this.btn_SendSelectedRecords.Location = new System.Drawing.Point(13, 41);
            this.btn_SendSelectedRecords.Name = "btn_SendSelectedRecords";
            this.btn_SendSelectedRecords.Size = new System.Drawing.Size(129, 23);
            this.btn_SendSelectedRecords.TabIndex = 35;
            this.btn_SendSelectedRecords.Text = "Send Selected Records";
            this.btn_SendSelectedRecords.UseVisualStyleBackColor = true;
            this.btn_SendSelectedRecords.Click += new System.EventHandler(this.btn_SendSelectedRecords_Click);
            // 
            // btn_NewTable
            // 
            this.btn_NewTable.Location = new System.Drawing.Point(332, 12);
            this.btn_NewTable.Name = "btn_NewTable";
            this.btn_NewTable.Size = new System.Drawing.Size(75, 23);
            this.btn_NewTable.TabIndex = 34;
            this.btn_NewTable.Text = "new table";
            this.btn_NewTable.UseVisualStyleBackColor = true;
            this.btn_NewTable.Click += new System.EventHandler(this.btn_NewTable_Click);
            // 
            // Btn_SaveDB
            // 
            this.Btn_SaveDB.Location = new System.Drawing.Point(148, 41);
            this.Btn_SaveDB.Name = "Btn_SaveDB";
            this.Btn_SaveDB.Size = new System.Drawing.Size(131, 23);
            this.Btn_SaveDB.TabIndex = 33;
            this.Btn_SaveDB.Text = "Save DB on Target";
            this.Btn_SaveDB.UseVisualStyleBackColor = true;
            this.Btn_SaveDB.Click += new System.EventHandler(this.Btn_SaveDB_Click);
            // 
            // btnDeleteO_DATA
            // 
            this.btnDeleteO_DATA.Location = new System.Drawing.Point(251, 12);
            this.btnDeleteO_DATA.Name = "btnDeleteO_DATA";
            this.btnDeleteO_DATA.Size = new System.Drawing.Size(75, 23);
            this.btnDeleteO_DATA.TabIndex = 32;
            this.btnDeleteO_DATA.Text = "Delete Current O_DATA";
            this.btnDeleteO_DATA.UseVisualStyleBackColor = true;
            this.btnDeleteO_DATA.Click += new System.EventHandler(this.btnDeleteO_DATA_Click);
            // 
            // btn_ListDB
            // 
            this.btn_ListDB.Enabled = false;
            this.btn_ListDB.Location = new System.Drawing.Point(598, 603);
            this.btn_ListDB.Name = "btn_ListDB";
            this.btn_ListDB.Size = new System.Drawing.Size(10, 10);
            this.btn_ListDB.TabIndex = 30;
            this.btn_ListDB.Text = "List DB\'s";
            this.btn_ListDB.UseVisualStyleBackColor = true;
            this.btn_ListDB.Visible = false;
            this.btn_ListDB.Click += new System.EventHandler(this.btn_ListDB_Click);
            // 
            // btnShow
            // 
            this.btnShow.Location = new System.Drawing.Point(13, 12);
            this.btnShow.Name = "btnShow";
            this.btnShow.Size = new System.Drawing.Size(75, 23);
            this.btnShow.TabIndex = 29;
            this.btnShow.Text = "ShowDB";
            this.btnShow.UseVisualStyleBackColor = true;
            this.btnShow.Click += new System.EventHandler(this.btnShow_Click);
            // 
            // btnXML
            // 
            this.btnXML.Location = new System.Drawing.Point(171, 12);
            this.btnXML.Name = "btnXML";
            this.btnXML.Size = new System.Drawing.Size(75, 23);
            this.btnXML.TabIndex = 28;
            this.btnXML.Text = "Read XML";
            this.btnXML.UseVisualStyleBackColor = true;
            this.btnXML.Click += new System.EventHandler(this.btmXML_Click);
            // 
            // btnListTables
            // 
            this.btnListTables.Location = new System.Drawing.Point(91, 12);
            this.btnListTables.Name = "btnListTables";
            this.btnListTables.Size = new System.Drawing.Size(75, 23);
            this.btnListTables.TabIndex = 40;
            this.btnListTables.Text = "List Tables";
            this.btnListTables.UseVisualStyleBackColor = true;
            this.btnListTables.Click += new System.EventHandler(this.btnListTables_Click);
            // 
            // tbCurTable
            // 
            this.tbCurTable.Location = new System.Drawing.Point(285, 41);
            this.tbCurTable.Name = "tbCurTable";
            this.tbCurTable.Size = new System.Drawing.Size(152, 20);
            this.tbCurTable.TabIndex = 41;
            // 
            // _Client_SQL2DataSet1
            // 
            this._Client_SQL2DataSet1.DataSetName = "_Client_SQL2DataSet";
            this._Client_SQL2DataSet1.SchemaSerializationMode = System.Data.SchemaSerializationMode.IncludeSchema;
            // 
            // _Client_SQLDataSet1
            // 
            this._Client_SQLDataSet1.DataSetName = "_Client_SQLDataSet";
            this._Client_SQLDataSet1.SchemaSerializationMode = System.Data.SchemaSerializationMode.IncludeSchema;
            // 
            // bindingSource2
            // 
            this.bindingSource2.DataSource = this._Client_SQL2DataSet1;
            this.bindingSource2.Position = 0;
            // 
            // dataGridView1
            // 
            this.dataGridView1.AutoGenerateColumns = false;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.DataSource = this.bindingSource2;
            this.dataGridView1.Location = new System.Drawing.Point(6, 70);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size(591, 532);
            this.dataGridView1.TabIndex = 42;
            // 
            // o_DATATableAdapter
            // 
            this.o_DATATableAdapter.ClearBeforeFill = true;
            // 
            // tbStatus
            // 
            this.tbStatus.Location = new System.Drawing.Point(620, 14);
            this.tbStatus.Multiline = true;
            this.tbStatus.Name = "tbStatus";
            this.tbStatus.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbStatus.Size = new System.Drawing.Size(271, 588);
            this.tbStatus.TabIndex = 43;
            // 
            // DlgForm1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(903, 614);
            this.Controls.Add(this.tbStatus);
            this.Controls.Add(this.dataGridView1);
            this.Controls.Add(this.tbCurTable);
            this.Controls.Add(this.btnListTables);
            this.Controls.Add(this.btn_SendSelectedRecords);
            this.Controls.Add(this.btn_NewTable);
            this.Controls.Add(this.Btn_SaveDB);
            this.Controls.Add(this.btnDeleteO_DATA);
            this.Controls.Add(this.btn_ListDB);
            this.Controls.Add(this.btnShow);
            this.Controls.Add(this.btnXML);
            this.Name = "DlgForm1";
            this.Text = "DlgForm1";
            this.Load += new System.EventHandler(this.DlgForm1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.table)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this._Client_SQL2DataSet1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this._Client_SQLDataSet1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        //private _Client_SQL_DB2DataSetTableAdapters.O_DATATableAdapter o_DATATableAdapter;
        private System.Windows.Forms.Button btn_SendSelectedRecords;
        private System.Windows.Forms.Button btn_NewTable;
        private System.Windows.Forms.Button Btn_SaveDB;
        private System.Windows.Forms.Button btnDeleteO_DATA;
        private System.Windows.Forms.Button btn_ListDB;
        private System.Windows.Forms.Button btnShow;
        private System.Windows.Forms.Button btnXML;
        private System.Data.DataTable table;
        private System.Windows.Forms.DataGridViewTextBoxColumn labelDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn portDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn onoffDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn inputportDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn typeDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn timedelayDataGridViewTextBoxColumn;
        private System.Windows.Forms.Button btnListTables;
        private System.Windows.Forms.TextBox tbCurTable;
        private _Client_SQL2DataSet _Client_SQL2DataSet1;
        private _Client_SQLDataSet _Client_SQLDataSet1;
        private System.Windows.Forms.BindingSource bindingSource2;
        private System.Windows.Forms.DataGridView dataGridView1;
        private _Client_SQL2DataSetTableAdapters.O_DATATableAdapter o_DATATableAdapter;
        private System.Windows.Forms.TextBox tbStatus;
    }
}