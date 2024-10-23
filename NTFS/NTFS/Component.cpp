#include "Component.h"
Component::Component() {
	name = L"";
	size = 0;
}
Component::~Component() {}
wstring Component::getName() {
	return name;
}
void Component::setID(int ID) {
	this->ID = ID;
}
int Component::getID() {
	return this->ID;
}
void Component::setParentID(int parentID) {
	this->parentID = parentID;
}
int Component::getParentID() {
	return this->parentID;
}
void Component::setName(wstring name) {
	this->name = name;
}
void Component::addComponent(Component* item) {}
void Component::printDirTree(int count) {}
// Xuat cac thong tin co ban cua tap tin
void Component::printInfo() {
	cout << "ID: " << ID << endl;
	int mode = _setmode(_fileno(stdout), _O_U16TEXT);
	wcout << L"Name: " << name << endl;
	mode = _setmode(_fileno(stdout), mode);
	cout << "Type: " << getClassName() << endl;
	cout << "Size: " << size << endl;
}


Folder::Folder() {}
Folder::~Folder() {
	for (int i = 0; i < components.size(); i++)
		delete components[i];
	components.clear();
}
int Folder::getSize() {
	return size;
}
void Folder::addReferenceID(int ID) {
	referenceIDs.push_back(ID);
}
void Folder::addComponent(Component* item) {
	components.push_back(item);
}
string Folder::getClassName() {
	return "Folder";
}
void Folder::setSize(int size) {
	this->size = size;
}
// in cay thu muc
// count la bien phan cap cua folder/file trong cay thu muc
void Folder::printDirTree(int count) {
	for (int i = 0; i < components.size(); i++) {
		for (int j = 0; j <= count; j++)
			cout << "___";
		cout << "[" << components[i]->getClassName() << "] ";
		int mode = _setmode(_fileno(stdout), _O_U16TEXT);
		wcout << components[i]->getName() << " - " << components[i]->getID() << endl;
		mode = _setmode(_fileno(stdout), mode);
		components[i]->printDirTree(++count);
		--count;
	}
}
vector<int> Folder::getReferenceIDs() {
	return referenceIDs;
}
wstring Folder::getExtension() {
	return L""; 
}
Component* Folder::findWithID(int id) {
	if (id == this->ID)
		return this;
	for (int i = 0; i < components.size(); i++) {
		Component* res = components[i]->findWithID(id);
		if (res != NULL)
			return res;
	}
	return NULL;
}

// Xuat cac thong tin co ban va cay thu muc con
void Folder::printInfo() {
	Component::printInfo();

	cout << "Child directory tree:" << endl;
	cout << "[" << getClassName() << "] ";
	int mode = _setmode(_fileno(stdout), _O_U16TEXT);
	wcout << name << " - " << ID << endl;
	mode = _setmode(_fileno(stdout), mode);
	this->printDirTree();
}


File::File() {}
int File::getSize() {
	return size;
}
string File::getClassName() {
	return "File";
}
void File::setSize(int size) {
	this->size = size;
}
wstring File::getExtension() {
	size_t idx = name.find_last_of(L".");
	return name.substr(idx + 1);
}
vector<int> File::getReferenceIDs() {
	vector<int> res;
	res.resize(0);
	return res;
}
Component* File::findWithID(int id) {
	if (id == this->ID)
		return this;
	return NULL;
}
void File::setContent(wstring content) {
	this->content = content;
}
// Xuat cac thong tin co ban va noi dung file
void File::printInfo() {
	Component::printInfo();
	wstring exts = getExtension(); // lay duoi mo rong cua tap tin
	if (exts == L"txt" || exts == L"TXT") {
		if (size == 0) {
			cout << "File does not have content!" << endl;
			return;
		}
		cout << "Content:" << endl;
		int mode = _setmode(_fileno(stdout), _O_U16TEXT);
		wcout << content << endl;
		mode = _setmode(_fileno(stdout), mode);
	}
	else {
		int mode = _setmode(_fileno(stdout), _O_U16TEXT);
		wcout << L"Use another tool to read " << getExtension() << L" file" << endl;
		mode = _setmode(_fileno(stdout), mode);
	}
}