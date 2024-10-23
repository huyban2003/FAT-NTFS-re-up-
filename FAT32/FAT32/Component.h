#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <codecvt>
#include <fcntl.h>
#include <io.h>
#include <locale>

using namespace std;

class Component {
protected:
	wstring name;
	int size;
	vector<unsigned int> clusters;
	vector<unsigned int> sectors;
public:
	Component();
	virtual ~Component() ;
	vector <unsigned int> GetSectors();
	vector <unsigned int> GetClusters();
	wstring getName();
	virtual int getSize() = 0;
	virtual string getClassName() = 0;
	int GetFirstCluster();
	void setName(wstring name);
	virtual void setSize(int size) = 0;
	void setClusters(vector<unsigned int> clusters);
	void setSectors(vector<unsigned int> sectors);
	void printInfo();
	virtual void addComponent(Component* item);
	virtual void printDirTree(int& idx, int count = 0);
	void addSectors(unsigned int sectors);
	void addCluster(unsigned int cluster);
	virtual wstring getExtension() = 0;
	virtual Component* findWithID(int id, int& idx) = 0;
};

class Folder :public Component {
	vector<Component*> components;
public:
	Folder();
	~Folder();

	string getClassName();
	int getSize();
	void setSize(int size);

	void addComponent(Component* item);
	void printDirTree(int& idx, int count = 0);
	wstring getExtension() { return L""; };
	Component* findWithID(int id, int& idx);
};


class File :public Component {
public:
	File();

	string getClassName();
	int getSize();
	void setSize(int size);
	wstring getExtension();
	Component* findWithID(int id, int& idx);
};