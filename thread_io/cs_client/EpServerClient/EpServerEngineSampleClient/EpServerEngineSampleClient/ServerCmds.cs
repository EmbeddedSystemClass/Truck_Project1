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
			ENABLE_START,		// 1
			STARTER_OFF,		// 2
			ON_ACC,				// 3
			OFF_ACC,			// 4
			ON_FUEL_PUMP,		// 5
			OFF_FUEL_PUMP,		// 6
			ON_FAN,				// 7
			OFF_FAN,			// 8
			ON_LIGHTS,			// 9
			OFF_LIGHTS,			// 10
			ON_BRIGHTS,			// 11
			OFF_BRIGHTS,		// 12
			BLANK,
			ON_BRAKES,
			OFF_BRAKES,
			ON_RUNNING_LIGHTS,
			OFF_RUNNING_LIGHTS,
			SPECIAL_CMD,
			START_SEQ,
			SHUTDOWN,
			ON_LLIGHTS,
			OFF_LLIGHTS,
			ON_LBRIGHTS,
			OFF_LBRIGHTS,
			ON_RLIGHTS,
			OFF_RLIGHTS,
			ON_RBRIGHTS,
			OFF_RBRIGHTS,
			BLOWER_OFF,
			BLOWER1,
			BLOWER2,
			BLOWER3,
			WIPER1,
			WIPER2,
			WIPER_OFF,
			STOP_MBOX_XMIT,
			START_MBOX_XMIT,
			SEND_TIME_DATA,
			SHUTDOWN_IOBOX,
			REBOOT_IOBOX,
			TEST_ALL_IO,
			SEND_ODATA,
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
			CLOSE_DB,
			OPEN_DB,
			BAD_MSG,
			CURRENT_TIME,
			SET_PARAMS,
			EXIT_PROGRAM,
			ENGINE_TEMP,
			SEND_RT_VALUES
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
        //public int GetCount()
        //{
        //    Array n = Enum.GetValues(typeof(ServerCmds));
        //    return n.Length;
        //}
        public string GetName(int cmd)
        {
            string cmd2 = "";
            cmd++;
            int i = 0;
            do
            {
                cmd2 = Enum.GetName(typeof(Server_cmds), i);
                i++;
            } while (i != cmd);
            return cmd2;
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
                case "LEFT_LIGHTS":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_LLIGHTS;
                    else sendcmd = (int)Server_cmds.OFF_LLIGHTS;
                    break;
                case "LEFT_BRIGHTS":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_LBRIGHTS;
                    else sendcmd = (int)Server_cmds.OFF_LBRIGHTS;
                    break;
                case "RIGHT_LIGHTS":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_RLIGHTS;
                    else sendcmd = (int)Server_cmds.OFF_RLIGHTS;
                    break;
                case "RIGHT_BRIGHTS":
                    if (onoff == 1)
                        sendcmd = (int)Server_cmds.ON_RBRIGHTS;
                    else sendcmd = (int)Server_cmds.OFF_RBRIGHTS;
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
//                case "UPLOAD_NEW":
 //                   sendcmd = (int)Server_cmds.UPLOAD_NEW;
//                    break;
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
//                case "SET_TIME":
//                    sendcmd = (int)Server_cmds.SET_TIME;
//                    break;
                case "DISCONNECT":
                    sendcmd = (int)Server_cmds.DISCONNECT;
                    break;
                case "SPECIAL_CMD":
                    sendcmd = (int)Server_cmds.SPECIAL_CMD;
                    break;
                case "BLOWER1_ON":
                    sendcmd = (int)Server_cmds.BLOWER1;
                    break;
                case "BLOWER2_ON":
                    sendcmd = (int)Server_cmds.BLOWER2;
                    break;
                case "BLOWER3_ON":
                    sendcmd = (int)Server_cmds.BLOWER3;
                    break;
                case "BLOWER_OFF":
                    sendcmd = (int)Server_cmds.BLOWER_OFF;
                    break;
                case "SET_PARAMS":
                    sendcmd = (int)Server_cmds.SET_PARAMS;
                    break;
//                case "TEST_ALL_IO":
//                    sendcmd = (int)Server_cmds.TEST_ALL_IO;
//                    break;
				case "STOP_MBOX_XMIT":
                    sendcmd = (int)Server_cmds.STOP_MBOX_XMIT;
					break;
				case "START_MBOX_XMIT":
                    sendcmd = (int)Server_cmds.START_MBOX_XMIT;
					break;
                default:
                    sendcmd = (int)Server_cmds.NON_CMD;
                    break;
            }
            bytes.SetValue((byte)sendcmd, 0);
            Packet packet = new Packet(bytes, 0, bytes.Count(), false);
            m_client.Send(packet);
        }
    }
}
