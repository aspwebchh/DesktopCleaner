#include "stdafx.h"

#include <vector>
#include <string>
#include <iostream>
#include "tinyxml.h"
#include "common.h"

using namespace std;


TiXmlElement * SummaryResult::xmlNode(string name, string value) {
	TiXmlElement * ele = new TiXmlElement(name.c_str());
	TiXmlText * val = new TiXmlText(value.c_str());
	ele->LinkEndChild(val);
	return ele;
}

bool SummaryResult::CheckNode(TiXmlElement *node, const string &id) {
	auto value = node->Attribute("id");
	if (strcmp(value, id.c_str()) == 0) {
		return true;
	} else {
		auto next = node->NextSiblingElement();
		if (next && next->Type() == TiXmlElement::TINYXML_ELEMENT) {
			return CheckNode(next, id);
		} else {
			return false;
		}
	}
}

bool SummaryResult::ExistsNode(TiXmlElement *root, const string &id) {
	auto firstChild = root->FirstChildElement();
	if (firstChild && firstChild->Type() == TiXmlElement::TINYXML_ELEMENT) {
		return CheckNode(firstChild, id);
	} else {
		return false;
	}
}

void SummaryResult::FullElements(TiXmlElement *root, const vector<SummaryItem> &summaries) {
	auto firstChild = root->FirstChildElement();

	for (int i = 0; i < summaries.size(); i++) {
		auto summaryItem = summaries[i];

		TiXmlElement * fileItemEle = new TiXmlElement("FileItem");

		if (ExistsNode(root, summaryItem.fileItem.id)) {
			continue;
		}

		TiXmlElement * pathEle = xmlNode("Path", summaryItem.newPath.c_str());
		TiXmlElement * fileNameEle = xmlNode("FileName", summaryItem.fileItem.fileName.c_str());
		TiXmlElement * fileExtEle = xmlNode("FileExt", summaryItem.fileItem.ext.c_str());
		TiXmlElement * timeEle = xmlNode("Time", summaryItem.time.c_str());

		fileItemEle->SetAttribute("id", summaryItem.fileItem.id.c_str());
		fileItemEle->LinkEndChild(pathEle);
		fileItemEle->LinkEndChild(fileNameEle);
		fileItemEle->LinkEndChild(fileExtEle);
		fileItemEle->LinkEndChild(timeEle);

		root->LinkEndChild(fileItemEle);
	}
}


void SummaryResult::save( vector<SummaryItem> &summaries, string root) {
	auto today = allFileDirName();
	auto filePath = (root + "\\" + today + ".xml");

	TiXmlDocument doc;
	bool found = doc.LoadFile(filePath.c_str());
	if (found) {
		TiXmlElement * rootEle = doc.RootElement();
		this->FullElements(rootEle, summaries);
		doc.SaveFile();
	} else {
		TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
		TiXmlElement * rootEle = new TiXmlElement("Files");
		this->FullElements(rootEle, summaries);
		doc.LinkEndChild(decl);
		doc.LinkEndChild(rootEle);
		doc.SaveFile(filePath.c_str());
	}
}