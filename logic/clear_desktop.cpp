#include "stdafx.h"
#include <iostream>
#include <string>
#include <Windows.h>
#include <shlobj.h>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <map>
#include "common.h"


using namespace std;

void add2Group(string key, FileItem fileItem, map<string, vector<FileItem>> &dataSource) {
	map<string, vector<FileItem>>::iterator found = dataSource.find(key);
	if (found != dataSource.end()) {
		found->second.push_back(fileItem);
	}
	else {
		vector<FileItem> fileItems;
		fileItems.push_back(fileItem);
		dataSource.insert(pair<string, vector<FileItem>>(key, fileItems));
	}
}

map<string, vector<FileItem>> group(vector<FileItem> files) {
	map<string, vector<FileItem>> dataSource;
	for (int i = 0; i < files.size(); i++) {
		auto fileItem = files[i];
		add2Group(fileItem.ext, fileItem, dataSource);
	}
	map<string, vector<FileItem>>::iterator it;
	return dataSource;
}

string getDesktopPath() {
	TCHAR szPath[MAX_PATH];
	SHGetSpecialFolderPath(0, szPath, CSIDL_DESKTOPDIRECTORY, 0);
	wstring ws = szPath;
	auto path = ws2s(szPath);
	regex reg("(.+\\w).*");
	smatch mc;
	if (regex_match(path, mc, reg)) {
		path = mc[1];
	}
	return path;
}

string int2String(int value) {
	stringstream ss;
	ss << value;
	return ss.str();
}

string allFileDirName() {
	auto now = time(NULL);
	auto tim = new tm();
	localtime_s(tim, &now);
	auto year = int2String(tim->tm_year + 1900);
	auto month = int2String(tim->tm_mon + 1);
	auto day = int2String(tim->tm_mday);
	delete tim;
	return year + "-" + month + "-" + day;
}

string first_letter(string& sentence){
	string::iterator it = sentence.begin();
	bool space_flag = true;

	while (it != sentence.end()) {
		if (isalpha(*it) && space_flag)
		{
			*it = toupper(*it);
			space_flag = false;
		}
		if (isspace(*it)) {
			space_flag = true;
		}
		it++;
	}
	return sentence;
}

int main() {
	auto path = getDesktopPath();
	auto files = getFiles(path);
	auto fileGroup = group(files);

	string dir = path + "/" + allFileDirName();
	createDir(dir);

	map<string, vector<FileItem>>::iterator it;
	for (it = fileGroup.begin(); it != fileGroup.end(); it++) {
		string ext = (it->first);
		first_letter(ext);
		createDir(dir + "/" + ext);
	}

	cin.get();
	return 0;
}

