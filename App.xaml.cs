using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;

namespace DesktopCleaner {
    /// <summary>
    /// App.xaml 的交互逻辑
    /// </summary>
    public partial class App : Application {
        public App() {
            this.DispatcherUnhandledException += App_DispatcherUnhandledException;
            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
        }

        private void CurrentDomain_UnhandledException( object sender, UnhandledExceptionEventArgs e ) {
            var ex = e.ExceptionObject as Exception;
            MessageBox.Show( ex.Message );
            MessageBox.Show( ex.StackTrace );
        }

        private void App_DispatcherUnhandledException( object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e ) {
            MessageBox.Show( Environment.NewLine + e.Exception.Message );
            e.Handled = true;
        }
    }
}
