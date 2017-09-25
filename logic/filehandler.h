#pragma once

#include "common.h"


using namespace std;

class FileHandler {
private:
	string targetPath;
public:
	FileHandler( string targetPath ) {
		this->targetPath = targetPath;
	}

	void handleFile(string path) {
		cout << allFileDirName() << endl;
		cout << "123" << endl;
	}

	void handleDir() {

	}
};