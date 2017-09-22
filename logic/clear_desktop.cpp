#include "stdafx.h"
#include <iostream>
#include <string>
#include <Windows.h>
#include   <shlobj.h>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include "common.h"


using namespace std;


string getDesktopPath() {
	TCHAR szPath[MAX_PATH];
	SHGetSpecialFolderPath(0, szPath, CSIDL_DESKTOPDIRECTORY, 0);
	auto path = ws2s(szPath);
	regex reg("(.+\\w).*");
	smatch mc;
	if (regex_match(path, mc, reg)) {
		path = mc[1];
	}
	return path;
}

int main() {
	auto path = getDesktopPath();
	auto files = getFiles(path);
	for (int i = 0; i < files.size(); i++) {
		cout << files[i].ext << endl;
	}
	cin.get();
    return 0;
}

