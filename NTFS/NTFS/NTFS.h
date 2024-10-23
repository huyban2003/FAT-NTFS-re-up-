#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <iomanip>
#include <math.h>
#include "Component.h"

using namespace std;

class NTFS
{
private:
	LPCWSTR  drive;
	BYTE* sector;
	unsigned int bytesPerSector;  // so byte tren sector
	unsigned int sectorsPerCluster; // so sector tren cluster
	unsigned int sectorsPerTrack; // so sector tren track
	unsigned int totalSectors; // so sector cua o dia
	unsigned int MFTClusterNumber; // cluster bat dau cua MFT
	unsigned int MFTMirrrClusterNumber; // cluster bat dau cua MFT Mirr (MFT du phong)
	unsigned int clusterPerFileRecordSegment; // KichThuoc cua 1 MFT entry

	Component* rootDir; // root directory tree

public:
	NTFS();
	NTFS(LPCWSTR drive);
	~NTFS();

	void readSector(LPCWSTR drive, int readPoint, BYTE*& sector, int size);
	void readSectorEx(LPCWSTR drive, unsigned  int readPoint, BYTE*& sector, int size);
	string decToHex(int decNumber);
	string getByteToString(BYTE* sector, int offset, int nByte);
	unsigned int hexToDec(string hexNumber);
	void printDataFromBPB();
	void printSector(BYTE*& sector);
	unsigned int getValueOfOffset(BYTE* sector, int offset, int nByte);
	unsigned int calcMFTEntry(unsigned int decNumber);

	void Read_MFT(BYTE* sector);
	string getString(BYTE* DATA, int offset, int num);
	wstring to_wstring(const string& stringToConvert);
	string getBytesForChar(BYTE* DATA, int offset, int number, int nBytePerChar);
	wstring readStringFromBytes(string input);
	void readFileContent(Component* comp, BYTE* MFT, int Start_Att_DATA);
	Component* findComponent(vector<Component*> comps, int ID);
	void linkComponent(vector<Component*> comps);
	Component* getRoot();
};