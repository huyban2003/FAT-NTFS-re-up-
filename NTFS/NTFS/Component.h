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
	int ID;
	int parentID;
	wstring name;
	int size;
	

public:
	Component();
	virtual ~Component() ;

	void setName(wstring name);
	wstring getName();
	virtual wstring getExtension() = 0;
	virtual void setSize(int size) = 0;
	virtual int getSize() = 0;
	virtual string getClassName() = 0;
	void setID(int ID);
	int getID();
	void setParentID(int parentID);
	int getParentID();
	virtual void setContent(wstring content) = 0;

	virtual void addComponent(Component* item);
	virtual void addReferenceID(int ID) = 0;
	virtual vector<int> getReferenceIDs() = 0;

	virtual void printDirTree(int count = 0);
	virtual Component* findWithID(int id) = 0;
	virtual void printInfo();
};

class Folder :public Component {
	vector<int> referenceIDs;
	vector<Component*> components;
public:
	Folder();
	~Folder();

	string getClassName();
	int getSize();
	void setSize(int size);
	wstring getExtension();

	void addComponent(Component* item);
	void addReferenceID(int ID);
	vector<int> getReferenceIDs();

	void printDirTree(int count = 0);
	Component* findWithID(int id);
	void printInfo();
	void setContent(wstring content) {};
};


class File :public Component {
	wstring content;
public:
	File();

	string getClassName();
	int getSize();
	void setSize(int size);
	wstring getExtension();

	void addReferenceID(int ID) {};
	vector<int> getReferenceIDs();
	Component* findWithID(int id);
	void printInfo();
	void setContent(wstring content);
};