#include "NTFS.h"

NTFS::NTFS()
{
	this->drive = L"\\\\.\\ F:";
	this->sector = new BYTE[512];
	readSector(drive, 0, sector, 512);
	this->bytesPerSector = getValueOfOffset(sector, 0x0B, 2);
	this->sectorsPerCluster = getValueOfOffset(sector, 0x0D, 1);
	this->sectorsPerTrack = getValueOfOffset(sector, 0x18, 2);
	this->totalSectors = getValueOfOffset(sector, 0x28, 8);
	this->MFTClusterNumber = getValueOfOffset(sector, 0x30, 8);//tinh sector bat dau
	this->MFTMirrrClusterNumber = getValueOfOffset(sector, 0x38, 8);
	this->clusterPerFileRecordSegment = calcMFTEntry(getValueOfOffset(sector, 0x40, 1)); // 1024 -> signed int

	rootDir = new Folder;
}

NTFS::NTFS(LPCWSTR drive)
{
	this->sector = new BYTE[512];
	// read Bootsector to get data
	this->drive = drive;
	readSector(drive, 0, sector, 512);
	this->bytesPerSector = getValueOfOffset(sector, 0x0B, 2);
	this->sectorsPerCluster = getValueOfOffset(sector, 0x0D, 1);
	this->sectorsPerTrack = getValueOfOffset(sector, 0x18, 2);
	this->totalSectors = getValueOfOffset(sector, 0x28, 8);
	this->MFTClusterNumber = getValueOfOffset(sector, 0x30, 8);//tinh sector bat dau
	this->MFTMirrrClusterNumber = getValueOfOffset(sector, 0x38, 8);
	this->clusterPerFileRecordSegment = calcMFTEntry(getValueOfOffset(sector, 0x40, 1)); // 1024 -> signed int

	rootDir = new Folder;
}

NTFS::~NTFS()
{
	delete sector;
	sector = NULL;
	delete rootDir;
	rootDir = NULL;
}

void NTFS::readSector(LPCWSTR drive, int readPoint, BYTE*& sector, int size)
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

void NTFS::readSectorEx(LPCWSTR drive, unsigned int readPoint, BYTE*& sector, int size)
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

	LARGE_INTEGER read;
	read.QuadPart = readPoint * 512;
	if (device == INVALID_HANDLE_VALUE) // Open Error
	{
		cout << "CreateFile: " << GetLastError() << endl;
		exit(0);
	}
	SetFilePointerEx(device, read, 0, FILE_BEGIN);//Set a Point to Read

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

string NTFS::decToHex(int decNumber)
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

// nhan dang offset theo format 0xAB : dong A cot B
string NTFS::getByteToString(BYTE* sector, int offset, int nByte)
{
	string res;
	string tmpRes;
	// nhan dang LE va chuyen nguoc lai
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

unsigned int NTFS::hexToDec(string hexNumber)
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

unsigned int NTFS::calcMFTEntry(unsigned int decNumber)
{
	// doi chuoi nhi phan
	string tmpRes;
	while (decNumber > 0)
	{
		int tmp = decNumber % 2;
		decNumber = decNumber / 2;
		tmpRes.push_back(char(tmp + 48));
	}
	reverse(tmpRes.begin(), tmpRes.end());

	// dao bit
	for (int i = 0; i < tmpRes.length(); i++)
	{
		if (tmpRes[i] == '0')
		{
			tmpRes[i] = '1';
		}
		else tmpRes[i] = '0';
	}

	// cong 1
	int tmp;
	int index = tmpRes.length() - 1;
	do
	{
		if (tmpRes[index] == '0')
		{
			tmpRes[index] = '1';
			tmp = 0;
			break;
		}
		else
		{
			tmpRes[index] = '0';
			tmp = 1;
		}
		index--;
	} while (true);

	// doi sang he 10
	tmp = 0;
	unsigned int res = 0;
	for (int i = tmpRes.length() - 1; i >= 0; i--)
	{
		if (tmpRes[i] == '1')
		{
			res = res + 1 * pow(2, tmp);
		}
		tmp++;
	}

	return pow(2, res);
}

void NTFS::printDataFromBPB()
{
	cout << "Bytes per sector: " << this->bytesPerSector << endl;
	cout << "Sectors per cluster: " << this->sectorsPerCluster << endl;
	cout << "Sectors per track: " << this->sectorsPerTrack << endl;
	cout << "Total sectors: " << this->totalSectors << endl;
	cout << "$MFT cluster number: " << this->MFTClusterNumber << endl;
	cout << "$MFTMirr cluster number: " << this->MFTMirrrClusterNumber << endl;
	cout << "Cluster per File Record Segment: " << this->clusterPerFileRecordSegment << endl;

}

void NTFS::printSector(BYTE*& sector)
{
	int count = 0;
	int num = 0;

	cout << "         0  1  2  3  4  5  6  7    8  9  A  B  C  D  E  F" << endl;

	cout << "0x00" << this->decToHex(num) << "0 ";
	bool flag = 0;
	for (int i = 0; i < 512; i++)
	{
		if (i % 16 == 0 && i != 0) // kiem tra het mot offset
		{
			num++;
			cout << endl;
			string tmp = decToHex(num);

			if (tmp.length() == 1)
			{
				tmp = "0x00" + tmp;
			}
			else tmp = "0x0" + tmp;
			tmp.push_back('0');
			cout << tmp << " ";
		}
		if (i % 8 == 0)
		{
			cout << "  ";
		}
		printf("%02X ", sector[i]); // in sector thu i dang hex
	}
	cout << endl;
}

unsigned int NTFS::getValueOfOffset(BYTE* sector, int offset, int nByte)
{
	return hexToDec(getByteToString(sector, offset, nByte));
}

//Doc BYTE tu DATA thanh 1 string
string NTFS::getString(BYTE* DATA, int offset, int num) {
	char* res = new char[num + 1];
	memcpy(res, DATA + offset, num);
	string s = "";
	for (int i = 0; i < num; i++)
		if (res[i] != 0x00 && res[i] != 0xFF)
			s += res[i];
	return s;
}

wstring NTFS::to_wstring(const string& stringToConvert)
{
	wstring wideString = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(stringToConvert);
	return wideString;
}

// get bytes in the format of 2 bytes per character
string NTFS::getBytesForChar(BYTE* DATA, int offset, int number, int nBytePerChar)
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

// read utf16 string
wstring NTFS::readStringFromBytes(string input) {
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

void NTFS::Read_MFT(BYTE* MFT)
{
	vector<Component*> componentList; // danh sach cac file/folder doc duoc
	unsigned int index = MFTClusterNumber * sectorsPerCluster; // sector bat dau MFT
	int dem = 0;
	bool flag = 0;
	while (dem < 60)
	{
		// doc MFTEntry
		readSectorEx(this->drive, index, MFT, 1024);
		//if (getByteToString(MFT, 0x00, 4) == "454C4946")
		if (to_wstring(getString(MFT, 0x00, 4)) == L"FILE")
		{
			int ID = getValueOfOffset(MFT, 0x2C, 4);
			if (ID > 38) // khong phai tap tin he thong
			{
				cout << endl << endl;
				cout << "ID: " << ID << endl;
				flag = 1;

				// -------- DOC $STANDARD_INFORMATION --------
				int Start_Att_InF = getValueOfOffset(MFT, 0x014, 2); // offset bat dau attribute $STANDARD_INFORMATION
				cout << "Attribute $STANDARD_INFORMATION" << endl;
				int INF_size = getValueOfOffset(MFT, Start_Att_InF + 4, 4); // kich thuoc cua attribute $STANDARD_INFORMATION
				cout << "Size (include header) of attribute: " << INF_size << endl;
				int fol = getValueOfOffset(MFT, Start_Att_InF + 56, 4); // xac dinh la folder hay file
				Component* comp = NULL; // tap tin dang doc
				if (fol == 0)
					comp = new Folder;
				else
					comp = new File;
				comp->setID(ID);
				cout << "--> " << comp->getClassName() << endl;

				// -------- DOC $FILE_NAME --------
				int Start_Att_FN = Start_Att_InF + INF_size; // offset bat dau attribute $FILE_NAME
				cout << "Attribute $FILE_NAME" << endl;
				// lay size
				int FN_size = getValueOfOffset(MFT, Start_Att_FN + 4, 4); // kich thuoc cua attribute $FILE_NAME
				cout << "Size (include header) of attribute: " << FN_size << endl;
				// lay parent id
				int Parent_id = getValueOfOffset(MFT, Start_Att_FN + 24, 6);
				// them parent vo dsach
				if (Parent_id > 38) 
					cout << "- Parent ID: " << Parent_id << endl;
				else
				{
					Parent_id = 0;
					cout << "- Parent ID: null" << endl;
				}
				comp->setParentID(Parent_id);
				// doc folder/file name
				int lengthName = getValueOfOffset(MFT, Start_Att_FN + 88, 1); // so ki tu cua ten file/folder
				cout << "- Name length: " << lengthName << endl;
				wstring filename = readStringFromBytes(getBytesForChar(MFT, Start_Att_FN + 90, lengthName * 2, 2));
				comp->setName(filename);
				int mode = _setmode(_fileno(stdout), _O_U16TEXT);
				wcout << "- Name: " << filename << endl;
				mode = _setmode(_fileno(stdout), mode);

				if (fol == 0) { // neu la folder
					int Start_Att_IDXROOT = Start_Att_FN + FN_size; // offset bat dau attribute $INDEX_ROOT
					int tmp = getValueOfOffset(MFT, Start_Att_IDXROOT, 4); // lay id cua attribute
					if (tmp == 64) // neu la attribute $OBJECT_ID -> bo qua
					{
						int len_object = getValueOfOffset(MFT, Start_Att_IDXROOT + 4, 4); // kich thuoc cua attribute $OBJECT_ID
						Start_Att_IDXROOT += len_object;
					}
					// -------- DOC $INDEX_ROOT --------
					cout << "Attribute $INDEX_ROOT" << endl;
					int IDXROOT_size = getValueOfOffset(MFT, Start_Att_IDXROOT + 4, 4); // kich thuoc cua attribute $INDEX_ROOT
					cout << "Size (include header) of attribute: " << IDXROOT_size << endl;
					int dataStart = Start_Att_IDXROOT + getValueOfOffset(MFT, Start_Att_IDXROOT + 20, 2); // offset bat dau phan noi dung cua attribute
					int idxHeader = dataStart + 16; // offset bat dau Index Header

					cout << "- Child IDs: ";
					int idxEntry = idxHeader + getValueOfOffset(MFT, idxHeader, 4); // offset bat dau index entry dau tien
					while (true) {
						int referenceID = getValueOfOffset(MFT, idxEntry, 6); // lay ID cua file/folder duoc tham chieu toi
						if (referenceID == 0) // het cac index entry
							break;
						cout << referenceID << ", ";
						comp->addReferenceID(referenceID); // them vao danh sach cac ID cua tap tin con
						int idxEntryLen = getValueOfOffset(MFT, idxEntry + 8, 2); // do dai cua index entry
						idxEntry += idxEntryLen; // index entry tiep theo
					}
					cout << endl;
				}
				else { // neu la file
					int Start_Att_DATA = Start_Att_FN + FN_size; // offset bat dau attribute $DATA
					int tmp = getValueOfOffset(MFT, Start_Att_DATA, 4); // lay id cua attribute
					if (tmp == 64) // neu la attribute $OBJECT_ID -> bo qua
					{
						int len_object = getValueOfOffset(MFT, Start_Att_DATA + 4, 4); // kich thuoc cua attribute $OBJECT_ID
						Start_Att_DATA += len_object;
					}
					// DOC $DATA
					cout << "Attribute $DATA" << endl; 
					int DT_size = getValueOfOffset(MFT, Start_Att_DATA + 4, 4); // kich thuoc cua attribute $DATA
					cout << "Size (include header) of attribute: " << DT_size << endl;
					int type = getValueOfOffset(MFT, Start_Att_DATA + 8, 1);

					if (type == 0)
					{
						cout << "Type: Resident" << endl;
						readFileContent(comp, MFT, Start_Att_DATA);
					}
					else
						cout << "Type: Non-Resident" << endl;
				}
				componentList.push_back(comp); // them vao danh sach cac file/folder doc duoc
			}
		}
		else if (flag == 1)
		{
			break;
		}
		index += 2; // MFT entry tiep theo
		dem++;
	}
	linkComponent(componentList); // lien ket cac file/folder theo ID de tao thanh cay thu muc
	return;
}

void NTFS::readFileContent(Component* comp, BYTE* MFT, int Start_Att_DATA) {
	if (comp->getClassName() == "Folder")
		return;
	wstring exts = comp->getExtension(); // lay duoi mo rong cua tap tin
	if (exts == L"txt" || exts == L"TXT") {
		int Content_size = getValueOfOffset(MFT, Start_Att_DATA + 16, 4); // kich thuoc phan noi dung cua tap tin
		cout << "File size: " << Content_size << " bytes" << endl;
		comp->setSize(Content_size);
		if (Content_size == 0) {
			cout << "File does not have content!" << endl;
			return;
		}
		int Content_start = getValueOfOffset(MFT, Start_Att_DATA + 20, 2); // offset bat dau phan noi dung
		wstring content = to_wstring(getString(MFT, Start_Att_DATA + Content_start, Content_size));
		comp->setContent(content);
		cout << "Content:" << endl;
		int mode = _setmode(_fileno(stdout), _O_U16TEXT);
		wcout << content << endl;
		mode = _setmode(_fileno(stdout), mode);
	}
	else {
		int mode = _setmode(_fileno(stdout), _O_U16TEXT);
		wcout << L"Use another tool to read " << comp->getExtension() << L" file" << endl;
		mode = _setmode(_fileno(stdout), mode);
	}
}

// tim tap tin theo ID tu danh sach cac tap tin
Component* NTFS::findComponent(vector<Component*> comps, int ID) {
	for (int i = 0; i < comps.size(); i++)
		if (ID == comps[i]->getID())
			return comps[i];
	return NULL;
}

// lien ket cac file/folder theo ID de tao thanh cay thu muc
void NTFS::linkComponent(vector<Component*> comps) {
	for (int i = 0; i < comps.size(); i++) { // duyet het danh sach cac tap tin da doc duoc
		if (comps[i]->getParentID() == 0) // nam o thu muc goc
			rootDir->addComponent(comps[i]);
		vector<int> tmp = comps[i]->getReferenceIDs(); // lay danh sach cac ID ma tap tin hien tai tham chieu toi
		for (int j = 0; j < tmp.size(); j++) {
			Component* childComp = findComponent(comps, tmp[j]); // tim tap tin theo ID tu danh sach cac tap tin
			if (childComp) // neu tim thay
				comps[i]->addComponent(childComp); // them tap tin do vao danh sach cac tap tin con
		}
	}
}

Component* NTFS::getRoot() {
	return rootDir;
}