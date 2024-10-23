#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <iomanip>
#include <math.h>
#include "Component.h"


class FAT32
{
private:
	LPCWSTR  drive;
	unsigned int bytesPerSector; 
	unsigned int sectorsPerCluster; 
	unsigned int reservedSectors; 
	unsigned int numbersOfFATs; 
	unsigned int totalSectors; 
	unsigned int sectorsPerFAT;
	unsigned int rootCluster; 
	unsigned int firstSectorOfFAT1; 
	unsigned int firstSectorOfRDET; 
	unsigned int firstSectorOfData; 
	BYTE* sector; 
	Component* rootDir; // root directory

public:
	// constructor - destructor 
	FAT32();
	FAT32(LPCWSTR drive);
	~FAT32();

	
	string decToHex(int decNumber);
	string getByteToString(BYTE* sector, int offset, int nByte);
	unsigned int GetFirstSectorRDET();
	unsigned int hexToDec(string hexNumber);
	unsigned int getValueOfOffset(BYTE* sector, int offset, int nByte);
	string getBytesForChar(BYTE* DATA, int offset, int number, int nBytePerChar);
	wstring readStringFromBytes(string input);
	void removeLastSpaces(wstring& name);
	Component* getRoot();
	string GetString(BYTE* DATA, int offset, int num);
	friend wstring to_wstring(const string& stringToConvert);

	void readSector(LPCWSTR  drive, int readPoint, BYTE*& sector, int size);
	void readRDET(BYTE* RDET, int sectorIdx, Component* root);
	void readFAT1(BYTE*& FAT1, int sectorNth);
	void ReadFileData(Component* tmp);
	void printDataFromBootSector();
	void printRDET();
};