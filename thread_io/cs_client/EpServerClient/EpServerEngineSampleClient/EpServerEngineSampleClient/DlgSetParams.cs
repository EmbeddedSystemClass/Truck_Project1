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
		private bool refreshed = false;
        private ConfigParams cfg;
		private string password = "testasdf";
        ServerCmds svrcmd;
        IDictionary<string, int> dtemps;
        List<string> temp_str;
		IDictionary<string, int> lights_str;
		IDictionary<string, int> timeout_str;
		public void SetParams(ConfigParams _cfg)
		{
			cfg = _cfg;
		}
		public bool GetSet()
		{
			return cfg.set;
		}
        public ConfigParams GetParams()
        {
			refreshed = true;
            return cfg;
        }
        public void SetClient(INetworkClient client)
        {
            m_client = client;
        }
        private void CancelClick(object sender, EventArgs e)
        {
		}
		public string get_temp_str(int index)
        {
			// return string given the index
			if (index < 512 && index > 402)
			{
				int ret = 250;
				ret += (511 - index);
				if (ret > 0 && ret < 359)
					return temp_str[ret];
				else return "NAN";
			}
			else if (index < 251 && index > 0)
				return temp_str[250 - index];
			else return "NAN";
        }
		//			cbFanOn.SelectedIndex = get_temp_index(get_temp_str(cfg.fan_on));

		public int get_temp_index(string str)
		{
			int i;
			for(i = 0;i < 359;i++)
			{
				if (str == temp_str[i])
					return i;
			}
			return -1;
		}
		public int get_lights_on(string str)
		{
			return lights_str[str];
		}
		public int get_timeout(string str)
		{
			// return selected index of the string
			return timeout_str[str];
		}
		// TODO: need a dialog that pops up if blower1_on is lower that blower2_on or 
		// blower3_on and so on

		private void OKClicked(object sender, EventArgs e)
        {
			if (refreshed == true)
			{
				string str = cbFanOff.SelectedItem.ToString();
				cfg.fan_off = dtemps[str];

				str = cbFanOn.SelectedItem.ToString();
				cfg.fan_on = dtemps[str];

				str = cbBlowerEnabled.SelectedItem.ToString();
				cfg.blower_enabled = dtemps[str];

				str = cbBlower3.SelectedItem.ToString();
				cfg.blower3_on = dtemps[str];

				str = cbBlower2.SelectedItem.ToString();
				cfg.blower2_on = dtemps[str];

				str = cbBlower1.SelectedItem.ToString();
				cfg.blower1_on = dtemps[str];

				str = cbTempLimit.SelectedItem.ToString();
				cfg.si_engine_temp_limit = dtemps[str];

				//textBox1.Text = cbFanOff.SelectedIndex.ToString();
				//blower1_on = Convert.ToInt16(cbBlower1.SelectedItem.ToString());
				//MessageBox.Show(cbRPMUpdateRate.SelectedItem.ToString());
				cfg.rpm_mph_update_rate = Convert.ToInt16(cbRPM_MPHUpdateRate.SelectedItem.ToString());
				//MessageBox.Show(cfg.rpm_mph_update_rate.ToString());
				cfg.high_rev_limit = Convert.ToInt16(cbHighRevLimit.SelectedItem.ToString());
				cfg.low_rev_limit = Convert.ToInt16(cbLowRevLimit.SelectedItem.ToString());
				cfg.rpm_mph_update_rate = Convert.ToInt16(cbRPM_MPHUpdateRate.SelectedItem.ToString());
				cfg.FPGAXmitRate = Convert.ToInt16(cbFPGAXmitRate.SelectedItem.ToString());
				cfg.lights_on_delay = get_lights_on(cbLightsOnDelay.SelectedItem.ToString());
				cfg.password_timeout = get_timeout(cbPasswordTimeout.SelectedItem.ToString());
				cfg.baudrate3 = Convert.ToInt16(cbBaudRate3.SelectedItem.ToString());
				cfg.password = password;
				cfg.test_bank = cbTestBank.SelectedIndex;
			}
		}
        private void cbFanOn_SelectedIndexChanged(object sender, EventArgs e)
        {
            string str = cbFanOn.SelectedItem.ToString();
            cfg.fan_on = dtemps[str];
            cfg.si_fan_on = cbFanOn.SelectedIndex;
        }
        private void cbFanOff_SelectedIndexChanged(object sender, EventArgs e)
        {
            string str = cbFanOff.SelectedItem.ToString();
            cfg.fan_off = dtemps[str];
            cfg.si_fan_off = cbFanOff.SelectedIndex;
        }
        private void cbTempLimit_selected_index_changed(object sender, EventArgs e)
        {
            string str = cbTempLimit.SelectedItem.ToString();
            cfg.engine_temp_limit = dtemps[str];
            cfg.si_engine_temp_limit = cbTempLimit.SelectedIndex;
        }
        private void cbBlowerEnabled_SelectedIndexChanged(object sender, EventArgs e)
        {
            string str = cbBlowerEnabled.SelectedItem.ToString();
            cfg.blower_enabled = dtemps[str];
            cfg.si_blower_enabled = cbBlowerEnabled.SelectedIndex;
        }
        private void cbBlower3_SelectedIndexChanged(object sender, EventArgs e)
        {
            string str = cbBlower3.SelectedItem.ToString();
            cfg.blower3_on = dtemps[str];
            cfg.si_blower3_on = cbBlower3.SelectedIndex;
        }
        private void cbBlower2_SelectedIndexChanged(object sender, EventArgs e)
        {
            string str = cbBlower2.SelectedItem.ToString();
            cfg.blower2_on = dtemps[str];
            cfg.si_blower2_on = cbBlower2.SelectedIndex;
        }
        private void cbBlower1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string str = cbBlower1.SelectedItem.ToString();
            cfg.blower1_on = dtemps[str];
            cfg.si_blower1_on = cbBlower1.SelectedIndex;
        }
        private void cbHighRevLimit_SelectedIndexChanged(object sender, EventArgs e)
        {
            cfg.high_rev_limit = Convert.ToInt16(cbHighRevLimit.SelectedItem.ToString());
            cfg.si_high_rev_limit = cbHighRevLimit.SelectedIndex;
        }
        private void cbLowRevLimit_SelectedIndexChanged(object sender, EventArgs e)
        {
            cfg.low_rev_limit = Convert.ToInt16(cbLowRevLimit.SelectedItem.ToString());
            cfg.si_low_rev_limit = cbLowRevLimit.SelectedIndex;
        }
        private void cbLightsOnDelay_SelectedIndexChanged(object sender, EventArgs e)
        {
			cfg.lights_on_delay = get_lights_on(cbLightsOnDelay.SelectedItem.ToString());
            cfg.si_lights_on_delay = cbLightsOnDelay.SelectedIndex;
        }
        private void cbFPGAXmitRate_SelectedIndexChanged(object sender, EventArgs e)
        {
            cfg.FPGAXmitRate = cbFPGAXmitRate.SelectedIndex;
            cfg.si_FPGAXmitRate = cbFPGAXmitRate.SelectedIndex;
        }
        private void cbRPMUpdateRate_SelectedIndexChanged(object sender, EventArgs e)
        {
            cfg.rpm_mph_update_rate = cbRPM_MPHUpdateRate.SelectedIndex;
        }
        private void comboBox6_SelectedIndexChanged(object sender, EventArgs e)
        {
            //cfg.mph_mph_update_rate = cbMPHUpdateRate.SelectedIndex;
        }
        private void cbTestBank_SelectedIndexChanged(object sender, EventArgs e)
        {
            cfg.test_bank = cbTestBank.SelectedIndex;
            cfg.si_test_bank = cbTestBank.SelectedIndex;
        }
		private void cbPasswordTimeout_SelectedIndexChanged(object sender, EventArgs e)
		{
			string str = cbPasswordTimeout.SelectedItem.ToString();
			cfg.password_timeout = timeout_str[str];
			cfg.si_password_timeout = cbPasswordTimeout.SelectedIndex;
		}
		private void cbPasswordRetries_SelectedIndexChanged(object sender, EventArgs e)
		{
			// since retries are: 2,3,4,5,6 then just add 2 to index
			cfg.password_retries = cbPasswordRetries.SelectedIndex + 2;
			cfg.si_password_retries = cbPasswordRetries.SelectedIndex;
		}
		private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
		{
			cfg.baudrate3 = cbBaudRate3.SelectedIndex;
			cfg.si_baudrate3 = cbBaudRate3.SelectedIndex;
		}
		public DlgSetParams(ConfigParams cfg_params)
        {
            cfg = cfg_params;
            InitializeComponent();
            svrcmd = new ServerCmds();
			cfg.set = true;

            // initialize the string list with all the string values in the dictionary and
            // populate the temperature comboboxes with them all
            temp_str = new List<string>(359);
			temp_str.Add("257.0\0");
			temp_str.Add("256.1\0");
			temp_str.Add("255.2\0");
			temp_str.Add("254.3\0");
			temp_str.Add("253.4\0");
			temp_str.Add("252.5\0");
			temp_str.Add("251.6\0");
			temp_str.Add("250.7\0");
			temp_str.Add("249.8\0");
			temp_str.Add("248.9\0");
			temp_str.Add("248.0\0");
			temp_str.Add("247.1\0");
			temp_str.Add("246.2\0");
			temp_str.Add("245.3\0");
			temp_str.Add("244.4\0");
			temp_str.Add("243.5\0");
			temp_str.Add("242.6\0");
			temp_str.Add("241.7\0");
			temp_str.Add("240.8\0");
			temp_str.Add("239.9\0");
			temp_str.Add("239.0\0");
			temp_str.Add("238.1\0");
			temp_str.Add("237.2\0");
			temp_str.Add("236.3\0");
			temp_str.Add("235.4\0");
			temp_str.Add("234.5\0");
			temp_str.Add("233.6\0");
			temp_str.Add("232.7\0");
			temp_str.Add("231.8\0");
			temp_str.Add("230.9\0");
			temp_str.Add("230.0\0");
			temp_str.Add("229.1\0");
			temp_str.Add("228.2\0");
			temp_str.Add("227.3\0");
			temp_str.Add("226.4\0");
			temp_str.Add("225.5\0");
			temp_str.Add("224.6\0");
			temp_str.Add("223.7\0");
			temp_str.Add("222.8\0");
			temp_str.Add("221.9\0");
			temp_str.Add("221.0\0");
			temp_str.Add("220.1\0");
			temp_str.Add("219.2\0");
			temp_str.Add("218.3\0");
			temp_str.Add("217.4\0");
			temp_str.Add("216.5\0");
			temp_str.Add("215.6\0");
			temp_str.Add("214.7\0");
			temp_str.Add("213.8\0");
			temp_str.Add("212.9\0");
			temp_str.Add("212.0\0");
			temp_str.Add("211.1\0");
			temp_str.Add("210.2\0");
			temp_str.Add("209.3\0");
			temp_str.Add("208.4\0");
			temp_str.Add("207.5\0");
			temp_str.Add("206.6\0");
			temp_str.Add("205.7\0");
			temp_str.Add("204.8\0");
			temp_str.Add("203.9\0");
			temp_str.Add("203.0\0");
			temp_str.Add("202.1\0");
			temp_str.Add("201.2\0");
			temp_str.Add("200.3\0");
			temp_str.Add("199.4\0");
			temp_str.Add("198.5\0");
			temp_str.Add("197.6\0");
			temp_str.Add("196.7\0");
			temp_str.Add("195.8\0");
			temp_str.Add("194.9\0");
			temp_str.Add("194.0\0");
			temp_str.Add("193.1\0");
			temp_str.Add("192.2\0");
			temp_str.Add("191.3\0");
			temp_str.Add("190.4\0");
			temp_str.Add("189.5\0");
			temp_str.Add("188.6\0");
			temp_str.Add("187.7\0");
			temp_str.Add("186.8\0");
			temp_str.Add("185.9\0");
			temp_str.Add("185.0\0");
			temp_str.Add("184.1\0");
			temp_str.Add("183.2\0");
			temp_str.Add("182.3\0");
			temp_str.Add("181.4\0");
			temp_str.Add("180.5\0");
			temp_str.Add("179.6\0");
			temp_str.Add("178.7\0");
			temp_str.Add("177.8\0");
			temp_str.Add("176.9\0");
			temp_str.Add("176.0\0");
			temp_str.Add("175.1\0");
			temp_str.Add("174.2\0");
			temp_str.Add("173.3\0");
			temp_str.Add("172.4\0");
			temp_str.Add("171.5\0");
			temp_str.Add("170.6\0");
			temp_str.Add("169.7\0");
			temp_str.Add("168.8\0");
			temp_str.Add("167.9\0");
			temp_str.Add("167.0\0");
			temp_str.Add("166.1\0");
			temp_str.Add("165.2\0");
			temp_str.Add("164.3\0");
			temp_str.Add("163.4\0");
			temp_str.Add("162.5\0");
			temp_str.Add("161.6\0");
			temp_str.Add("160.7\0");
			temp_str.Add("159.8\0");
			temp_str.Add("158.9\0");
			temp_str.Add("158.0\0");
			temp_str.Add("157.1\0");
			temp_str.Add("156.2\0");
			temp_str.Add("155.3\0");
			temp_str.Add("154.4\0");
			temp_str.Add("153.5\0");
			temp_str.Add("152.6\0");
			temp_str.Add("151.7\0");
			temp_str.Add("150.8\0");
			temp_str.Add("149.9\0");
			temp_str.Add("149.0\0");
			temp_str.Add("148.1\0");
			temp_str.Add("147.2\0");
			temp_str.Add("146.3\0");
			temp_str.Add("145.4\0");
			temp_str.Add("144.5\0");
			temp_str.Add("143.6\0");
			temp_str.Add("142.7\0");
			temp_str.Add("141.8\0");
			temp_str.Add("140.9\0");
			temp_str.Add("140.0\0");
			temp_str.Add("139.1\0");
			temp_str.Add("138.2\0");
			temp_str.Add("137.3\0");
			temp_str.Add("136.4\0");
			temp_str.Add("135.5\0");
			temp_str.Add("134.6\0");
			temp_str.Add("133.7\0");
			temp_str.Add("132.8\0");
			temp_str.Add("131.9\0");
			temp_str.Add("131.0\0");
			temp_str.Add("130.1\0");
			temp_str.Add("129.2\0");
			temp_str.Add("128.3\0");
			temp_str.Add("127.4\0");
			temp_str.Add("126.5\0");
			temp_str.Add("125.6\0");
			temp_str.Add("124.7\0");
			temp_str.Add("123.8\0");
			temp_str.Add("122.9\0");
			temp_str.Add("122.0\0");
			temp_str.Add("121.1\0");
			temp_str.Add("120.2\0");
			temp_str.Add("119.3\0");
			temp_str.Add("118.4\0");
			temp_str.Add("117.5\0");
			temp_str.Add("116.6\0");
			temp_str.Add("115.7\0");
			temp_str.Add("114.8\0");
			temp_str.Add("113.9\0");
			temp_str.Add("113.0\0");
			temp_str.Add("112.1\0");
			temp_str.Add("111.2\0");
			temp_str.Add("110.3\0");
			temp_str.Add("109.4\0");
			temp_str.Add("108.5\0");
			temp_str.Add("107.6\0");
			temp_str.Add("106.7\0");
			temp_str.Add("105.8\0");
			temp_str.Add("104.9\0");
			temp_str.Add("104.0\0");
			temp_str.Add("103.1\0");
			temp_str.Add("102.2\0");
			temp_str.Add("101.3\0");
			temp_str.Add("100.4\0");
			temp_str.Add("99.5\0");
			temp_str.Add("98.6\0");
			temp_str.Add("97.7\0");
			temp_str.Add("96.8\0");
			temp_str.Add("95.9\0");
			temp_str.Add("95.0\0");
			temp_str.Add("94.1\0");
			temp_str.Add("93.2\0");
			temp_str.Add("92.3\0");
			temp_str.Add("91.4\0");
			temp_str.Add("90.5\0");
			temp_str.Add("89.6\0");
			temp_str.Add("88.7\0");
			temp_str.Add("87.8\0");
			temp_str.Add("86.9\0");
			temp_str.Add("86.0\0");
			temp_str.Add("85.1\0");
			temp_str.Add("84.2\0");
			temp_str.Add("83.3\0");
			temp_str.Add("82.4\0");
			temp_str.Add("81.5\0");
			temp_str.Add("80.6\0");
			temp_str.Add("79.7\0");
			temp_str.Add("78.8\0");
			temp_str.Add("77.9\0");
			temp_str.Add("77.0\0");
			temp_str.Add("76.1\0");
			temp_str.Add("75.2\0");
			temp_str.Add("74.3\0");
			temp_str.Add("73.4\0");
			temp_str.Add("72.5\0");
			temp_str.Add("71.6\0");
			temp_str.Add("70.7\0");
			temp_str.Add("69.8\0");
			temp_str.Add("68.9\0");
			temp_str.Add("68.0\0");
			temp_str.Add("67.1\0");
			temp_str.Add("66.2\0");
			temp_str.Add("65.3\0");
			temp_str.Add("64.4\0");
			temp_str.Add("63.5\0");
			temp_str.Add("62.6\0");
			temp_str.Add("61.7\0");
			temp_str.Add("60.8\0");
			temp_str.Add("59.9\0");
			temp_str.Add("59.0\0");
			temp_str.Add("58.1\0");
			temp_str.Add("57.2\0");
			temp_str.Add("56.3\0");
			temp_str.Add("55.4\0");
			temp_str.Add("54.5\0");
			temp_str.Add("53.6\0");
			temp_str.Add("52.7\0");
			temp_str.Add("51.8\0");
			temp_str.Add("50.9\0");
			temp_str.Add("50.0\0");
			temp_str.Add("49.1\0");
			temp_str.Add("48.2\0");
			temp_str.Add("47.3\0");
			temp_str.Add("46.4\0");
			temp_str.Add("45.5\0");
			temp_str.Add("44.6\0");
			temp_str.Add("43.7\0");
			temp_str.Add("42.8\0");
			temp_str.Add("41.9\0");
			temp_str.Add("41.0\0");
			temp_str.Add("40.1\0");
			temp_str.Add("39.2\0");
			temp_str.Add("38.3\0");
			temp_str.Add("37.4\0");
			temp_str.Add("36.5\0");
			temp_str.Add("35.6\0");
			temp_str.Add("34.7\0");
			temp_str.Add("33.8\0");
			temp_str.Add("32.9\0");
			temp_str.Add("31.1\0");
			temp_str.Add("30.2\0");
			temp_str.Add("29.3\0");
			temp_str.Add("28.4\0");
			temp_str.Add("27.5\0");
			temp_str.Add("26.6\0");
			temp_str.Add("25.7\0");
			temp_str.Add("24.8\0");
			temp_str.Add("23.9\0");
			temp_str.Add("23.0\0");
			temp_str.Add("22.1\0");
			temp_str.Add("21.2\0");
			temp_str.Add("20.3\0");
			temp_str.Add("19.4\0");
			temp_str.Add("18.5\0");
			temp_str.Add("17.6\0");
			temp_str.Add("16.7\0");
			temp_str.Add("15.8\0");
			temp_str.Add("14.9\0");
			temp_str.Add("14.0\0");
			temp_str.Add("13.1\0");
			temp_str.Add("12.2\0");
			temp_str.Add("11.3\0");
			temp_str.Add("10.4\0");
			temp_str.Add("9.5\0"); 
			temp_str.Add("8.6\0"); 
			temp_str.Add("7.7\0"); 
			temp_str.Add("6.8\0"); 
			temp_str.Add("5.9\0"); 
			temp_str.Add("5.0\0"); 
			temp_str.Add("4.1\0"); 
			temp_str.Add("3.2\0"); 
			temp_str.Add("2.3\0"); 
			temp_str.Add("1.4\0"); 
			temp_str.Add("0.5\0"); 
			temp_str.Add("-0.4\0");
			temp_str.Add("-1.3\0");
			temp_str.Add("-2.2\0");
			temp_str.Add("-3.1\0");
			temp_str.Add("-4.0\0");
			temp_str.Add("-4.9\0");
			temp_str.Add("-5.8\0");
			temp_str.Add("-6.7\0");
			temp_str.Add("-7.6\0");
			temp_str.Add("-8.5\0");
			temp_str.Add("-9.4\0");
			temp_str.Add("-10.3\0");
			temp_str.Add("-11.2\0");
			temp_str.Add("-12.1\0");
			temp_str.Add("-13.0\0");
			temp_str.Add("-13.9\0");
			temp_str.Add("-14.8\0");
			temp_str.Add("-15.7\0");
			temp_str.Add("-16.6\0");
			temp_str.Add("-17.5\0");
			temp_str.Add("-18.4\0");
			temp_str.Add("-19.3\0");
			temp_str.Add("-20.2\0");
			temp_str.Add("-21.1\0");
			temp_str.Add("-22.0\0");
			temp_str.Add("-22.9\0");
			temp_str.Add("-23.8\0");
			temp_str.Add("-24.7\0");
			temp_str.Add("-25.6\0");
			temp_str.Add("-26.5\0");
			temp_str.Add("-27.4\0");
			temp_str.Add("-28.3\0");
			temp_str.Add("-29.2\0");
			temp_str.Add("-30.1\0");
			temp_str.Add("-31.0\0");
			temp_str.Add("-31.9\0");
			temp_str.Add("-32.8\0");
			temp_str.Add("-33.7\0");
			temp_str.Add("-34.6\0");
			temp_str.Add("-35.5\0");
			temp_str.Add("-36.4\0");
			temp_str.Add("-37.3\0");
			temp_str.Add("-38.2\0");
			temp_str.Add("-39.1\0");
			temp_str.Add("-40.0\0");
			temp_str.Add("-40.9\0");
			temp_str.Add("-41.8\0");
			temp_str.Add("-42.7\0");
			temp_str.Add("-43.6\0");
			temp_str.Add("-44.5\0");
			temp_str.Add("-45.4\0");
			temp_str.Add("-46.3\0");
			temp_str.Add("-47.2\0");
			temp_str.Add("-48.1\0");
			temp_str.Add("-49.0\0");
			temp_str.Add("-49.9\0");
			temp_str.Add("-50.8\0");
			temp_str.Add("-51.7\0");
			temp_str.Add("-52.6\0");
			temp_str.Add("-53.5\0");
			temp_str.Add("-54.4\0");
			temp_str.Add("-55.3\0");
			temp_str.Add("-56.2\0");
			temp_str.Add("-57.1\0");
			temp_str.Add("-58.0\0");
			temp_str.Add("-58.9\0");
			temp_str.Add("-59.8\0");
			temp_str.Add("-60.7\0");
			temp_str.Add("-61.6\0");
			temp_str.Add("-62.5\0");
			temp_str.Add("-63.4\0");
			temp_str.Add("-64.3\0");
			temp_str.Add("-65.2\0");
			temp_str.Add("-66.1\0");

            // initialize dictionary with DS1620 values for each temp F
            dtemps = new Dictionary<string, int>(359);
            dtemps.Add("257.0\0", 250);
            dtemps.Add("256.1\0", 249);
            dtemps.Add("255.2\0", 248);
            dtemps.Add("254.3\0", 247);
            dtemps.Add("253.4\0", 246);
            dtemps.Add("252.5\0", 245);
            dtemps.Add("251.6\0", 244);
            dtemps.Add("250.7\0", 243);
            dtemps.Add("249.8\0", 242);
            dtemps.Add("248.9\0", 241);
            dtemps.Add("248.0\0", 240);
            dtemps.Add("247.1\0", 239);
            dtemps.Add("246.2\0", 238);
            dtemps.Add("245.3\0", 237);
            dtemps.Add("244.4\0", 236);
            dtemps.Add("243.5\0", 235);
            dtemps.Add("242.6\0", 234);
            dtemps.Add("241.7\0", 233);
            dtemps.Add("240.8\0", 232);
            dtemps.Add("239.9\0", 231);
            dtemps.Add("239.0\0", 230);
            dtemps.Add("238.1\0", 229);
            dtemps.Add("237.2\0", 228);
            dtemps.Add("236.3\0", 227);
            dtemps.Add("235.4\0", 226);
            dtemps.Add("234.5\0", 225);
            dtemps.Add("233.6\0", 224);
            dtemps.Add("232.7\0", 223);
            dtemps.Add("231.8\0", 222);
            dtemps.Add("230.9\0", 221);
            dtemps.Add("230.0\0", 220);
            dtemps.Add("229.1\0", 219);
            dtemps.Add("228.2\0", 218);
            dtemps.Add("227.3\0", 217);
            dtemps.Add("226.4\0", 216);
            dtemps.Add("225.5\0", 215);
            dtemps.Add("224.6\0", 214);
            dtemps.Add("223.7\0", 213);
            dtemps.Add("222.8\0", 212);
            dtemps.Add("221.9\0", 211);
            dtemps.Add("221.0\0", 210);
            dtemps.Add("220.1\0", 209);
            dtemps.Add("219.2\0", 208);
            dtemps.Add("218.3\0", 207);
            dtemps.Add("217.4\0", 206);
            dtemps.Add("216.5\0", 205);
            dtemps.Add("215.6\0", 204);
            dtemps.Add("214.7\0", 203);
            dtemps.Add("213.8\0", 202);
            dtemps.Add("212.9\0", 201);
            dtemps.Add("212.0\0", 200);
            dtemps.Add("211.1\0", 199);
            dtemps.Add("210.2\0", 198);
            dtemps.Add("209.3\0", 197);
            dtemps.Add("208.4\0", 196);
            dtemps.Add("207.5\0", 195);
            dtemps.Add("206.6\0", 194);
            dtemps.Add("205.7\0", 193);
            dtemps.Add("204.8\0", 192);
            dtemps.Add("203.9\0", 191);
            dtemps.Add("203.0\0", 190);
            dtemps.Add("202.1\0", 189);
            dtemps.Add("201.2\0", 188);
            dtemps.Add("200.3\0", 187);
            dtemps.Add("199.4\0", 186);
            dtemps.Add("198.5\0", 185);
            dtemps.Add("197.6\0", 184);
            dtemps.Add("196.7\0", 183);
            dtemps.Add("195.8\0", 182);
            dtemps.Add("194.9\0", 181);
            dtemps.Add("194.0\0", 180);
            dtemps.Add("193.1\0", 179);
            dtemps.Add("192.2\0", 178);
            dtemps.Add("191.3\0", 177);
            dtemps.Add("190.4\0", 176);
            dtemps.Add("189.5\0", 175);
            dtemps.Add("188.6\0", 174);
            dtemps.Add("187.7\0", 173);
            dtemps.Add("186.8\0", 172);
            dtemps.Add("185.9\0", 171);
            dtemps.Add("185.0\0", 170);
            dtemps.Add("184.1\0", 169);
            dtemps.Add("183.2\0", 168);
            dtemps.Add("182.3\0", 167);
            dtemps.Add("181.4\0", 166);
            dtemps.Add("180.5\0", 165);
            dtemps.Add("179.6\0", 164);
            dtemps.Add("178.7\0", 163);
            dtemps.Add("177.8\0", 162);
            dtemps.Add("176.9\0", 161);
            dtemps.Add("176.0\0", 160);
            dtemps.Add("175.1\0", 159);
            dtemps.Add("174.2\0", 158);
            dtemps.Add("173.3\0", 157);
            dtemps.Add("172.4\0", 156);
            dtemps.Add("171.5\0", 155);
            dtemps.Add("170.6\0", 154);
            dtemps.Add("169.7\0", 153);
            dtemps.Add("168.8\0", 152);
            dtemps.Add("167.9\0", 151);
            dtemps.Add("167.0\0", 150);
            dtemps.Add("166.1\0", 149);
            dtemps.Add("165.2\0", 148);
            dtemps.Add("164.3\0", 147);
            dtemps.Add("163.4\0", 146);
            dtemps.Add("162.5\0", 145);
            dtemps.Add("161.6\0", 144);
            dtemps.Add("160.7\0", 143);
            dtemps.Add("159.8\0", 142);
            dtemps.Add("158.9\0", 141);
            dtemps.Add("158.0\0", 140);
            dtemps.Add("157.1\0", 139);
            dtemps.Add("156.2\0", 138);
            dtemps.Add("155.3\0", 137);
            dtemps.Add("154.4\0", 136);
            dtemps.Add("153.5\0", 135);
            dtemps.Add("152.6\0", 134);
            dtemps.Add("151.7\0", 133);
            dtemps.Add("150.8\0", 132);
            dtemps.Add("149.9\0", 131);
            dtemps.Add("149.0\0", 130);
            dtemps.Add("148.1\0", 129);
            dtemps.Add("147.2\0", 128);
            dtemps.Add("146.3\0", 127);
            dtemps.Add("145.4\0", 126);
            dtemps.Add("144.5\0", 125);
            dtemps.Add("143.6\0", 124);
            dtemps.Add("142.7\0", 123);
            dtemps.Add("141.8\0", 122);
            dtemps.Add("140.9\0", 121);
            dtemps.Add("140.0\0", 120);
            dtemps.Add("139.1\0", 119);
            dtemps.Add("138.2\0", 118);
            dtemps.Add("137.3\0", 117);
            dtemps.Add("136.4\0", 116);
            dtemps.Add("135.5\0", 115);
            dtemps.Add("134.6\0", 114);
            dtemps.Add("133.7\0", 113);
            dtemps.Add("132.8\0", 112);
            dtemps.Add("131.9\0", 111);
            dtemps.Add("131.0\0", 110);
            dtemps.Add("130.1\0", 109);
            dtemps.Add("129.2\0", 108);
            dtemps.Add("128.3\0", 107);
            dtemps.Add("127.4\0", 106);
            dtemps.Add("126.5\0", 105);
            dtemps.Add("125.6\0", 104);
            dtemps.Add("124.7\0", 103);
            dtemps.Add("123.8\0", 102);
            dtemps.Add("122.9\0", 101);
            dtemps.Add("122.0\0", 100);
            dtemps.Add("121.1\0", 99);
            dtemps.Add("120.2\0", 98);
            dtemps.Add("119.3\0", 97);
            dtemps.Add("118.4\0", 96);
            dtemps.Add("117.5\0", 95);
            dtemps.Add("116.6\0", 94);
            dtemps.Add("115.7\0", 93);
            dtemps.Add("114.8\0", 92);
            dtemps.Add("113.9\0", 91);
            dtemps.Add("113.0\0", 90);
            dtemps.Add("112.1\0", 89);
            dtemps.Add("111.2\0", 88);
            dtemps.Add("110.3\0", 87);
            dtemps.Add("109.4\0", 86);
            dtemps.Add("108.5\0", 85);
            dtemps.Add("107.6\0", 84);
            dtemps.Add("106.7\0", 83);
            dtemps.Add("105.8\0", 82);
            dtemps.Add("104.9\0", 81);
            dtemps.Add("104.0\0", 80);
            dtemps.Add("103.1\0", 79);
            dtemps.Add("102.2\0", 78);
            dtemps.Add("101.3\0", 77);
            dtemps.Add("100.4\0", 76);
            dtemps.Add("99.5\0", 75);
            dtemps.Add("98.6\0", 74);
            dtemps.Add("97.7\0", 73);
            dtemps.Add("96.8\0", 72);
            dtemps.Add("95.9\0", 71);
            dtemps.Add("95.0\0", 70);
            dtemps.Add("94.1\0", 69);
            dtemps.Add("93.2\0", 68);
            dtemps.Add("92.3\0", 67);
            dtemps.Add("91.4\0", 66);
            dtemps.Add("90.5\0", 65);
            dtemps.Add("89.6\0", 64);
            dtemps.Add("88.7\0", 63);
            dtemps.Add("87.8\0", 62);
            dtemps.Add("86.9\0", 61);
            dtemps.Add("86.0\0", 60);
            dtemps.Add("85.1\0", 59);
            dtemps.Add("84.2\0", 58);
            dtemps.Add("83.3\0", 57);
            dtemps.Add("82.4\0", 56);
            dtemps.Add("81.5\0", 55);
            dtemps.Add("80.6\0", 54);
            dtemps.Add("79.7\0", 53);
            dtemps.Add("78.8\0", 52);
            dtemps.Add("77.9\0", 51);
            dtemps.Add("77.0\0", 50);
            dtemps.Add("76.1\0", 49);
            dtemps.Add("75.2\0", 48);
            dtemps.Add("74.3\0", 47);
            dtemps.Add("73.4\0", 46);
            dtemps.Add("72.5\0", 45);
            dtemps.Add("71.6\0", 44);
            dtemps.Add("70.7\0", 43);
            dtemps.Add("69.8\0", 42);
            dtemps.Add("68.9\0", 41);
            dtemps.Add("68.0\0", 40);
            dtemps.Add("67.1\0", 39);
            dtemps.Add("66.2\0", 38);
            dtemps.Add("65.3\0", 37);
            dtemps.Add("64.4\0", 36);
            dtemps.Add("63.5\0", 35);
            dtemps.Add("62.6\0", 34);
            dtemps.Add("61.7\0", 33);
            dtemps.Add("60.8\0", 32);
            dtemps.Add("59.9\0", 31);
            dtemps.Add("59.0\0", 30);
            dtemps.Add("58.1\0", 29);
            dtemps.Add("57.2\0", 28);
            dtemps.Add("56.3\0", 27);
            dtemps.Add("55.4\0", 26);
            dtemps.Add("54.5\0", 25);
            dtemps.Add("53.6\0", 24);
            dtemps.Add("52.7\0", 23);
            dtemps.Add("51.8\0", 22);
            dtemps.Add("50.9\0", 21);
            dtemps.Add("50.0\0", 20);
            dtemps.Add("49.1\0", 19);
            dtemps.Add("48.2\0", 18);
            dtemps.Add("47.3\0", 17);
            dtemps.Add("46.4\0", 16);
            dtemps.Add("45.5\0", 15);
            dtemps.Add("44.6\0", 14);
            dtemps.Add("43.7\0", 13);
            dtemps.Add("42.8\0", 12);
            dtemps.Add("41.9\0", 11);
            dtemps.Add("41.0\0", 10);
            dtemps.Add("40.1\0", 9);
            dtemps.Add("39.2\0", 8);
            dtemps.Add("38.3\0", 7);
            dtemps.Add("37.4\0", 6);
            dtemps.Add("36.5\0", 5);
            dtemps.Add("35.6\0", 4);
            dtemps.Add("34.7\0", 3);
            dtemps.Add("33.8\0", 2);
            dtemps.Add("32.9\0", 1);
            dtemps.Add("31.1\0", 511);
            dtemps.Add("30.2\0", 510);
            dtemps.Add("29.3\0", 509);
            dtemps.Add("28.4\0", 508);
            dtemps.Add("27.5\0", 507);
            dtemps.Add("26.6\0", 506);
            dtemps.Add("25.7\0", 505);
            dtemps.Add("24.8\0", 504);
            dtemps.Add("23.9\0", 503);
            dtemps.Add("23.0\0", 502);
            dtemps.Add("22.1\0", 501);
            dtemps.Add("21.2\0", 500);
            dtemps.Add("20.3\0", 499);
            dtemps.Add("19.4\0", 498);
            dtemps.Add("18.5\0", 497);
            dtemps.Add("17.6\0", 496);
            dtemps.Add("16.7\0", 495);
            dtemps.Add("15.8\0", 494);
            dtemps.Add("14.9\0", 493);
            dtemps.Add("14.0\0", 492);
            dtemps.Add("13.1\0", 491);
            dtemps.Add("12.2\0", 490);
            dtemps.Add("11.3\0", 489);
            dtemps.Add("10.4\0", 488);
            dtemps.Add("9.5\0", 487);
            dtemps.Add("8.6\0", 486);
            dtemps.Add("7.7\0", 485);
            dtemps.Add("6.8\0", 484);
            dtemps.Add("5.9\0", 483);
            dtemps.Add("5.0\0", 482);
            dtemps.Add("4.1\0", 481);
            dtemps.Add("3.2\0", 480);
            dtemps.Add("2.3\0", 479);
            dtemps.Add("1.4\0", 478);
            dtemps.Add("0.5\0", 477);
            dtemps.Add("-0.4\0", 476);
            dtemps.Add("-1.3\0", 475);
            dtemps.Add("-2.2\0", 474);
            dtemps.Add("-3.1\0", 473);
            dtemps.Add("-4.0\0", 472);
            dtemps.Add("-4.9\0", 471);
            dtemps.Add("-5.8\0", 470);
            dtemps.Add("-6.7\0", 469);
            dtemps.Add("-7.6\0", 468);
            dtemps.Add("-8.5\0", 467);
            dtemps.Add("-9.4\0", 466);
            dtemps.Add("-10.3\0", 465);
            dtemps.Add("-11.2\0", 464);
            dtemps.Add("-12.1\0", 463);
            dtemps.Add("-13.0\0", 462);
            dtemps.Add("-13.9\0", 461);
            dtemps.Add("-14.8\0", 460);
            dtemps.Add("-15.7\0", 459);
            dtemps.Add("-16.6\0", 458);
            dtemps.Add("-17.5\0", 457);
            dtemps.Add("-18.4\0", 456);
            dtemps.Add("-19.3\0", 455);
            dtemps.Add("-20.2\0", 454);
            dtemps.Add("-21.1\0", 453);
            dtemps.Add("-22.0\0", 452);
            dtemps.Add("-22.9\0", 451);
            dtemps.Add("-23.8\0", 450);
            dtemps.Add("-24.7\0", 449);
            dtemps.Add("-25.6\0", 448);
            dtemps.Add("-26.5\0", 447);
            dtemps.Add("-27.4\0", 446);
            dtemps.Add("-28.3\0", 445);
            dtemps.Add("-29.2\0", 444);
            dtemps.Add("-30.1\0", 443);
            dtemps.Add("-31.0\0", 442);
            dtemps.Add("-31.9\0", 441);
            dtemps.Add("-32.8\0", 440);
            dtemps.Add("-33.7\0", 439);
            dtemps.Add("-34.6\0", 438);
            dtemps.Add("-35.5\0", 437);
            dtemps.Add("-36.4\0", 436);
            dtemps.Add("-37.3\0", 435);
            dtemps.Add("-38.2\0", 434);
            dtemps.Add("-39.1\0", 433);
            dtemps.Add("-40.0\0", 432);
            dtemps.Add("-40.9\0", 431);
            dtemps.Add("-41.8\0", 430);
            dtemps.Add("-42.7\0", 429);
            dtemps.Add("-43.6\0", 428);
            dtemps.Add("-44.5\0", 427);
            dtemps.Add("-45.4\0", 426);
            dtemps.Add("-46.3\0", 425);
            dtemps.Add("-47.2\0", 424);
            dtemps.Add("-48.1\0", 423);
            dtemps.Add("-49.0\0", 422);
            dtemps.Add("-49.9\0", 421);
            dtemps.Add("-50.8\0", 420);
            dtemps.Add("-51.7\0", 419);
            dtemps.Add("-52.6\0", 418);
            dtemps.Add("-53.5\0", 417);
            dtemps.Add("-54.4\0", 416);
            dtemps.Add("-55.3\0", 415);
            dtemps.Add("-56.2\0", 414);
            dtemps.Add("-57.1\0", 413);
            dtemps.Add("-58.0\0", 412);
            dtemps.Add("-58.9\0", 411);
            dtemps.Add("-59.8\0", 410);
            dtemps.Add("-60.7\0", 409);
            dtemps.Add("-61.6\0", 408);
            dtemps.Add("-62.5\0", 407);
            dtemps.Add("-63.4\0", 406);
            dtemps.Add("-64.3\0", 405);
            dtemps.Add("-65.2\0", 404);
            dtemps.Add("-66.1\0", 403);

			lights_str = new Dictionary<string,int>(13);

			lights_str.Add("1 second",1);
			lights_str.Add("2 seconds",2);
			lights_str.Add("3 seconds",3);
			lights_str.Add("5 seconds",5);
			lights_str.Add("10 seconds",10);
			lights_str.Add("15 seconds",15);
			lights_str.Add("30 seconds",30);
			lights_str.Add("1 minute",60);
			lights_str.Add("2 minutes",120);
			lights_str.Add("5 minutes", 300);
			lights_str.Add("10 minutes",600);
			lights_str.Add("30 minutes", 1800);
			lights_str.Add("1 hour",3600);

			timeout_str = new Dictionary<string, int>(7);
			timeout_str.Add("10 seconds",10);
			timeout_str.Add("15 seconds",15);
			timeout_str.Add("30 seconds",30);
			timeout_str.Add("1 minute",60);
			timeout_str.Add("2 minutes",120);
			timeout_str.Add("5 minutes",300);
			timeout_str.Add("10 minutes",600);

			foreach (string str in temp_str)
                cbFanOn.Items.Add(str);

            foreach (string str in temp_str)
                cbFanOff.Items.Add(str);

            foreach (string str in temp_str)
                cbBlowerEnabled.Items.Add(str);

            foreach (string str in temp_str)
                cbBlower3.Items.Add(str);

            foreach (string str in temp_str)
                cbBlower2.Items.Add(str);

            foreach (string str in temp_str)
                cbBlower1.Items.Add(str);

            foreach (string str in temp_str)
                cbTempLimit.Items.Add(str);

			/*
			cbFanOn.SelectedIndex = get_temp_index(get_temp_str(cfg.fan_on));
			textBox1.AppendText(cbFanOn.SelectedIndex.ToString());
			cbFanOn.SelectedText = get_temp_str(cfg.fan_on);
			cbFanOff.SelectedIndex = get_temp_index(get_temp_str(cfg.fan_off));
			cbBlower1.SelectedIndex = get_temp_index(get_temp_str(cfg.blower1_on));
			cbBlower2.SelectedIndex = get_temp_index(get_temp_str(cfg.blower2_on));
			cbBlower3.SelectedIndex = get_temp_index(get_temp_str(cfg.blower3_on));
			cbBlowerEnabled.SelectedIndex = get_temp_index(get_temp_str(cfg.blower_enabled));
			cbTempLimit.SelectedIndex = get_temp_index(get_temp_str(cfg.engine_temp_limit));
			cbRPM_MPHUpdateRate.SelectedIndex = cfg.rpm_mph_update_rate;
			cbFPGAXmitRate.SelectedIndex = cfg.FPGAXmitRate;
			cbHighRevLimit.SelectedIndex = cfg.high_rev_limit;
			cbLowRevLimit.SelectedIndex = cfg.low_rev_limit;
			cbLightsOnDelay.SelectedIndex = cfg.lights_on_delay;
			cbPasswordTimeout.SelectedIndex = cfg.password_timeout;
			cbPasswordRetries.SelectedIndex = cfg.password_retries;
			cbBaudRate3.SelectedIndex = cfg.baudrate3;
			cbTestBank.SelectedIndex = cfg.test_bank;
			password = cfg.password;
			*/
		}

		private void btnChangePassword_Clicked(object sender, EventArgs e)
		{
			Password change_password = new Password(password,13);
			if (change_password.ShowDialog(this) == DialogResult.OK)
			{
				password = change_password.GetPassword();
				tbNewPasssword.Visible = true;
				tbNewPasssword.Text = password;
			}
			else
			{
				//                this.txtResult.Text = "Cancelled";
			}

		}

		private void button1_Click(object sender, EventArgs e)
		{
			if (refreshed == false)
			{
				cbFanOn.SelectedIndex = get_temp_index(get_temp_str(cfg.fan_on));
				cbFanOff.SelectedIndex = get_temp_index(get_temp_str(cfg.fan_off));
				cbBlower1.SelectedIndex = get_temp_index(get_temp_str(cfg.blower1_on));
				cbBlower2.SelectedIndex = get_temp_index(get_temp_str(cfg.blower2_on));
				cbBlower3.SelectedIndex = get_temp_index(get_temp_str(cfg.blower3_on));
				cbBlowerEnabled.SelectedIndex = get_temp_index(get_temp_str(cfg.blower_enabled));
				cbTempLimit.SelectedIndex = get_temp_index(get_temp_str(cfg.engine_temp_limit));
				cbRPM_MPHUpdateRate.SelectedIndex = cfg.rpm_mph_update_rate;
				cbFPGAXmitRate.SelectedIndex = cfg.FPGAXmitRate;
				cbHighRevLimit.SelectedIndex = cfg.high_rev_limit;
				cbLowRevLimit.SelectedIndex = cfg.low_rev_limit;
				cbLightsOnDelay.SelectedIndex = cfg.lights_on_delay;
				cbPasswordTimeout.SelectedIndex = cfg.password_timeout;
				cbPasswordRetries.SelectedIndex = cfg.password_retries;
				cbBaudRate3.SelectedIndex = cfg.baudrate3;
				cbTestBank.SelectedIndex = cfg.test_bank;
				password = cfg.password;
				refreshed = true;
				cfg.set = true;
			}
			/*
			textBox1.AppendText(cbFanOff.SelectedIndex.ToString() + " " + cfg.fan_off.ToString()+"\r\n");
			textBox1.AppendText(cbFanOn.SelectedIndex.ToString() + " " + cfg.fan_on.ToString() + "\r\n");
			textBox1.AppendText(cbBlower1.SelectedIndex.ToString() + " " + cfg.blower1_on.ToString() + "\r\n");
			textBox1.AppendText(cbBlower2.SelectedIndex.ToString() + " " + cfg.blower2_on.ToString() + "\r\n");
			textBox1.AppendText(get_temp_str(cfg.fan_off) + "\r\n");
			textBox1.AppendText(get_temp_str(cfg.fan_on) + "\r\n");
			textBox1.AppendText(get_temp_str(cfg.blower1_on) + "\r\n");
			textBox1.AppendText(get_temp_str(cfg.blower2_on) + "\r\n");
			textBox1.AppendText("\r\n" + get_temp_index(get_temp_str(cfg.fan_on)).ToString() + "\r\n");
			textBox1.AppendText(get_temp_index(get_temp_str(cfg.fan_off)).ToString() + "\r\n");
			*/
		}

		private void btnSendUpdateRates_Click(object sender, EventArgs e)
		{
			byte[] rpm_mph = BitConverter.GetBytes(cfg.rpm_mph_update_rate);
			byte[] bytes = new byte[rpm_mph.Count() + 2];
			bytes[0] = svrcmd.GetCmdIndexB("SET_RPM_MPH_RATE");
			System.Buffer.BlockCopy(rpm_mph, 0, bytes, 2, rpm_mph.Count());
			Packet packet = new Packet(bytes, 0, bytes.Count(), false);
			//m_client.Send(packet);

			rpm_mph = BitConverter.GetBytes(cfg.FPGAXmitRate);
			bytes = new byte[rpm_mph.Count() + 2];
			bytes[0] = svrcmd.GetCmdIndexB("SET_FPGA_RATE");
			System.Buffer.BlockCopy(rpm_mph, 0, bytes, 2, rpm_mph.Count());
			packet = new Packet(bytes, 0, bytes.Count(), false);
			m_client.Send(packet);
		}
	}
}
