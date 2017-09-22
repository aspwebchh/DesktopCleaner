#pragma once

#include <vector>
#include <string>

using namespace std;

enum FileType {
	dir = 0,
	file = 1
};

struct FileItem {
	string path;
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


