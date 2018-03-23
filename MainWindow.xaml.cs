using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Data;
using System.Threading;
using DesktopCleaner.model;
using System.Threading.Tasks;
using System.Drawing;
using System.IO;


namespace DesktopCleaner {
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window {
        public const string TYPE_ALL = "所有";
        private DataTable dataSource;
        private bool isCheckAll = false;
        private string currType = TYPE_ALL;
        public MainWindow() {
            InitializeComponent();

            InitTypes();

            if( ViewTargetPath() ) {
                InitPage();
            } else {
                ThreadPool.QueueUserWorkItem( delegate {
                    Thread.Sleep( 1000 );
                    Dispatcher.Invoke( new System.Windows.Forms.MethodInvoker( delegate {
                        MessageBox.Show( "请先设置放置被清理文件的位置" );
                    } ) );
                } );
            }

            ThreadPool.QueueUserWorkItem( delegate {
                model.Indexer.Run();
            } );
        }

        private void InitTypes() {
            ThreadPool.QueueUserWorkItem( delegate {
                var types = FileModel.GetFileTypes();
                types.Insert( 0, TYPE_ALL );
                Dispatcher.Invoke( (Action)delegate {
                    FileType.ItemsSource = types;
                    FileType.SelectedIndex = 0;
                } );
            } );
        }

        private void InitPage() {
            Refesh( true );
            RefreshDirSize();
        }

        private bool ViewTargetPath() {
            string targetPath = FileModel.GetTargetPath();
            if( targetPath == "" ) {
                return false;
            }
            TargetPathView.Text = targetPath;
            return true;
        }

        private List<string> GetDirIDs() {
            var result = dataSource.Rows.Cast<DataRow>()
                .Where( item => item[ "FileExt" ].ToString().ToLower() == "文件夹" )
                .Select( item => item[ "ID" ].ToString() )
                .ToList();
            return result;
        }

        private void UpdateDirSize( string id, int dirSize ) {
            lock( dataSource ) {
                foreach( DataRow row in dataSource.Rows ) {
                    var fileID = row[ "ID" ].ToString();
                    if( fileID == id ) {
                        row[ "FileSize" ] = FormatFileSize( dirSize.ToString() );
                    }
                }
            }
        }

        private string FindPathByID( string id ) {
            return dataSource.Rows.Cast<DataRow>().Where( item => item[ "ID" ].ToString() == id ).Select( item => item[ "FilePath" ].ToString() ).FirstOrDefault();
        }

        private void RefreshDirSize() {
            ThreadPool.QueueUserWorkItem( delegate {
                var dirIDs = GetDirIDs();
                dirIDs.Select( id => {
                    return new {
                        ID = id,
                        Task = Task.Factory.StartNew<long>( delegate {
                            var path = FindPathByID( id );
                            var dirSize = FileModel.ComputeDirSize( path );
                            return dirSize;
                        } )
                    };
                } )
                .ToList()
                .ForEach( item => {
                    UpdateDirSize( item.ID, (int)item.Task.Result );
                } );
                Dispatcher.Invoke( new System.Windows.Forms.MethodInvoker( delegate {
                    Refesh();
                } ) );
            } );
        }

        private void Button_Click( object sender, RoutedEventArgs e ) {
            isCheckAll = isCheckAll ? false : true;
            foreach( DataRow row in dataSource.Rows ) {
                row[ "IsChecked" ] = isCheckAll;
            }
            listView.DataContext = dataSource;
        }

        private void Button_Click_1( object sender, RoutedEventArgs e ) {
            var id = ( sender as Button ).CommandParameter as String;
            var result = ClearItem( id );
            if( !result.Item1 ) {
                MessageBox.Show( result.Item2 );
            }
            InitPage();
        }

        private void Button_Click_2( object sender, RoutedEventArgs e ) {
            var ids = this.GetSelectIDs();
            if( ids.Count() == 0 ) {
                MessageBox.Show( "请选择要清理的项目" );
                return;
            }
            var result = ids.Select( item => {
                return Task.Factory.StartNew<Tuple<bool, string>>( () => ClearItem( item ) );
            } )
            .Select( task => task.Result )
            .Where( item => !item.Item1 );
            if( result.Count() != 0 ) {
                MessageBox.Show( result.Count() + "项目清理失败" );
            }
            InitPage();
        }

        private Tuple<bool, string> ClearItem( string id ) {
            var success = FileModel.ClearItem( FindPathByID( id ) );
            return Tuple.Create( success, success ? "清理成功" : "清理失败" );
        }

        private List<string> GetSelectIDs() {
            var selectIDs = new List<string>();
            foreach( DataRow row in this.dataSource.Rows ) {
                if( Boolean.Parse( row[ "IsChecked" ].ToString() ) ) {
                    selectIDs.Add( row[ "ID" ].ToString() );
                }
            }
            return selectIDs;
        }

        private void Refesh( bool reload = false ) {
            if( reload ) {
                dataSource = ToTable( FileModel.FileInfoList( currType ) );
                MsgBar.Text = dataSource.Rows.Count + "个对象";
            }
            listView.DataContext = dataSource;
        }

        private String FormatFileSize( string b ) {
            int val = Convert.ToInt32( b );
            if( val == 0 ) {
                return "0";
            }
            return ( Math.Ceiling( (decimal)val / 1024 ) ).ToString();
        }



        private DataTable ToTable( List<Dictionary<string, object>> fileList ) {
            var dt = new DataTable();
            var fileNameCol = new DataColumn( "FileName", Type.GetType( "System.String" ) );
            var fileExtCol = new DataColumn( "FileExt", Type.GetType( "System.String" ) );
            var idCol = new DataColumn( "ID", Type.GetType( "System.String" ) );
            var checkCol = new DataColumn( "IsChecked", Type.GetType( "System.Boolean" ) );
            var fileSizeCol = new DataColumn( "FileSize", Type.GetType( "System.String" ) );
            var filePathCol = new DataColumn( "FilePath", Type.GetType( "System.String" ) );
            var accessTime = new DataColumn( "Last_Access_Time", Type.GetType( "System.String" ) );
            dt.Columns.Add( fileNameCol );
            dt.Columns.Add( fileExtCol );
            dt.Columns.Add( idCol );
            dt.Columns.Add( checkCol );
            dt.Columns.Add( fileSizeCol );
            dt.Columns.Add( filePathCol );
            dt.Columns.Add( accessTime );
            fileList.ForEach( item => {
                var row = dt.NewRow();
                row[ "FilePath" ] = item[ "FilePath" ];
                row[ "FileName" ] = item[ "FileName" ];
                row[ "FileExt" ] = item[ "FileExt" ];
                row[ "ID" ] = item[ "ID" ];
                row[ "IsChecked" ] = false;
                row[ "FileSize" ] = FormatFileSize( item[ "FileSize" ].ToString() );
                row[ "Last_Access_Time" ] = item[ "Last_Access_Time" ];
                dt.Rows.Add( row );
            } );
            return dt;
        }

        private void Button_Click_3( object sender, RoutedEventArgs e ) {
            var folder = new System.Windows.Forms.FolderBrowserDialog();
            if( folder.ShowDialog() == System.Windows.Forms.DialogResult.OK ) {
                FileModel.SetTargetPath( folder.SelectedPath );
                InitPage();
                ViewTargetPath();
            }
        }

        private void FileType_SelectionChanged( object sender, SelectionChangedEventArgs e ) {
            var selectValue = FileType.SelectedValue.ToString();
            currType = selectValue;
            InitPage();
        }

        private void listView_MouseLeftButtonDown( object sender, System.Windows.Input.MouseButtonEventArgs e ) {
            var selectItem = listView.SelectedItem as DataRowView;
            var path = selectItem[ "FilePath" ].ToString();
            try {
                System.Diagnostics.Process.Start( path );
            } catch( Exception ex ) {
                MessageBox.Show( ex.Message );
            }
        }

        private void TargetPathView_MouseUp( object sender, System.Windows.Input.MouseButtonEventArgs e ) {
            var path = TargetPathView.Text;
            try {
                System.Diagnostics.Process.Start( path );
            } catch( Exception ex ) {
                MessageBox.Show( ex.Message );
            }
        }

        private void MenuItem_Click_Clear( object sender, RoutedEventArgs e ) {
            var selectItem = listView.SelectedItem as DataRowView;
            var id = selectItem[ "ID" ].ToString();
            var result = ClearItem( id );
            if( !result.Item1 ) {
                MessageBox.Show( result.Item2 );
            }
            InitPage();
        }

        private void MenuItem_Click_Del( object sender, RoutedEventArgs e ) {
            var selectItem = listView.SelectedItem as DataRowView;
            var id = selectItem[ "ID" ].ToString();
            var path = this.FindPathByID( id );
            MessageBoxResult dr = MessageBox.Show( "文件删除后将不可恢复！", "提示", MessageBoxButton.OKCancel, MessageBoxImage.Question );
            if( dr == MessageBoxResult.OK ) {
                FileModel.DeleteItem( path );
                InitPage();
            }
        }
    }
}
