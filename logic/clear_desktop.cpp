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
#include "export.h"

#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"

#include "md5.h"

#include "filehandler.h"



using namespace std;
using namespace rapidjson;

void add2Group(string key, FileItem fileItem, map<string, vector<FileItem>> &dataSource) {
	map<string, vector<FileItem>>::iterator found = dataSource.find(key);
	if (found != dataSource.end()) {
		found->second.push_back(fileItem);
	} else {
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
	regex reg("(.+desktop).*", regex::icase);
	smatch mc;
	if (regex_match(path, mc, reg)) {
		path = mc[1];
	}
	return path;
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


void appendSummaryItem(vector<SummaryItem> &list, SummaryItem item) {
	list.push_back(item);
}

void appendSummaryItem(vector<SummaryItem> &list1, vector<SummaryItem> list2) {
	for (int i = 0; i < list2.size(); i++) {
		list1.push_back(list2[i]);
	}
}

vector<SummaryItem> moveFiles(vector<FileItem> files, string dirPath) {
	vector<SummaryItem> summaries;
	for (int i = 0; i < files.size(); i++) {
		auto fileItem = files[i];
		if (fileItem.fileType == dir) {
			vector<FileItem> childFiles = getFiles(fileItem.path);
			auto newPath = dirPath + "\\" + fileItem.fileName;
			createDir(newPath);
			auto result = moveFiles(childFiles, newPath);
			appendSummaryItem(summaries, result);
		} else {
			auto newPath = dirPath + "\\" + fileItem.fileName;
			SummaryItem summaryItem;
			summaryItem.fileItem = fileItem;
			summaryItem.newPath = newPath;
			summaryItem.time = current();
			appendSummaryItem(summaries, summaryItem);
			//copyFile(fileItem.path, newPath);
		}
	}
	return summaries;
}

Value createValue(string val, Document & doc) {
	Value item(kObjectType);
	Value value;
	value.SetString( val.c_str(), doc.GetAllocator());
	return value;
}

Value createValue(int val, Document & doc) {
	Value item(kObjectType);
	Value value;
	value.SetInt(val);
	return value;
}

void files2JSON(vector<FileItem>& files, Document& doc, Value& items) {
	for (auto fileItem : files) {
		Value item(kObjectType);
		item.AddMember("FileName", createValue(fileItem.fileName,doc), doc.GetAllocator());
		item.AddMember("FileExt", createValue(fileItem.ext, doc), doc.GetAllocator());
		item.AddMember("FilePath", createValue(fileItem.path, doc), doc.GetAllocator());
		item.AddMember("FileType", createValue(fileItem.fileType, doc), doc.GetAllocator());
		items.PushBack(item, doc.GetAllocator());
	}
}

char * GetFileInfoList() {
	auto path = getDesktopPath() + "\\test_desktop";
	auto files = getFiles(path);
	auto fileGroup = group(files);
	Document doc;
	doc.SetObject();
	map<string, vector<FileItem>>::iterator it;
	for (it = fileGroup.begin(); it != fileGroup.end(); it++) {
		auto ext = it->first;
		auto files = it->second;
		Value out(kArrayType);
		files2JSON(files, doc, out);
		Value key = createValue(ext,doc);
		doc.AddMember(key,out, doc.GetAllocator());
	}
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	string jsonString = buffer.GetString();
	const char *charPtr = jsonString.c_str();
	char *result = new char[strlen(charPtr)];
	strcpy(result, charPtr);
	return result;
}

void moveAll() {
	auto path = getDesktopPath() + "\\test_desktop";
	auto files = getFiles(path);
	auto fileGroup = group(files);
	string dir = path + "\\" + allFileDirName();
	createDir(dir);

	vector<SummaryItem> summaries;

	map<string, vector<FileItem>>::iterator it;
	for (it = fileGroup.begin(); it != fileGroup.end(); it++) {
		string ext = (it->first);
		if (ext == "lnk") {
			continue;
		}
		auto dirPath = dir + "\\" + ext;
		createDir(dirPath);
		auto files = it->second;
		auto result = moveFiles(files, dirPath);
		appendSummaryItem(summaries, result);
	}

	auto summaryResult = new SummaryResult();
	summaryResult->save(summaries, dir);
	delete summaryResult;
	cout << "done" << endl;
}


int main() {
	const auto deskTopPath = getDesktopPath() + "\\test_desktop";
	const string targetPath = getDesktopPath() + "\\temp_target";
	
	auto fileHandler = new FileHandler(deskTopPath, targetPath);
	fileHandler->exec();
	delete fileHandler;
	cout << "done" << endl;

	cin.get();
	return 0;
}




