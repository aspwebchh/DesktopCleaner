#pragma once


#include <vector>
#include <string>
#include "tinyxml.h"


using namespace std;

enum FileType {
	dir = 0,
	file = 1
};

enum MKDirStatus {
	success = 0,
	exists = 1,
	fail = 2
};

struct FileItem {
	string path;
	string fileName;
	FileType fileType;
	string ext;
};


const std::wstring s2ws(const std::string& s);

const std::string ws2s(const std::wstring& ws);


vector<FileItem> getFiles(string path);

vector<FileItem> getFiles(string path, bool returnAll);

template <class T>

int getArrayLen(T &array) {
	return sizeof(array) / sizeof(array[0]);
}

void saveFile(string content, string path);
void saveFile(string content);

MKDirStatus createDir(string dir);


void str2Lower(string &s);

int copyFile(string SourceFile, string NewFile);


struct SummaryItem {
	FileItem fileItem;
	string newPath;
	string time;
};

class SummaryResult {
private :
	TiXmlElement * xmlNode(string name, string value);
public:
	void save(vector<SummaryItem> &summaries , string root);
};

