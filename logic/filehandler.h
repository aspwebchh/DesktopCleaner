#pragma once

#include "common.h"
#include <vector>
#include <tuple>

using namespace std;


class FileHandler {
private:
	string targetPath;
	string todayTargetPath;
	string desktopPath;
	vector<SummaryItem> summaries;
	vector<FileItem> allDesktopFiles;
	bool desktopFilesLoaded = false;

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
		this->todayTargetPath = targetPath + "\\" + allFileDirName();
	}
	const vector<SummaryItem>& Clear(FileItem file) {
		vector<FileItem> files;
		files.push_back(file);
	
		return this->Clear(files);
	}
	const vector<SummaryItem>& Clear(vector<FileItem> &files) {
		this->summaries.clear();
		if (this->createDirectory(this->todayTargetPath)) {
			this->MoveFiles(files, this->todayTargetPath);
		}
		return this->summaries;
	}

	const vector<SummaryItem>& ClearAll(){
		auto files = this->GetAllDesktopFile();
		return this->Clear(files);
	}

	const vector<FileItem>& GetAllDesktopFile() {
		if (this->desktopFilesLoaded) {
			return this->allDesktopFiles;
		} else {
			this->allDesktopFiles = getFiles(this->desktopPath);
			this->desktopFilesLoaded = true;
			return this->allDesktopFiles;
		}
	}
	
	const tuple<bool, FileItem> FindFileItemByID(string &id) {
		auto allFiles = this->GetAllDesktopFile();
		for (auto &file : allFiles) {
			if (id == file.id) {
				return{ true,file };
			}
		}
		return{ false,FileItem{} };
	}
	
};