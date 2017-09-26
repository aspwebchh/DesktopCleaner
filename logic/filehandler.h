#pragma once

#include "common.h"


using namespace std;

class FileHandler {
private:
	string targetPath;
	string desktopPath;
	bool HandleFile(const FileItem &fileItem, const string &newDirPath) {
		if (fileItem.fileType == dir) {
			return false;
		}
		auto newFilePath = newDirPath + "\\" + fileItem.fileName;
		auto success = copyFile(fileItem.path, newFilePath);
		return success;
	}

	void MoveFiles(const vector<FileItem> &files, const string &path) {
		for (auto &file : files) {
			if (file.fileType == dir) {
				auto childFiles = getFiles(file.path);
				auto dirPath = path + "\\" + file.fileName;
				if (this->createDirectory(dirPath)) {
					this->MoveFiles(childFiles, dirPath);
				}
			} else {
				this->HandleFile(file, path);
			}
		}
	}
	bool createDirectory( const string &dir ) {
		auto createdResult = createDir(dir);
		if (createdResult == success || createdResult == exists) {
			return true;
		} else {
			return false;
		}
	}
public:
	FileHandler(const string &desktopPath,  const string &targetPath) {
		this->desktopPath = desktopPath;
		this->targetPath = targetPath;
	}
	void exec() {
		auto files = getFiles(this->desktopPath);
		auto path = this->targetPath + "\\" + allFileDirName();
		if (this->createDirectory(path)) {
			this->MoveFiles(files, path);
		}
	}
};