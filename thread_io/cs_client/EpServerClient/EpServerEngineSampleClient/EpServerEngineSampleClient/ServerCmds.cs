using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EpLibrary.cs;
using EpServerEngine.cs;

namespace EpServerEngineSampleClient
{
    class ServerCmds
    {
        enum Server_cmds
        {
            NON_CMD,
			ENABLE_START,
			STARTER_OFF,
			ON_ACC,
			OFF_ACC,
			ON_FUEL_PUMP,
			OFF_FUEL_PUMP,
			ON_FAN,
			OFF_FAN,
			ON_LIGHTS,
			OFF_LIGHTS,
			ON_BRIGHTS,
			OFF_BRIGHTS,
			ON_BRAKES,
			OFF_BRAKES,
			ON_RUNNING_LIGHTS,
			OFF_RUNNING_LIGHTS,
			START_SEQ,				// 17
			SHUTDOWN,
			SHUTDOWN_IOBOX,
			REBOOT_IOBOX,
			SEND_ODATA,
			EDIT_ODATA,
			EDIT_ODATA2,
			SEND_ALL_ODATA,
			RECV_ALL_ODATA,
			SHOW_ODATA,
			SAVE_TO_DISK,
			GET_DIR,
			LCD_SHIFT_RIGHT,
			LCD_SHIFT_LEFT,
			SCROLL_UP,
			SCROLL_DOWN,
			ENABLE_LCD,
			SET_TIME,
			GET_TIME,
			UPLOAD_NEW,
			NEW_PASSWORD1,
			SET_SERIAL_RECV_ON,
			SET_SERIAL_RECV_OFF,
			TEST_LEFT_BLINKER,
			TEST_RIGHT_BLINKER,
			RE_ENTER_PASSWORD,
			DISCONNECT,
			STOP_MBOX_RECV,
			CLOSE_DB,
			OPEN_DB,
			BAD_MSG,
			CURRENT_TIME,
			EXIT_PROGRAM
		}

        public ServerCmds()
        {

        }
        private INetworkClient m_client;
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

        public byte GetCmdIndexB(string cmd)
        {
            byte i = 0;
            string cmd2 = "";
            do
            {
                cmd2 = Enum.GetName(typeof(Server_cmds), i);
                i++;
            } while (cmd2 != cmd);
            i--;
           return i;
        }
        public int GetCmdIndexI(string cmd)
        {
            int i = 0;
            string cmd2 = "";
            do
            {
                cmd2 = Enum.GetName(typeof(Server_cmds), i);
                i++;
            } while (cmd2 != cmd);
            i--;
            return i;
        }
        public string GetName(int cmd)
        {
            return Enum.GetName(typeof(ServerCmds), cmd);
        }
        public void Send_Cmd(string cmd, int onoff)
        {
            string test = " ";
            int sendcmd = 0;
            byte[] bytes = BytesFromString(test);

            switch (cmd)
            {
                case "STARTER":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ENABLE_START;
                    else sendcmd = (int)Server_cmds.STARTER_OFF;
                    //                    cblistCommon.SetSelected(0, false);
                    break;
                case "IGNITION":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_ACC;
                    else sendcmd = (int)Server_cmds.OFF_ACC;
                    break;
                case "FUELPUMP":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_FUEL_PUMP;
                    else sendcmd = (int)Server_cmds.OFF_FUEL_PUMP;
                    break;
                case "COOLINGFAN":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_FAN;
                    else sendcmd = (int)Server_cmds.OFF_FAN;
                    break;
                case "LIGHTS":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_LIGHTS;
                    else sendcmd = (int)Server_cmds.OFF_LIGHTS;
                    break;
                case "BRIGHTS":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_BRIGHTS;
                    else sendcmd = (int)Server_cmds.OFF_BRIGHTS;
                    break;
                case "START_SEQ":
                    sendcmd = (int)Server_cmds.START_SEQ;
                    break;
                case "SHUTDOWN":
                    sendcmd = (int)Server_cmds.SHUTDOWN;
                    break;
                case "SHUTDOWN_IOBOX":
                    sendcmd = (int)Server_cmds.SHUTDOWN_IOBOX;
                    break;
                case "REBOOT_IOBOX":
                    sendcmd = (int)Server_cmds.REBOOT_IOBOX;
                    break;
                case "CLOSE_DB":
                    sendcmd = (int)Server_cmds.CLOSE_DB;
                    break;
                case "OPEN_DB":
                    sendcmd = (int)Server_cmds.OPEN_DB;
                    break;
                case "TEST_LEFT_BLINKER":
                    sendcmd = (int)Server_cmds.TEST_LEFT_BLINKER;
                    break;
                case "TEST_RIGHT_BLINKER":
                    sendcmd = (int)Server_cmds.TEST_RIGHT_BLINKER;
                    break;
                case "UPLOAD_NEW":
                    sendcmd = (int)Server_cmds.UPLOAD_NEW;
                    break;
                case "BRAKES":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_BRAKES;
                    else sendcmd = (int)Server_cmds.OFF_BRAKES;
                    break;
                case "RUNNING_LIGHTS":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_RUNNING_LIGHTS;
                    else sendcmd = (int)Server_cmds.OFF_RUNNING_LIGHTS;
                    break;
                case "SAVE_TO_DISK":
                    sendcmd = (int)Server_cmds.SAVE_TO_DISK;
                    break;
                case "CURRENT_TIME":
                    sendcmd = (int)Server_cmds.CURRENT_TIME;
                    break;
                case "GET_TIME":
                    sendcmd = (int)Server_cmds.GET_TIME;
                    break;
                case "SHOW_ODATA":
                    sendcmd = (int)Server_cmds.SHOW_ODATA;
                    break;
                case "DISCONNECT":
                    sendcmd = (int)Server_cmds.DISCONNECT;
                    break;
                case "STOP_MBOX_RECV":
                    sendcmd = (int)Server_cmds.STOP_MBOX_RECV;
                    break;
                default:
                    return;
            }
            bytes.SetValue((byte)sendcmd, 0);
            //            AddMsg(Enum.GetName(typeof(Server_cmds), sendcmd));
            //MessageBox.Show(send_cmd.ToString());
            Packet packet = new Packet(bytes, 0, bytes.Count(), false);
            m_client.Send(packet);
        }
    }
}
