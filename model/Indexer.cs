using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.IO;
using System.Threading.Tasks;

namespace DesktopCleaner.model {
    public static class Indexer {
        const string SRC_PATH = "paths.txt";

        public static void Run() {
            var path = FileModel.GetTargetPath();
            if( string.IsNullOrEmpty( path ) ) {
                return;
            }
            var paths = Scan( path );
            SavePaths( paths );
        }

        public static List<string> Find(string keyword) {
            Func<string, bool> finder = delegate ( string item ) {
                var fileName = Path.GetFileName( item );
                if( fileName.IndexOf( keyword ) != -1 ) {
                    return true;
                } else {
                    return false;
                }
            };
            var paths = GetPaths();
            return paths.Where( finder ).ToList();

        }

       

        private static List<string> GetPaths() {
            using( var fs = new FileStream( SRC_PATH, FileMode.OpenOrCreate, FileAccess.Read ) ) {
                var sw = new StreamReader( fs );
                var text = sw.ReadToEnd();
                return text.Split( new string[] { Environment.NewLine }, StringSplitOptions.RemoveEmptyEntries ).ToList();
            }
        }


        private static void SavePaths( List<string> paths ) {
            using( var fs = new FileStream( SRC_PATH, FileMode.OpenOrCreate, FileAccess.Write ) ) {
                var sw = new StreamWriter( fs );
                var text = String.Join( Environment.NewLine, paths.ToArray() );
                sw.Write( text );
            }
        } 

        private static List<string> Scan( string path ) {
            var results = new List<string>();
            var dirInfo = new DirectoryInfo( path );
            foreach( var fileInfoItem in dirInfo.GetFiles() ) {
                results.Add( fileInfoItem.FullName );
            }

            var tasks = new List<Task<List<string>>>();
            foreach( var dirInfoItem in dirInfo.GetDirectories() ) {
                tasks.Add( Task.Factory.StartNew( delegate {
                    return Scan( dirInfoItem.FullName );
                } ) );
            }
            tasks.ForEach( task => results.AddRange( task.Result ) );
            return results;
        }
    }
}
