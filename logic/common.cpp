#include "stdafx.h"
#include <string>
#include <fstream>
#include <sstream>
#include <io.h>
#include <vector>
#include <iostream>
#include <regex>
#include <io.h>
#include <direct.h>
#include <Windows.h>
#include <shlobj.h>
#include "common.h"
#include "md5.h"


#pragma comment(lib, "shell32.lib")

using namespace std;


void str2Lower(string &s) {
	transform(s.begin(), s.end(), s.begin(), ::tolower);
}

const std::wstring s2ws(const std::string& s)
{
	std::locale old_loc =
		std::locale::global(std::locale(""));
	const char* src_str = s.c_str();
	const size_t buffer_size = std::mbstowcs(NULL, src_str, 0);
	wchar_t* dst_wstr = new wchar_t[buffer_size];
	wmemset(dst_wstr, 0, buffer_size);
	std::mbstowcs(dst_wstr, src_str, buffer_size);
	std::wstring result = dst_wstr;
	delete[]dst_wstr;
	std::locale::global(old_loc);
	return result;
}
const std::string ws2s(const std::wstring& ws)
{
	std::locale old_loc =
		std::locale::global(std::locale(""));
	const wchar_t* src_wstr = ws.c_str();
	size_t buffer_size = std::wcstombs(NULL, src_wstr, 0);;
	char* dst_str = new char[buffer_size];
	memset(dst_str, 0, buffer_size);
	std::wcstombs(dst_str, src_wstr, buffer_size);
	std::string result = dst_str;
	delete[]dst_str;
	std::locale::global(old_loc);
	return result;
}

string getExt(string path) {
	regex reg(".+\\.([^\\.]+$)");
	smatch match;
	string ext;
	if (regex_match(path, match, reg)) {
		ext = match[1];
		str2Lower(ext);
	} else {
		ext = "Without-Extension";
	}
	return ext;
}

vector<FileItem> getFiles(string path, bool returnAll) {
	long  hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	vector<FileItem> files;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if ((fileinfo.attrib &  _A_SUBDIR)) {
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					auto pathVal = p.assign(path).append("\\").append(fileinfo.name);
					FileItem fileItem;
					fileItem.path = pathVal;
					fileItem.fileType = dir;
					fileItem.fileName = fileinfo.name;
					fileItem.ext = "Folder";
					fileItem.id = MD5(pathVal).toStr();
					files.push_back(fileItem);
					if (returnAll) {
						auto result = getFiles(p.assign(path).append("\\").append(fileinfo.name), returnAll);
						for (int i = 0; i < result.size(); i++) {
							files.push_back(result[i]);
						}
					}
				}
			} else {
				auto pathVal = p.assign(path).append("\\").append(fileinfo.name);
				auto ext = getExt(pathVal);
				if (ext == "lnk") {
					continue;
				}
				FileItem fileItem;
				fileItem.path = pathVal;
				fileItem.fileType = file;
				fileItem.fileName = fileinfo.name;
				fileItem.ext = ext;
				fileItem.id = MD5(pathVal).toStr();
				files.push_back(fileItem);
			}

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return files;
}

vector<FileItem> getFiles(string path) {
	return getFiles(path, false);
}


void saveFile(string content, string path) {
	ofstream outfile;
	outfile.open(path);
	if (!outfile) {
		cout << "txt文件打开失败!" << endl;
		exit(0);
	}
	outfile << content;
	outfile.close();
}

void saveFile(string content) {
	auto path = "C:\\Users\\宏鸿\\Desktop\\test.txt";
	saveFile(content, path);
}


MKDirStatus createDir(string dir) {
	if (access(dir.c_str(), 0) == -1) {
		int flag = mkdir(dir.c_str());
		if (flag == 0) {
			return success;
			cout << "make successfully" << endl;
		} else {
			return fail;
		}
	} else {
		return exists;
	}
}

bool copyFile(string SourceFile, string NewFile) {
	//cout << SourceFile << endl;
	//cout << NewFile << endl;
	//cout << "-----" << endl;
	ifstream in;
	ofstream out;
	in.open(SourceFile, ios::binary);
	if (in.fail()) {
		cout << "Error 1: Fail to open the source file." << SourceFile << endl;
		in.close();
		out.close();
		return false;
	}
	out.open(NewFile, ios::binary);
	if (out.fail()) {
		cout << "Error 2: Fail to create the new file." << NewFile << endl;
		out.close();
		in.close();
		return false;
	} else {
		out << in.rdbuf();
		out.close();
		in.close();
		return true;
	}
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


string current() {
	auto now = time(NULL);
	auto tim = new tm();
	localtime_s(tim, &now);
	auto year = int2String(tim->tm_year + 1900);
	auto month = int2String(tim->tm_mon + 1);
	auto day = int2String(tim->tm_mday);
	auto hour = int2String(tim->tm_hour);
	auto min = int2String(tim->tm_min);
	auto sec = int2String(tim->tm_sec);
	delete tim;
	return year + "-" + month + "-" + day + " " + hour + ":" + min + ":" + sec;
}


string first_letter(string& sentence) {
	string::iterator it = sentence.begin();
	bool space_flag = true;

	while (it != sentence.end()) {
		if (isalpha(*it) && space_flag) {
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

string getDesktopPath() {
	TCHAR szPath[MAX_PATH];
	SHGetSpecialFolderPath(0, szPath, CSIDL_DESKTOPDIRECTORY, 0);
	wstring ws = szPath;
	auto path = ws2s(szPath);
	regex reg("(.+desktop).*", regex::icase);
	smatch mc;
	if (regex_match(path, mc, reg)) {
		path = mc[1];
	}
	return path;
}
