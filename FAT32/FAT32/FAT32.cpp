#include "FAT32.h"

// constructor
FAT32::FAT32()
{
	this->drive = L"\\\\.\\ F:";
	this->readSector(drive, 0, sector, 512);
	this->drive = drive;
	this->bytesPerSector = this->hexToDec(this->getByteToString(sector, 0x00B, 2));
	this->sectorsPerCluster = this->hexToDec(this->getByteToString(sector, 0x00D, 1));
	this->reservedSectors = this->hexToDec(this->getByteToString(sector, 0x00E, 2)); // SB
	this->numbersOfFATs = this->hexToDec(this->getByteToString(sector, 0x010, 1)); // NF
	this->totalSectors = this->hexToDec(this->getByteToString(sector, 0x020, 4));
	this->sectorsPerFAT = this->hexToDec(this->getByteToString(sector, 0x024, 4)); //SF
	this->rootCluster = this->hexToDec(this->getByteToString(sector, 0x02C, 4));
	this->firstSectorOfFAT1 = this->reservedSectors;
	this->firstSectorOfRDET = this->reservedSectors + this->numbersOfFATs * this->sectorsPerFAT;
	this->firstSectorOfData = this->reservedSectors + this->numbersOfFATs * this->sectorsPerFAT +
		(this->rootCluster - 2) * sectorsPerCluster;
	rootDir = new Folder;
}

FAT32::FAT32(LPCWSTR drive)
{
	this->sector = new BYTE[512];
	// read Bootsector to get data
	this->readSector(drive, 0, sector, 512);
	this->drive = drive;
	this->bytesPerSector = this->hexToDec(this->getByteToString(sector, 0x00B, 2));
	this->sectorsPerCluster = this->hexToDec(this->getByteToString(sector, 0x00D, 1));
	this->reservedSectors = this->hexToDec(this->getByteToString(sector, 0x00E, 2)); // SB
	this->numbersOfFATs = this->hexToDec(this->getByteToString(sector, 0x010, 1)); // NF
	this->totalSectors = this->hexToDec(this->getByteToString(sector, 0x020, 4));
	this->sectorsPerFAT = this->hexToDec(this->getByteToString(sector, 0x024, 4)); //SF
	this->rootCluster = this->hexToDec(this->getByteToString(sector, 0x02C, 4));
	this->firstSectorOfFAT1 = this->reservedSectors;
	this->firstSectorOfRDET = this->reservedSectors + this->numbersOfFATs * this->sectorsPerFAT;
	this->firstSectorOfData = this->reservedSectors + this->numbersOfFATs * this->sectorsPerFAT +
		(this->rootCluster - 2) * sectorsPerCluster;
	rootDir = new Folder;
}

// destructor
FAT32::~FAT32()
{
	delete sector;
	sector = NULL;
	delete rootDir;
	rootDir = NULL;
}

//---------------------------- cac ham ho tro ---------------------------------------------
string FAT32::decToHex(int decNumber)
{
	string res;
	string tmp;
	string smpl = "0123456789ABCDEF";
	if (decNumber < 16)
	{
		tmp.push_back(smpl[decNumber]);
		return tmp;
	}
	while (decNumber > 0)
	{
		int a = decNumber % 16;
		tmp.push_back(smpl[a]);
		decNumber = decNumber / 16;
	}
	for (int i = tmp.length() - 1; i >= 0; i--)
	{
		res.push_back(tmp[i]);
	}

	return res;
}

unsigned int FAT32::hexToDec(string hexNumber)
{
	string smpl = "0123456789ABCDEF";
	unsigned int res = 0;
	int count = 0;
	int length = hexNumber.length();
	for (int i = length - 1; i >= 0; i--)
	{
		int tmp = smpl.find_first_of(hexNumber[i]);
		res = res + tmp * pow(16, count);
		count++;
	}
	return res;
}

string FAT32::getByteToString(BYTE* sector, int offset, int nByte)
{
	string res;
	string tmpRes;
	// nhan byte o dang Little Edian va chuyen nguoc lai 
	for (int i = nByte - 1; i >= 0; i--)
	{
		int tmp = (int)sector[offset + i];
		tmpRes = decToHex(tmp);
		if (tmpRes.length() == 1)
		{
			tmpRes = "0" + tmpRes;
		}
		res = res + tmpRes;
	}
	return res;
}

unsigned int FAT32::GetFirstSectorRDET() {
	return this->firstSectorOfRDET;
}

unsigned int FAT32::getValueOfOffset(BYTE* sector, int offset, int nByte) {
	return hexToDec(getByteToString(sector, offset, nByte));
}

//Doc BYTE tu DATA thanh 1 string
string FAT32::GetString(BYTE* DATA, int offset, int num) {
	char* res = new char[num + 1];
	memcpy(res, DATA + offset, num);
	string s = "";
	for (int i = 0; i < num; i++) {
		if (res[i] != 0x00 && res[i] != 0xFF)
			s += res[i];
	}
	return s;
}

// get bytes in the format of 2 bytes per character
string FAT32::getBytesForChar(BYTE* DATA, int offset, int number, int nBytePerChar)
{
	string res;
	if (nBytePerChar == 2) // neu la utf16 - 1 ki tu chiem 2 bytes
		number /= 2; // lay so luong ki tu can doc
	// neu nBytePerChar = 1 -> so ki tu chinh la so byte can doc
	for (int j = 0; j < number; j++) {
		for (int i = nBytePerChar - 1; i >= 0; i--) {
			int tmp = (int)DATA[offset + i];
			if (nBytePerChar == 1) { // neu la utf8 -> them 00 truoc ket qua
				res += "00";
			}
			if (tmp < 10) {
				res = res + "0";
			}
			res = res + decToHex(tmp);
		}
		offset += nBytePerChar; // offset tiep theo
	}
	return res;

}

//bo khoang trang cuoi cua ten file
void FAT32::removeLastSpaces(wstring& name) {
	size_t idx = name.find_last_not_of(L" ");
	if (idx != string::npos)
		name.erase(idx + 1);
	else
		name.clear();
	return;
}

Component* FAT32::getRoot() {
	return rootDir;
}

wstring to_wstring(const string& stringToConvert)
{
	wstring wideString = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(stringToConvert);
	return wideString;
}

// read utf16 string
wstring FAT32::readStringFromBytes(string input) {
	int len = input.length();
	if (len % 4 != 0)
		return L"";
	int cvtlen = len / 4 + 1;
	wchar_t* wbuff = new wchar_t[cvtlen];
	wchar_t* ptr = wbuff;
	wmemset(wbuff, 0, cvtlen);
	for (int i = 0; i < len; i += 4) {
		string nextbyte = input.substr(i, 4);
		*ptr = (wchar_t)stoi(nextbyte, NULL, 16);
		ptr++;
	}
	return wbuff;
}

// --------------------------------- cac ham chinh ----------------------------------------
void FAT32::readSector(LPCWSTR drive, int readPoint, BYTE*& sector, int size)
{
	int retCode = 0;
	DWORD bytesRead;
	HANDLE device = NULL;

	device = CreateFile(drive,    // Drive to open
		GENERIC_READ,           // Access mode
		FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
		NULL,                   // Security Descriptor
		OPEN_EXISTING,          // How to create
		0,                      // File attributes
		NULL);                  // Handle to template

	if (device == INVALID_HANDLE_VALUE) // Open Error
	{
		cout << "CreateFile: " << GetLastError() << endl;
		exit(0);
	}

	SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

	if (!ReadFile(device, sector, size, &bytesRead, NULL))
	{
		cout << "ReadFile: " << GetLastError() << endl;
		exit(0);
		//return 0;
	}
	else
	{
		return;
	}
}

void FAT32::readRDET(BYTE* RDET, int sectorIdx, Component* root) {
	//doc thong tin cua RDET
	readSector(drive, sectorIdx * 512, RDET, 512);

	//bo qua 2 entry dau
	int readPoint = 2 * 32;
	wstring name = L"";
	while (true) {
		unsigned int status = FAT32::getValueOfOffset(RDET, readPoint, 1); // doc trang thai cua entry

		if (status == 0x00) // neu bang 00 thi thoat khoi vong lap
			return;

		if (status != 0xE5) { // chi lay thong tin cua tap tin/thu muc khong bi xoa
			status = FAT32::getValueOfOffset(RDET, readPoint + 0x0B, 1); // lay attribute

			if (status == 0x0F) { // entry phu
				name = readStringFromBytes(getBytesForChar(RDET, readPoint + 0x01, 10, 2) + getBytesForChar(RDET, readPoint + 0x0E, 12, 2) + getBytesForChar(RDET, readPoint + 0x1C, 4, 2)) + name;
			}
			else if (status == 0x10 || status == 0x20) { // entry chinh
				vector<unsigned int> clusterList;
				clusterList.resize(0);// lay cac cluster cua tap tin/thu muc voi 1 cluster = 8 sector
				// lay cluster bat dau tren entry
				unsigned int clusterStart = getValueOfOffset(RDET, readPoint + 0x1A, 2);

				clusterList.push_back(clusterStart);
				unsigned int tempCluster = clusterStart;

				// doc bang FAT1
				int currentSectorIdx = -1; // sector hien tai cua bang FAT1
				BYTE* FAT1 = new BYTE[512];
				while (true) {
					unsigned int offset = tempCluster * 4;
					int tempSectorIdx = offset / 512; // chi so cua sector can doc
					if (tempSectorIdx != currentSectorIdx) { // neu khong phai sector hien tai
						currentSectorIdx = tempSectorIdx;
						readFAT1(FAT1, tempSectorIdx); // doc thong tin tren bang FAT
					}
					int statusOfCluster = getValueOfOffset(FAT1, offset - tempSectorIdx * 512, 4);
					if (statusOfCluster == 0x0FFFFFFF || statusOfCluster == 0x0FFFFFF7 || statusOfCluster == 0x0FFFFFF8 || statusOfCluster == 0) // kiem tra neu trang thai cua cluster la EOF/BAD
						break;
					clusterList.push_back(tempCluster++);
				}


				Component* tmp;
				if (status == 0x10) { // neu la thu muc
					tmp = new Folder;
					if (name == L"")
						name = readStringFromBytes(getBytesForChar(RDET, readPoint, 8, 1));
				}
				else { // neu la tap tin
					tmp = new File;
					if (name == L"") {
						name = readStringFromBytes(getBytesForChar(RDET, readPoint, 8, 1));
						removeLastSpaces(name);
						name += L"." + readStringFromBytes(getBytesForChar(RDET, readPoint + 8, 3, 1));
					}
				}
				tmp->setName(name);
				tmp->setClusters(clusterList);
				tmp->setSize(getValueOfOffset(RDET, readPoint + 0x1C, 4));

				name = L"";
				if (clusterList[0] != 0)// doc tap tin khong rong
				{
					for (int i = (clusterList[0] - 2) * sectorsPerCluster + firstSectorOfData; i < (clusterList[clusterList.size() - 1] - 1) * sectorsPerCluster + firstSectorOfData; i++)
					{

						tmp->addSectors(i);
					}
				}
				else tmp->GetSectors().resize(0);
				root->addComponent(tmp);

				if (status == 0x10) { //doc tiep neu ben trong thu muc van con
					BYTE* nextRDET = new BYTE[512];
					readRDET(nextRDET, reservedSectors + sectorsPerFAT * numbersOfFATs + (clusterStart - 2) * sectorsPerCluster, tmp);
				}
			}
			else
				name = L"";
		}
		readPoint += 32; // chuyen con tro toi entry ke tiep

		if (readPoint == 512) { // sau khi het 1 sector
			readPoint = 0;
			sectorIdx++;
			readSector(drive, sectorIdx * 512, RDET, 512);
		}
	}
}

// read FAT1 table at n_th sector
void FAT32::readFAT1(BYTE*& FAT1, int sectorNth) {
	readSector(drive, (firstSectorOfFAT1 + sectorNth) * 512, FAT1, 512);
}

void FAT32::ReadFileData(Component* tmp) {
	if (tmp->getClassName() == "Folder") { // neu la folder -> in cay thu muc con
		cout << "Child directory tree:" << endl;
		cout << "[" << tmp->getClassName() << "] ";
		int mode = _setmode(_fileno(stdout), _O_U16TEXT);
		wcout << tmp->getName() << endl;
		mode = _setmode(_fileno(stdout), mode);
		int idx = 0;
		tmp->printDirTree(idx);
		return;
	}
	else { // neu la file -> in noi dung file
		int mode = 0;
		string res;
		if (tmp->getExtension() == L"txt" || tmp->getExtension() == L"TXT") {
			int size = tmp->getSize();
			if (size == 0)
			{
				cout << "File does not have content!" << endl;
				return;
			}
			else
			{
				int temp = tmp->GetSectors()[0];
				cout << "\t \t \t Content: " << endl;
				while (size > 0) {
					BYTE* DATA = new BYTE[512];
					this->readSector(drive, temp * 512, DATA, 512);
					res = GetString(DATA, 0x00, 512);
					wstring ws = to_wstring(res);
					mode = _setmode(_fileno(stdout), _O_U16TEXT);
					wcout << ws;
					mode = _setmode(_fileno(stdout), mode);
					size -= 512;
					temp++;
				}
				cout << endl;
				return;
			}

		}
		else {
			int mode = _setmode(_fileno(stdout), _O_U16TEXT);
			wcout << tmp->getExtension() << endl;
			mode = _setmode(_fileno(stdout), mode);
			cout << "Use another tool to read! " << endl;
			return;
		}
	}
}

void FAT32::printRDET()
{
	BYTE* RDET = new BYTE[512];
	readRDET(RDET, firstSectorOfRDET, rootDir);
	delete[]RDET;
	RDET = NULL;
}

void FAT32::printDataFromBootSector()
{
	cout << "Bytes per sector: " << this->bytesPerSector << endl;
	cout << "Sectors per cluster: " << this->sectorsPerCluster << endl;
	cout << "Reserved sectors: " << this->reservedSectors << endl;
	cout << "Number of FATs: " << this->numbersOfFATs << endl;
	cout << "Total sectors: " << this->totalSectors << endl;
	cout << "Sectors per FAT: " << this->sectorsPerFAT << endl;
	cout << "Root cluster: " << this->rootCluster << endl;
	cout << "The first sector of FAT1 table: " << firstSectorOfFAT1 << endl;
	cout << "The first sector of RDET: " << firstSectorOfRDET << endl;
	cout << "The first sector of Data: " << firstSectorOfData << endl;
}