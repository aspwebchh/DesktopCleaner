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


void SummaryResult::save(vector<SummaryItem> &summaries, string root) {
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement * rootEle = new TiXmlElement("Files");
	for (int i = 0; i < summaries.size(); i++) {
		auto summaryItem = summaries[i];

		TiXmlElement * fileItemEle = new TiXmlElement("FileItem");

		TiXmlElement * pathEle = xmlNode("Path", summaryItem.newPath.c_str());
		TiXmlElement * fileNameEle = xmlNode("FileName", summaryItem.fileItem.fileName.c_str());
		TiXmlElement * fileExtEle = xmlNode("FileExt", summaryItem.fileItem.ext.c_str()); 
		TiXmlElement * timeEle = xmlNode("Time", summaryItem.time.c_str()); 

		fileItemEle->SetAttribute("id", summaryItem.fileItem.id.c_str());
		fileItemEle->LinkEndChild(pathEle);
		fileItemEle->LinkEndChild(fileNameEle);
		fileItemEle->LinkEndChild(fileExtEle);
		fileItemEle->LinkEndChild(timeEle);

		rootEle->LinkEndChild(fileItemEle);
	}
	doc.LinkEndChild(decl);
	doc.LinkEndChild(rootEle);
	
	auto today = allFileDirName();
	doc.SaveFile((root + "\\"+ today +".xml").c_str());
}