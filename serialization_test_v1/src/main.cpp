
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <fstream>
#include <sys/time.h>
#include "Image.hpp"
using namespace std;
using namespace boost::archive;

int parseLine(char* line) {
	int i = strlen(line);
	while (*line < '0' || *line > '9')
		line++;
	line[i - 3] = '\0';
	i = atoi(line);
	return i;
}

int getCurrentUsedVM() { //Note: this value is in KB!
	FILE* file = fopen("/proc/self/status", "r");
	int result = -1;
	char line[128];

	while (fgets(line, 128, file) != NULL) {
		if (strncmp(line, "VmRSS:", 6) == 0) {
			result = parseLine(line);
			break;
		}
	}
	fclose(file);
	return result;
}

long long getTotalVM() {
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long totalVirtualMem = memInfo.totalram;
	//Add other values in next statement to avoid int overflow on right hand side...
	totalVirtualMem += memInfo.totalswap;
	totalVirtualMem *= memInfo.mem_unit;
	return totalVirtualMem;
}

unsigned long long getCurrentTimeMs() {
	unsigned long long ret = 0;
	struct timeval t;
	gettimeofday(&t, NULL);
	ret = (((unsigned long long) t.tv_sec) * 1000)
			+ (((unsigned long long) t.tv_usec) / 1000);
	return ret;
}

int main()
{
	std::vector<char>* vImageDataTemp = NULL;
	try {
		std::ifstream imageFile("./sample1.jpg", std::ios::binary | std::ios::ate);
		std::ifstream::pos_type pos = imageFile.tellg();
		vImageDataTemp = new std::vector<char>(pos);
		imageFile.seekg(0, std::ios::beg);
		imageFile.read(&(*vImageDataTemp)[0], pos);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	vector<unsigned char>* vImageData = new std::vector<unsigned char>(vImageDataTemp->begin(), vImageDataTemp->end());
	if(vImageDataTemp) delete vImageDataTemp;

	ostringstream* pOss;
	text_oarchive* pOa;
	istringstream* pIss;
	text_iarchive* pIa;
	int iCount = 10;
	unsigned long long lStartTime = getCurrentTimeMs();
	for(int i = 0; i < iCount; ++i)
	{
		cout << "Image " << i+1 << endl;
		opencctv::Image* pImage = new opencctv::Image;
		pImage->setWidth(2288);
		pImage->setHeight(1712);
		pImage->setTimestamp("Custom Time 0");
		pImage->setImageData(*vImageData);
		cout << "Data of original image: " << (int)pImage->getImageData()[1] << " " << (int)pImage->getImageData()[2] << " " << (int)pImage->getImageData()[3] << " ... "
				<< (int)pImage->getImageData()[pImage->getImageData().size()-1] << " " << (int)pImage->getImageData()[pImage->getImageData().size()-2] << " " << (int)pImage->getImageData()[pImage->getImageData().size()-3] << endl;

		pOss = new ostringstream;
		pOa = new text_oarchive(*pOss);
		(*pOa) << (*pImage);
		if (pImage)
			delete pImage;

		string* pSSerializedImage = new string(pOss->str());
		pOss->clear();
		if (pOa)
			delete pOa;
		if (pOss)
			delete pOss;
		cout << "Size of serialized image (bytes): " << pSSerializedImage->length() << endl;

		opencctv::Image* pImageParsed = new opencctv::Image;
		pIss = new istringstream(*pSSerializedImage);
		pIa = new text_iarchive(*pIss);
		(*pIa) >> (*pImageParsed);
		if(pIa) delete pIa;
		if(pIss) delete pIss;
		if(pSSerializedImage) delete pSSerializedImage;
		// cout << "Total VM (bytes): " << getTotalVM() << endl;
		cout << "VM used (bytes): " << getCurrentUsedVM() * 1024 << endl;
		// cout << "Timestamp of de-serialized image: " << pImageParsed->getTimestamp() << endl;
		cout << "Data of de-serialized image: " << (int)pImageParsed->getImageData()[1] << " " << (int)pImageParsed->getImageData()[2] << " " << (int)pImageParsed->getImageData()[3] << " ... "
				<< (int)pImageParsed->getImageData()[pImageParsed->getImageData().size()-1] << " " << (int)pImageParsed->getImageData()[pImageParsed->getImageData().size()-2] << " " << (int)pImageParsed->getImageData()[pImageParsed->getImageData().size()-3] << endl;
		if(pImageParsed) delete pImageParsed;
		cout << endl;
	}
	if(vImageData) delete vImageData;

	unsigned long long lTimePerImage = (getCurrentTimeMs() - lStartTime) / (unsigned long long)iCount;
	cout << "Time per image (ms): " << lTimePerImage << endl;
	return 0;
}
