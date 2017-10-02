#pragma once


#include <vector>
#include <string>
#include <sstream>
#include <stdio.h>
#include <time.h>
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
	string id;
	string path;
	string fileName;
	FileType fileType;
	string ext;
	unsigned long fileSize;
};

struct SummaryItem {
	FileItem fileItem;
	string newPath;
	string time;
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

MKDirStatus createDir(string dir);


void str2Lower(string &s);

bool copyFile(string SourceFile, string NewFile);



class SummaryResult {
private :
	TiXmlElement * xmlNode(string name, string value);
	void FullElements(TiXmlElement *root, const vector<SummaryItem> &summaries);
	bool CheckNode(TiXmlElement *node, const string &id);
	bool ExistsNode(TiXmlElement *root, const string &id);
public:
	void save(const vector<SummaryItem> &summaries , string root);
};


string int2String(int value);


string allFileDirName();


string current();

string first_letter(string& sentence);

string getDesktopPath();

unsigned long dirSize(const string &dir);

string fileContent(string path);