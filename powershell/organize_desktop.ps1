

$path = Split-Path -Parent $MyInvocation.MyCommand.Definition
$path = $path + "\"

function getFiles() {
   return dir -Path $path | Where-Object {$_.Name -notmatch "^\d+_\d+_\d+$" -and $_.Name -notmatch "(\.lnk)|(\.url)|(\.ps1)" }
}


function execMove( $fileInfo ) {
    $ext = [System.IO.Path]::GetExtension($fileInfo.FullName)
    $dirPath = $path + [DateTime]::Now.ToString("yyyy_MM_dd") + "\" + $ext
    $dirExists = [System.IO.Directory]::Exists($dirPath)
    if ($dirExists -eq $FALSE) {
         [System.IO.Directory]::CreateDirectory($dirPath)
    }
    $src = $fileInfo.FullName
    $dest = $dirPath + "\" + $fileInfo.Name
    copy-item -Path $src -Destination $dest -Recurse -Force
    remove-item -Path $src -Recurse -ErrorAction "SilentlyContinue"
}

getFiles | ForEach-Object { execMove $_ }

