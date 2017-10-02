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
	}

	static tuple<bool, string> GetTargetPath() {
		auto configFilePath = GetConfigFilePath();
		auto targetPath = fileContent(configFilePath);
		if (targetPath.size() == 0) {
			return{ false, "" };
		}
		bool status = ConfigManager::CreateDir(targetPath);
		if (!status) {
			return{ false, "" };
		}
		return{ true,targetPath };
	}
};