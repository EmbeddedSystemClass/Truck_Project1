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
    public partial class ChangeDB : Form
    {
        public ChangeDB(string conn)
        {
            InitializeComponent();
            ConnStr = conn;
            dblist = GetDatabaseList();
            foreach (var el in dblist)
            {
                string str = el.ToString();
                if (str.Contains("C:\\USERS"))
                {
//                    MessageBox.Show(str);
                    lbDBs.Items.Add(str);
                }
            }
        }

        public string ConnStr { get; set; }
        public string returnStr { get; set; }
        private List<string> dblist;
        public string selectedDB { get; set; }

        public List<string> GetDatabaseList()
        {
            List<string> list = new List<string>();

            // Open connection to the database

            using (SqlConnection con = new SqlConnection(ConnStr))
            {
                con.Open();

                // Set up a command with the given query and associate
                // this with the current connection.
                using (SqlCommand cmd = new SqlCommand("SELECT name from sys.databases", con))
                {
                    using (IDataReader dr = cmd.ExecuteReader())
                    {
                        while (dr.Read())
                        {
                            list.Add(dr[0].ToString());
                        }
                    }
                }
            }
            return list;
        }
        private void btnOK_Click(object sender, EventArgs e)
        {
            returnStr = selectedDB;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }
        private void btnCancel_Click(object sender, EventArgs e)
        {
            returnStr = "";
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }
        private void Selchanged(object sender, EventArgs e)
        {
            int sel = lbDBs.SelectedIndex;
            selectedDB = lbDBs.GetItemText(lbDBs.SelectedItem);
        }
    }
}
