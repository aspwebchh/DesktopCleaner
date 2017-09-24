using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Web.Script.Serialization;
using Newtonsoft.Json;

namespace GUI {
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window {
        public MainWindow() {
            InitializeComponent();

            var result = FileInfoList();
            MessageBox.Show(result.Count.ToString());
        }

        private List<Dictionary<string, object>> FileInfoList() {
            IntPtr intPtr = CFunction.GetFileInfoList();
            string jsonString = Marshal.PtrToStringAnsi(intPtr);
            var jsonObject = JsonConvert.DeserializeObject<Dictionary<string, List<Dictionary<string, object>>>>(jsonString);
            var result = new List<Dictionary<string, object>>();
            foreach( var key in jsonObject.Keys ) {
                var val = jsonObject [key];
                val.ForEach(item => result.Add(item));
            }
            return result;
        }

    }
}
