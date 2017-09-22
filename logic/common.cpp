#include "stdafx.h"
#include <string>
#include <fstream>
#include <sstream>
#include <io.h>
#include <vector>
#include <iostream>
#include <regex>
#include "common.h"

#pragma   comment(lib, "shell32.lib")

using namespace std;

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

string getExt( string path) {
	regex reg(".+\\.([^\\.]+$)");
	smatch match;
	if (regex_match(path,match,reg)) {
		return match[1];
	}
	return "";
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
					files.push_back(fileItem);
					if (returnAll) {
						auto result = getFiles(p.assign(path).append("\\").append(fileinfo.name), returnAll);
						for (int i = 0; i < result.size(); i++) {
							files.push_back(result[i]);
						}
					}
				}
			}
			else {
				auto pathVal = p.assign(path).append("\\").append(fileinfo.name);
				auto ext = getExt(pathVal);
				FileItem fileItem;
				fileItem.path = pathVal;
				fileItem.fileType = file;
				fileItem.ext = ext;
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
	if (!outfile)
	{
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