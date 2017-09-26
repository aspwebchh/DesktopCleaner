#pragma once

#include "common.h"
#include <vector>


using namespace std;

class FileHandler {
private:
	string targetPath;
	string desktopPath;
	vector<SummaryItem> summaries;
	string today;

	void addSummary(const FileItem &fileItem, const string &newPath) {
		SummaryItem summaryItem;
		summaryItem.fileItem = fileItem;
		summaryItem.time = current();
		summaryItem.newPath = newPath;
		summaries.push_back(summaryItem);
	}

	bool HandleFile(const FileItem &fileItem, const string &newDirPath) {
		if (fileItem.fileType == dir) {
			return false;
		}
		auto newFilePath = newDirPath + "\\" + fileItem.fileName;
		auto success = copyFile(fileItem.path, newFilePath);
		if (success) {
			this->addSummary(fileItem, newFilePath);
		}
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
		this->today = allFileDirName();
	}
	const vector<SummaryItem>& exec() {
		auto files = getFiles(this->desktopPath);
		auto path = this->targetPath + "\\" + today;
		if (this->createDirectory(path)) {
			this->MoveFiles(files, path);
		}
		return this->summaries;
	}
};