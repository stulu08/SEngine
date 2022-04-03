using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	public class Application {
		public uint width => InternalCalls.application_getWidth();
		public uint height => InternalCalls.application_getHeight();
		public uint aspectRatio => width / height;

		public static void exit(int code) { InternalCalls.application_exit(code); }
	}
}
