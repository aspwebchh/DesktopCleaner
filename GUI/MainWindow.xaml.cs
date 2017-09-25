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
using System.Data;

namespace GUI {
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window {
        public MainWindow() {
            InitializeComponent();

            var result = ToTable(FileInfoList());
            listView.DataContext = result;
        }


        private DataTable ToTable( List<Dictionary<string, object>> fileList) {
            var dt = new DataTable();
            var fileNameCol = new DataColumn("FileName", Type.GetType("System.String"));
            var fileExtCol = new DataColumn("FileExt", Type.GetType("System.String"));
            dt.Columns.Add(fileNameCol);
            dt.Columns.Add(fileExtCol);
            fileList.ForEach(item => {
                var row = dt.NewRow();
                row ["FileName"] = item ["FileName"];
                row ["FileExt"] = item ["FileExt"];
                dt.Rows.Add(row);
            });
            return dt;
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
