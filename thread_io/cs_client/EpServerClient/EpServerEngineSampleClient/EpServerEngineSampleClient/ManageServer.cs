﻿using System;
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
    public partial class ManageServer : Form
    {
        INetworkClient m_client = null;
        int reboot_code = 0;
        ServerCmds svrcmd = new ServerCmds();
        public ManageServer(INetworkClient client)
        {
            m_client = client;
            InitializeComponent();
            svrcmd.SetClient(m_client);
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
                //tbReceived.Text += message + "\r\n";
                tbReceived.AppendText(message + "\r\n");
            }
        }

        private void RebootServer_Click(object sender, EventArgs e)
        {
            AddMsg("sending REBOOT_IOBOX");
            reboot_code = 1;
        }

        private void ShutdownServer_Click(object sender, EventArgs e)
        {
            AddMsg("sending SHUTDOWN_IOBOX");
            reboot_code = 2;
        }

        private void UploadNew_Click(object sender, EventArgs e)
        {
            AddMsg("sending UPLOAD_NEW");
            reboot_code = 3;
        }

        private void UploadOther_Click(object sender, EventArgs e)
        {
            AddMsg("sending UPLOAD_OTHER");
            reboot_code = 4;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            string cmd = "";
            int offset = 0;

            switch (reboot_code)
            {
                case 0:
                    break;
                case 1:
                    cmd = "REBOOT_IOBOX";
                    break;
                case 2:
                    cmd = "SHUTDOWN_IOBOX";
                    break;
                case 3:
                    cmd = "UPLOAD_NEW";
                    break;
                case 4:
                    cmd = "UPLOAD_OTHER";
                    break;
                default:
                    break;
            }
            if (reboot_code > 0 && reboot_code < 5)
            {
                offset = svrcmd.GetCmdIndexI(cmd);
                svrcmd.Send_Cmd(offset);
                AddMsg("sending REBOOT_IOBOX");
                this.DialogResult = DialogResult.OK;
            }
            else this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void tbReceived_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
