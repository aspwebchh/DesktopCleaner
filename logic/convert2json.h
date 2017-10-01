#pragma once

#include <map>
#include <vector>
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"


using namespace rapidjson;
using namespace std;

class Convert2Json {
private:
	static Value createValue(string val, Document & doc) {
		Value item(kObjectType);
		Value value;
		value.SetString(val.c_str(), doc.GetAllocator());
		return value;
	}
	static Value createValue(int val, Document & doc) {
		Value item(kObjectType);
		Value value;
		value.SetInt(val);
		return value;
	}
	static Value createValue(bool val, Document & doc) {
		Value item(kObjectType);
		Value value;
		value.SetBool(val);
		return value;
	}

	static void files2JSON(vector<FileItem>& files, Document& doc, Value& items) {
		for (auto fileItem : files) {
			Value item(kObjectType);
			item.AddMember("ID", Convert2Json::createValue(fileItem.id, doc), doc.GetAllocator());
			item.AddMember("FileName", Convert2Json::createValue(fileItem.fileName, doc), doc.GetAllocator());
			item.AddMember("FileExt", Convert2Json::createValue(fileItem.ext, doc), doc.GetAllocator());
			item.AddMember("FilePath", Convert2Json::createValue(fileItem.path, doc), doc.GetAllocator());
			item.AddMember("FileType", Convert2Json::createValue(fileItem.fileType, doc), doc.GetAllocator());
			item.AddMember("FileSize", Convert2Json::createValue( int2String( fileItem.fileSize ), doc), doc.GetAllocator());
			items.PushBack(item, doc.GetAllocator());
		}
	}

	static char * Doc2String(Document &doc) {
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		string jsonString = buffer.GetString();
		const char *charPtr = jsonString.c_str();
		char *result = new char[strlen(charPtr)];
		strcpy(result, charPtr);
		return result;
	}
public:
	static char * ResultJSON(const bool success, const string &msg) {
		Document doc;
		doc.SetObject();
		Value value = createValue(success, doc);
		doc.AddMember("Success", value, doc.GetAllocator());

		Value value1 = createValue(msg, doc);
		doc.AddMember("Message", value1, doc.GetAllocator());
		return Doc2String(doc);
	}

	static char * GetFileInfoList(map<string, vector<FileItem>> fileGroup) {
		Document doc;
		doc.SetObject();
		map<string, vector<FileItem>>::iterator it;
		for (it = fileGroup.begin(); it != fileGroup.end(); it++) {
			auto ext = it->first;
			auto files = it->second;
			Value out(kArrayType);
			Convert2Json::files2JSON(files, doc, out);
			Value key = Convert2Json::createValue(ext, doc);
			doc.AddMember(key, out, doc.GetAllocator());
		}
		return Doc2String(doc);
	}

};
