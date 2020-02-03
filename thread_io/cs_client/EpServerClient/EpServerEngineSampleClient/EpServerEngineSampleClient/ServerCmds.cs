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
			ENABLE_START,       // 1
			STARTER_OFF,        // 2
			ON_ACC,             // 3
			OFF_ACC,            // 4
			ON_FUEL_PUMP,       // 5
			OFF_FUEL_PUMP,      // 6
			ON_FAN,             // 7
			OFF_FAN,            // 8
			ON_LIGHTS,          // 9
			OFF_LIGHTS,         // 10
			ON_BRIGHTS,         // 11
			OFF_BRIGHTS,        // 12
			BLANK,
			ESTOP_SIGNAL,
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
			BLOWER1,
			BLOWER2,
			BLOWER3,
			BLOWER_OFF,
			WIPER1,
			WIPER2,
			WIPER_OFF,
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
			SHOW_ODATA,
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
			INDOOR_TEMP,
			SEND_RT_VALUES1,
			SEND_RT_VALUES2,
			SEND_RT_VALUES3,
			ENGINE_RUNTIME,
			SERVER_UPTIME,
			SEND_CONFIG,
			SEND_MSG,
			SEND_RPM,
			SEND_MPH,
			SEND_ADCS1,
			SEND_ADCS2,
			NAV_UP,
			NAV_DOWN,
			NAV_SIDE,
			NAV_CLICK,
			NAV_CLOSE,
			NAV_NUM,
			SEND_STATUS,
			SERVER_UP,
			SERVER_DOWN,
			UPLOAD_NEW,
			UPLOAD_OTHER,
			UPLOAD_NEW_PARAM,
			SET_TEMP_LIMIT,
			SET_FAN_ON,
			SET_FAN_OFF,
			HIGH_REV_LIMIT,
			LOW_REV_LIMIT,
			LIGHTS_ON_DELAY,
			SET_BLOWER_EN_TEMP,
			SET_BLOWER1_TEMP,
			SET_BLOWER2_TEMP,
			SET_BLOWER3_TEMP,
			SET_BATT_BOX_TEMP,
			TEMP_TOO_HIGH,
			GET_VERSION,
			SVR_CMD,
			HOME_SVR_ON,
			HOME_SVR_OFF,
			UPDATE_CONFIG,
			SEND_CONFIG2,
			GET_CONFIG2,
			CLIENT_CONNECTED,
			SERVER_CONNECTED,
			SET_KEYMODE,
			PASSWORD_OK,
			PASSWORD_BAD,
			SET_PASSWORD_TIMEOUT,
			SET_PASSWORD_RETRIES,
			SHELL_AND_RENAME,
			REFRESH_LCD,
			SEND_GPS_GLL_DATA,
			SEND_GPS_GGA_DATA,
			SEND_GPS_GSA_DATA,
			SEND_GPS_GSV_DATA,
			SEND_GPS_RMC_DATA,
			SEND_GPS_VTG_DATA,
			SEND_GPS_ZDA_DATA,
			ENABLE_GPS_SEND_DATA,
			SET_GPS_DATA
		}

		public ServerCmds()
        {

        }
        private INetworkClient m_client;
        public void SetClient(INetworkClient client)
        {
            m_client = client;
        }
		//Queue<int> qt = new Queue<int>();

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
        public void Send_Cmd(int sendcmd)
        {
            string test = " ";
            byte[] bytes = BytesFromString(test);
			if (m_client.IsConnectionAlive)
			{
				bytes.SetValue((byte)sendcmd, 0);
				Packet packet = new Packet(bytes, 0, bytes.Count(), false);
				m_client.Send(packet);
			}
        }
    }
}
