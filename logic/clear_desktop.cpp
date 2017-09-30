#include "stdafx.h"
#include <iostream>
#include <string>


#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <map>
#include <tuple>
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


void SaveSummaries(const vector<SummaryItem> &summaries) {
	SummaryResult sumaryResult;
	sumaryResult.save(summaries, targetPath);
}

char * GetFileInfoList() {
	auto files = fileHandler.GetAllDesktopFile();
	auto fileGroup = group(files);
	return Convert2Json::GetFileInfoList(fileGroup);
}

char * ClearItem(char * id) {
	string strId(id);
	tuple<bool,FileItem> result = fileHandler.FindFileItemByID(strId);
	bool found = get<0>(result);
	FileItem fileItem = get<1>(result);
	if (found) {
		auto summaries = fileHandler.Clear(fileItem);
		SaveSummaries(summaries);
		return Convert2Json::ResultJSON(found, "操作成功");
	} else {
		return Convert2Json::ResultJSON(found, "ID不存在");
	}
}


int main() {



	auto summaries = fileHandler.ClearAll();

	SaveSummaries(summaries);

	cout << "done" << endl;

	cin.get();
	return 0;
}




