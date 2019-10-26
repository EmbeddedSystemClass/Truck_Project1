using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EpServerEngineSampleServer
{
    public class ConfigParams
    {
        public int fan_on { get; set; }
        public int fan_off { get; set; }
        public int rpm_update_rate { get; set; }
        public int mph_update_rate { get; set; }
        public int high_rev_limit { get; set; }
        public int low_rev_limit { get; set; }
        public int FPGAXmitRate { get; set; }
        public int blower_enabled { get; set; }
        public int blower1_on { get; set; }
        public int blower2_on { get; set; }
        public int blower3_on { get; set; }
        public int lights_on_delay { get; set; }
        public int engine_temp_limit { get; set; }
        public int battery_box_temp { get; set; }
        public int test_bank { get; set; }

        public int si_fan_on { get; set; }
        public int si_fan_off { get; set; }
        public int si_rpm_update_rate { get; set; }
        public int si_mph_update_rate { get; set; }
        public int si_high_rev_limit { get; set; }
        public int si_low_rev_limit { get; set; }
        public int si_FPGAXmitRate { get; set; }
        public int si_blower_enabled { get; set; }
        public int si_blower1_on { get; set; }
        public int si_blower2_on { get; set; }
        public int si_blower3_on { get; set; }
        public int si_lights_on_delay { get; set; }
        public int si_engine_temp_limit { get; set; }
        public int si_test_bank { get; set; }
        public int si_battery_box_temp { get; set; }
        public bool set { get; set; }
    }
}
