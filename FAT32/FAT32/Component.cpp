#include "Component.h"
Component::Component() {
	name = L"";
	size = 0;
	clusters.resize(0);
}
Component::~Component() {}
wstring Component::getName() {
	return name;
}
void Component::setName(wstring name) {
	this->name = name;
}

void Component::setSectors(vector<unsigned int> sectors) {
	this->sectors.resize(0);
	for (int i = 0; i < sectors.size(); i++)
		this->sectors.push_back(sectors[i]);
}

void Component::setClusters(vector<unsigned int> clusters) {
	this->clusters.resize(0);
	for (int i = 0; i < clusters.size(); i++)
		this->clusters.push_back(clusters[i]);
}

vector <unsigned int> Component::GetSectors() {
	return sectors;
}

vector <unsigned int> Component::GetClusters() {
	return sectors;
}

void Component::addComponent(Component* item) {}
void Component::printDirTree(int& idx, int count) {}
void Component::addSectors(unsigned int sector) {
	sectors.push_back(sector);
}
void Component::addCluster(unsigned int cluster) {
	clusters.push_back(cluster);
}
int Component::GetFirstCluster() {
	return this->clusters[0];
}
void Component::printInfo() {
	int mode = _setmode(_fileno(stdout), _O_U16TEXT);
	wcout << L"\t \t \t Name: " << name << endl;
	mode = _setmode(_fileno(stdout), mode);
	cout << "Attribute: " << getClassName() << endl;
	cout << "Size: " << size << endl;
	cout << "Start cluster: " << clusters[0] << endl;
	cout << "Include clusters: ";
	for (int i = 0; i < clusters.size(); i++)
	{
		cout << clusters[i];
		if (i < clusters.size() - 1) cout << ", ";
	}
	if (sectors.size() == 0)
	{
		cout <<endl<< "Does not include any sector data!" ;
	}
	else
	{
		cout << endl << "Include sector: ";
		for (int i = 0; i < sectors.size(); i++)
		{
			cout << sectors[i];
			if (i < sectors.size() - 1) cout << ", ";
		}
	}
	
	cout << endl;
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
// idx la so thu tu cua folder/file
// count la bien phan cap cua folder/file trong cay thu muc
void Folder::printDirTree(int& idx, int count) {
	for (int i = 0; i < components.size(); i++) {
		for (int j = 0; j <= count; j++)
			cout << "___";
		cout << ++idx << ".";
		cout << "[" << components[i]->getClassName() << "] ";
		int mode = _setmode(_fileno(stdout), _O_U16TEXT);
		wcout << components[i]->getName() << endl;
		mode = _setmode(_fileno(stdout), mode);
		components[i]->printDirTree(idx, ++count);
		--count;
	}
}

// id la so thu tu duoc nhap vao
// idx la bien dem tim so thu tu cua folder/file
Component* Folder::findWithID(int id, int& idx) {
	if (id == idx)
		return this;
	for (int i = 0; i < components.size(); i++) {
		Component* res = components[i]->findWithID(id, ++idx);
		if (res != NULL)
			return res;
	}
	return NULL;
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

// id la so thu tu duoc nhap vao
// idx la bien dem tim so thu tu cua folder/file
Component* File::findWithID(int id, int& idx) {
	if (id == idx)
		return this;
	return NULL;
}