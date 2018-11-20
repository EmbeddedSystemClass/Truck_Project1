using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.SqlClient;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EpServerEngineSampleClient
{
    public partial class SwitchTable : Form
    {
        public SwitchTable(string conn)
        {
            InitializeComponent();
            ConnStr = conn;

            sqlCnn = new SqlConnection(ConnStr);
            try
            {
                sqlCnn.Open();
                // call dialog that searches for all tables in current connection

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            tablelist = ListTables();
            foreach (var el in tablelist)
            {
                string str = el.ToString();
                lbTables.Items.Add(str);
            }

        }
        public string ConnStr { get; set; }
        public string returnStr { get; set; }
        private IList<string> tablelist;
        public string selectedTable { get; set; }
        private SqlConnection sqlCnn;

        public IList<string> ListTables()
        {
            List<string> tables = new List<string>();
            DataTable dt = sqlCnn.GetSchema("Tables");
            foreach (DataRow row in dt.Rows)
            {
                string tablename = (string)row[2];
                tables.Add(tablename);
            }
            return tables;
        }

        private void SelIndexChanged(object sender, EventArgs e)
        {
            int sel = lbTables.SelectedIndex;
            selectedTable = lbTables.GetItemText(lbTables.SelectedItem);
        }
        private void btnOK_Click(object sender, EventArgs e)
        {
            returnStr = selectedTable;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }
        private void btnCancel_Click(object sender, EventArgs e)
        {
            returnStr = "";
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}

