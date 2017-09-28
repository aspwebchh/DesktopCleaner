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

        private DataTable dataSource;
        private bool isCheckAll = false;
        public MainWindow() {
            InitializeComponent();

            this.dataSource = ToTable(FileInfoList());
            listView.DataContext = dataSource;
        }

        private void Button_Click( object sender, RoutedEventArgs e ) {
            isCheckAll = isCheckAll ? false : true;
            foreach( DataRow row in dataSource.Rows ) {
                row ["IsChecked"] = isCheckAll;
            }
            listView.DataContext = dataSource;
        }

        private void Button_Click_1( object sender, RoutedEventArgs e ) {
            var id = ( sender as Button ).CommandParameter as String;
            var param = Encoding.Default.GetBytes(id.ToCharArray());
            IntPtr intPtr = CFunction.ClearItem(ref param[0]);
            string jsonString = Marshal.PtrToStringAnsi(intPtr);
            MessageBox.Show(jsonString);
        }

        private DataTable ToTable( List<Dictionary<string, object>> fileList) {
            var dt = new DataTable();
            var fileNameCol = new DataColumn("FileName", Type.GetType("System.String"));
            var fileExtCol = new DataColumn("FileExt", Type.GetType("System.String"));
            var idCol = new DataColumn("ID", Type.GetType("System.String"));
            var checkCol = new DataColumn("IsChecked", Type.GetType("System.Boolean"));
            dt.Columns.Add(fileNameCol);
            dt.Columns.Add(fileExtCol);
            dt.Columns.Add(idCol);
            dt.Columns.Add(checkCol);
            fileList.ForEach(item => {
                var row = dt.NewRow();
                row ["FileName"] = item ["FileName"];
                row ["FileExt"] = item ["FileExt"];
                row ["ID"] = item ["ID"];
                row ["IsChecked"] = false;
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
