using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	public static class Application {
		public static uint width => InternalCalls.application_getWidth();
		public static uint height => InternalCalls.application_getHeight();
		public static uint aspectRatio => width / height;

		public static void exit(int code) { InternalCalls.application_exit(code); }
	}
}
