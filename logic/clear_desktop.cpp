#include "stdafx.h"
#include <iostream>
#include <string>


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
#include "convert2json.h"

using namespace std;
using namespace rapidjson;

static const auto deskTopPath = getDesktopPath() + "\\test_desktop";
static const string targetPath = getDesktopPath() + "\\temp_target";
static FileHandler fileHandler(deskTopPath, targetPath);

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

/*
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
*/

char * GetFileInfoList() {
	auto files = fileHandler.GetAllDesktopFile();
	auto fileGroup = group(files);
	return Convert2Json::GetFileInfoList(fileGroup);
}

/*void moveAll() {
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
}*/

int main() {
	auto summaries = fileHandler.ClearAll();

	SummaryResult sumaryResult;
	sumaryResult.save(summaries, targetPath);

	cout << "done" << endl;

	cin.get();
	return 0;
}




