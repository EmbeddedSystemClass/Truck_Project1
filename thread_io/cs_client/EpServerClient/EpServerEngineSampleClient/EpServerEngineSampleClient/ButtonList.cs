using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EpServerEngineSampleClient
{
	public class ButtonList : IEquatable<ButtonList>
	{
		public int TabOrder { get; set; }
		public Button Ctl { get; set; }
		public bool Enabled { get; set; }
		public string Name { get; set; }


		public override string ToString()
		{
			return Name + " " + TabOrder.ToString();
		}
		public override bool Equals(object obj)
		{
			if (obj == null) return false;
			ButtonList objAsPart = obj as ButtonList;
			if (objAsPart == null) return false;
			else return Equals(objAsPart);
		}
		public override int GetHashCode()
		{
			return TabOrder;
		}
		public bool Equals(ButtonList other)
		{
			if (other == null) return false;
			return (this.TabOrder.Equals(other.TabOrder));
		}
/*
		public void SetColor(ButtonList btn)
		{
			btn.Ctl.ForeColor = (Color)
		}
*/
	}
	/*
		class ButtonList
		{
			public int tab { get; set; }
			public Button ctl { get; set; }
			public bool enabled { get; set; }
		}
	*/
}
