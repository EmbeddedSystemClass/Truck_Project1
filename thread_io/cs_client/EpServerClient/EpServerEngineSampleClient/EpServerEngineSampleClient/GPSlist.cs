using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EpServerEngineSampleClient
{
	public class GPSlist : IEquatable<GPSlist>
	{
		public int index { get; set; }
		public string Name { get; set; }
		public int update_rate { get; set; }
		public bool selected { get; set; }

		public override string ToString()
		{
			return Name + " " + index.ToString();
		}
		public override bool Equals(object obj)
		{
			if (obj == null) return false;
			GPSlist objAsPart = obj as GPSlist;
			if (objAsPart == null) return false;
			else return Equals(objAsPart);
		}
		public override int GetHashCode()
		{
			return index;
		}
		public bool Equals(GPSlist other)
		{
			if (other == null) return false;
			return (this.index.Equals(other.index));
		}
	}
}
