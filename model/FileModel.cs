using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using DesktopCleaner.common;

namespace DesktopCleaner.model {
    public static class FileModel {

        private const string CONFIG_FILE_NAME = "config.conf";

        public static void SetTargetPath( string path ) {
            using( var file = new FileStream( CONFIG_FILE_NAME, FileMode.Truncate ) ) {
                var sw = new StreamWriter( file );
                sw.Write( path );
                sw.Flush();
            }
        }

        public static string GetTargetPath() {
            using( var file = new FileStream( CONFIG_FILE_NAME, FileMode.OpenOrCreate ) ) {
                var sr = new StreamReader( file );
                string path = sr.ReadToEnd();
                if(!Directory.Exists(path))
                {
                    return string.Empty;
                }
                return path;
            }
        }

        public static long ComputeDirSize( string dirPath ) {
            long size = 0;
            var folder = new DirectoryInfo( dirPath );
            foreach( var fileInfo in folder.GetFiles() ) {
                size += fileInfo.Length;
            }
            foreach( var dirInfo in folder.GetDirectories() ) {
                size += ComputeDirSize( dirInfo.FullName );
            }
            return size;
        }

        public static List<Dictionary<string, object>> FileInfoList(string type) {
            var data = FileInfoList();
            if( string.IsNullOrEmpty( type ) || type == MainWindow.TYPE_ALL ) {
                return data;
            }
            return data.Where( item => item[ "FileExt" ].ToString() == type ).ToList();
        }

        public static List<Dictionary<string, object>> FileInfoList() {
            string deskTopPath = Environment.GetFolderPath( Environment.SpecialFolder.DesktopDirectory );
            var folder = new DirectoryInfo( deskTopPath );
            var result = folder.GetFiles()
                .Where( item => !item.Name.EndsWith( ".LNK", StringComparison.OrdinalIgnoreCase ) )
                .Cast<FileSystemInfo>()
                .Union( folder.GetDirectories() )
                .Select( fileInfo => {
                    var isFile = fileInfo is FileInfo;
                    Func<string> getExt = delegate {
                        if( isFile ) {
                            return string.IsNullOrEmpty( fileInfo.Extension ) ? "Without-Extension" : fileInfo.Extension.Substring( 1, fileInfo.Extension.Length - 1 );
                        } else {
                            return "folder";
                        }
                    };
                    var item = new Dictionary<string, object>();
                    item[ "ID" ] = common.Common.MD5( fileInfo.FullName ).ToLower();
                    item[ "FileName" ] = fileInfo.Name;
                    item[ "FileExt" ] = FileTypeDesc( getExt() );
                    item[ "FilePath" ] = fileInfo.FullName;
                    item[ "FileSize" ] = isFile ? ( fileInfo as FileInfo ).Length : 0;
                    item[ "FileType" ] = isFile ? 1 : 0;
                    item[ "Last_Access_Time" ] = fileInfo.LastAccessTime.ToShortDateString();
                    return item;
                } )
                .OrderBy( item => item[ "FileName" ] )
                .OrderByDescending( item => item[ "FileType" ] )
                .ToList();
            return result;
        }

        public static List<string> GetFileTypes() {
            return FileInfoList().Select( item => item[ "FileExt" ].ToString() ).Distinct().ToList();
        }

        private static string FileTypeDesc( string fileType ) {
            if( string.IsNullOrEmpty( fileType ) ) {
                return "";
            }
            fileType = fileType.ToLower();

            var dic = new Dictionary<string, string>();
            dic.Add( "doc", "word文档" );
            dic.Add( "xls", "excel文档" );
            dic.Add( "ppt", "ppt文档" );
            dic.Add( "docx", "word文档" );
            dic.Add( "xlsx", "excel文档" );
            dic.Add( "pptx", "ppt文档" );
            dic.Add( "rar", "rar压缩包" );
            dic.Add( "zip", "zip压缩包" );
            dic.Add( "html", "html文档" );
            dic.Add( "htm", "htm文档" );
            dic.Add( "jpg", "jpg图片" );
            dic.Add( "gif", "gif图片" );
            dic.Add( "png", "png图片" );
            dic.Add( "folder", "文件夹" );
            dic.Add( "txt", "文本文件" );
            dic.Add( "text", "文本文件" );
            dic.Add( "xml", "xml文件" );
            dic.Add( "lnk", "快捷方式" );
            dic.Add( "psd", "ps源文件" );

            if( dic.ContainsKey( fileType ) ) {
                return dic[ fileType ];
            } else {
                return fileType;
            }
        }

        private static string RenamePathName( string path ) {
            var dir = Path.GetDirectoryName( path );
            var extName = Path.GetExtension( path );
            var fileName = Path.GetFileNameWithoutExtension( path );
            var regex = new Regex( @"\((?<count>\d+)\)$" );
            var newFileName = string.Empty;
            if( regex.IsMatch( fileName ) ) {
                newFileName = regex.Replace( fileName, match => {
                    var mCount = match.Groups[ "count" ];
                    return "(" + ( Convert.ToInt32( mCount.Value ) + 1 ) + ")";
                } );
            } else {
                newFileName = fileName + "(2)";
            }
            var result = dir + "\\" + newFileName + extName;
            return result;
        }

        public static bool ClearItem( string path ) {
            var dirName = DateTime.Now.ToString( "yyyy-MM-dd" );
            var targetPath = GetTargetPath();
            var targetDir = targetPath + "\\" + dirName;
            if( !Directory.Exists( targetDir ) ) {
                Directory.CreateDirectory( targetDir );
            }
            if( File.Exists( path ) ) {
                var fileInfo = new FileInfo( path );
                var targetFilePath = targetDir + "\\" + fileInfo.Name;
                while( File.Exists( targetFilePath ) ) {
                    targetFilePath = RenamePathName( targetFilePath );
                }
                fileInfo.MoveTo( targetFilePath );
            } else if( Directory.Exists( path ) ) {
                var dirInfo = new DirectoryInfo( path );
                var targetDirPath = targetDir + "\\" + dirInfo.Name;
                while( Directory.Exists( targetDirPath ) ) {
                    targetDirPath = RenamePathName( targetDirPath );
                }
                MoveDir( dirInfo, dirInfo.FullName, targetDirPath );
                dirInfo.Delete( true );
            } else {
                return false;
            }
            return true;
        }

        private static void MoveDir( DirectoryInfo dirInfo, string srcRootPath,  string descRootPath ) {
            var descDirPath = dirInfo.FullName.Replace( srcRootPath, descRootPath );
            if( !Directory.Exists( descDirPath ) ) {
                Directory.CreateDirectory( descDirPath );
            }

            var tasks = new List<Task>();

            foreach( var fileItem in dirInfo.GetFiles() ) {
                var task = Task.Factory.StartNew( delegate {
                    var descPath = fileItem.FullName.Replace( srcRootPath, descRootPath );
                    fileItem.MoveTo( descPath );
                } );
                tasks.Add( task );
            }
            foreach( var dirItem in dirInfo.GetDirectories() ) {
                var task = Task.Factory.StartNew( delegate {
                    MoveDir( dirItem, srcRootPath, descRootPath );
                } );
                tasks.Add( task );
            }

            tasks.ForEach( task => task.Wait() );
        }

        public static void DeleteItem( string path ) {
            if( File.Exists( path ) ) {
                File.Delete( path );
            } else {
                if( Directory.Exists( path ) ) {
                    Directory.Delete( path, true );
                }
            }
        }
    }
}
