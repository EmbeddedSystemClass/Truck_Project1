using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EpLibrary.cs;
using EpServerEngine.cs;

namespace EpServerEngineSampleClient
{
    public partial class DlgSetParams : Form
    {
        private INetworkClient m_client;

        private byte[] ret_bytes;
        public DlgSetParams()
        {
            InitializeComponent();
        }
        public void SetClient(INetworkClient client)
        {
            m_client = client;
        }
        byte[] BytesFromString(String str)
        {
            byte[] bytes = new byte[str.Length * sizeof(char)];
            System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
            return bytes;
        }
        //public bytes[] GetParams()
        //{

        //}
        private void CancelClick(object sender, EventArgs e)
        {

        }

        private void OKClicked(object sender, EventArgs e)
        {
            //return this.
        }

        private void FanOnValueChanged(object sender, EventArgs e)
        {

        }

        private void FanOffValueChanged(object sender, EventArgs e)
        {

        }

        private void UpDwnEnableBlrTemp_ValueChanged(object sender, EventArgs e)
        {

        }

        private void UpDwnBlr3On_ValueChanged(object sender, EventArgs e)
        {

        }

        private void UpDwnBlr2On_ValueChanged(object sender, EventArgs e)
        {

        }

        private void UpDwnBlr1On_ValueChanged(object sender, EventArgs e)
        {

        }

        private void UpDwnHighRev_ValueChanged(object sender, EventArgs e)
        {

        }

        private void UpDwnLowRev_ValueChanged(object sender, EventArgs e)
        {

        }

        private void UpDwnLightOn_ValueChanged(object sender, EventArgs e)
        {

        }

        private void UpDwnFPGAXmitRate_ValueChanged(object sender, EventArgs e)
        {

        }
    }
}
