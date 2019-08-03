using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EpServerEngineSampleClient
{
	class ClientParams
	{
		public bool AutoConn { get; set; }
		public string IPAdress { get; set; }
		public int PortNo { get; set; }
		public bool Primary { get; set; }
		public int AttemptsToConnect { get; set; }
	}
}
