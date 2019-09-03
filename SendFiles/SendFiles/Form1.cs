using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace SendFiles
{
    public partial class Form1 : Form
    {
        public string SendingFilePath = string.Empty;
        private const int BufferSize = 10000;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            progressBar1.Visible=true;
            progressBar1.Minimum=1;
            progressBar1.Value=1;
            progressBar1.Step=1;
            
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            OpenFileDialog Dlg = new OpenFileDialog();
            Dlg.Filter = "All Files (*.*)|*.*";
            Dlg.CheckFileExists = true;
            Dlg.Title = "Choose a File";
            Dlg.InitialDirectory = @"C:\users\daniel\dev\";
			if (Dlg.ShowDialog() == DialogResult.OK)
			{
				SendingFilePath = Dlg.FileName;
				tbFileSelected.Text = Dlg.FileName;
			}
			else
				tbFileSelected.Text = "";
        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            if (SendingFilePath != string.Empty)
            {   
                SendTCP(SendingFilePath, txtIP.Text, Int32.Parse(txtPort.Text));
            }
            else
                MessageBox.Show("Select a file","Warning");
        }
        public void SendTCP(string M, string IPA, Int32 PortN)
        {
            byte[] SendingBuffer = null;
            TcpClient client = null;
            lblStatus.Text = "";
            NetworkStream netstream = null;
            try
            {
                client = new TcpClient(IPA, PortN);
                lblStatus.Text = "Connected to the Server...\n";
                netstream = client.GetStream();
                FileStream Fs = new FileStream(M, FileMode.Open, FileAccess.Read);
                int NoOfPackets = Convert.ToInt32(Math.Ceiling(Convert.ToDouble(Fs.Length) / Convert.ToDouble(BufferSize)));
                progressBar1.Maximum = NoOfPackets;
                int TotalLength = (int)Fs.Length, CurrentPacketLength, counter = 0;
				byte[] bytes = new byte[8];
				tbDebug.AppendText(TotalLength.ToString());
				tbDebug.AppendText("\r\n");
				tbDebug.AppendText(NoOfPackets.ToString());
				tbDebug.AppendText("\r\n");
				int count = 4;
				System.Buffer.BlockCopy(BitConverter.GetBytes(TotalLength),0,bytes,0,count);
				System.Buffer.BlockCopy(BitConverter.GetBytes(NoOfPackets), 0, bytes, 4, count);
				//System.Buffer.BlockCopy(src, srcOffset, dst, dstOffset, len);

				// first write the file size and no. of packets
				netstream.Write(bytes, 0, 8);
				
				for (int i = 0; i < NoOfPackets; i++)
                {
                    if (TotalLength > BufferSize)
                    {
                        CurrentPacketLength = BufferSize;
                        TotalLength = TotalLength - CurrentPacketLength;
                    }
                    else
                        CurrentPacketLength = TotalLength;
                    SendingBuffer = new byte[CurrentPacketLength];
                    Fs.Read(SendingBuffer, 0, CurrentPacketLength);
                    netstream.Write(SendingBuffer, 0, (int)SendingBuffer.Length);
					//tbDebug.AppendText(SendingBuffer.Length.ToString());
					//tbDebug.AppendText("\r\n");
                    if (progressBar1.Value >= progressBar1.Maximum)
                        progressBar1.Value = progressBar1.Minimum;
                    progressBar1.PerformStep();
                }
                
                lblStatus.Text=lblStatus.Text+"Sent "+Fs.Length.ToString()+" bytes to the server";
                Fs.Close();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
                netstream.Close();
                client.Close();

            }
        }

		private void groupBox1_Enter(object sender, EventArgs e)
		{

		}
	}
}