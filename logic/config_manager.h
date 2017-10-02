#pragma once

#include <windows.h>
#include <direct.h>  
#include <iostream>
#include <regex>
#include "common.h"

using namespace std;



class ConfigManager {
private:
	static const string GetCurrWorkPath() {
		char buffer[MAX_PATH];
		_getcwd(buffer, MAX_PATH);
		return string(buffer);
	};
	static const string GetConfigFilePath() {
		const string configFileName = "config.txt";
		const string configFilePath = GetCurrWorkPath() + "\\" + configFileName;
		return configFilePath;
	}
	static bool CreateDir(string dir) {
		cout << dir << endl;
		auto status = createDir(dir);
		if (status != fail ) {
			return true;
		}
		regex reg(R"([\\\/][^\\\/]+[\\\/]?$)");
		auto parentDir = regex_replace(dir, reg, "");
		if (!ConfigManager::CreateDir(parentDir)) {
			return false;
		}
		auto parentDirStatus = createDir(dir);
		if (parentDirStatus != fail) {
			return true;
		} else {
			return false;
		}
	}
public:
	static void SetTargetPath( const string targetPath ) {
		auto configFilePath = GetConfigFilePath();
		saveFile(targetPath, configFilePath);


		string dir = "C:\\Users\\Administrator\\Desktop\\新建文件夹\\1111\\222\\3333\\222\\3333\\444";
		bool status = ConfigManager::CreateDir(dir);
		cout << status << endl;
	}

	static string GetTargetPath() {

	}
};