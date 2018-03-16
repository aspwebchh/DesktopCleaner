using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DesktopCleaner.common {
    public static class Common {
        public static string MD5( string str ) {
            return System.Web.Security.FormsAuthentication.HashPasswordForStoringInConfigFile( str, "MD5" );
        }
    }
}
