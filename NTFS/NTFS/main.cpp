#include<iostream>
#include<cstdlib>
#include<stdio.h>
#include<vector>
#include<windows.h>
#include<iomanip>
#include<sstream>
#include <string>
#include <map>

#include "NTFS.h"
using namespace std;

int main()
{
	wstring disk_name;
	cout << "Input volume name: ";
	wcin >> disk_name;
	disk_name = L"\\\\.\\" + disk_name + L":";
	LPCWSTR drive = disk_name.c_str(); //chuyen tu string -> cstring

	NTFS myNTFS(drive);
	BYTE* BPB = new BYTE[512];
	BYTE* MFT = new BYTE[1024];
	Component* root = myNTFS.getRoot();
	

	int choice;
	string cont, cont1 = "";
	while(true)
	{
		cout << "1. Print data of Partition Boot Sector" << endl;;
		cout << "2. Detail information and directory tree" << endl;
		cout << "3. Exit" << endl;
		cout << "Your choice: ";
		cin >> choice;

        switch (choice)
        {
        case 1:
        {
            system("cls");
            cout << "\t \t \t Data of bootsector " << endl;
            myNTFS.printDataFromBPB();
            cout << endl;
            break;
        }
        case 2:
        {
            system("cls");
            cout << "\t \t \t Details information " << endl;
            myNTFS.Read_MFT(MFT);
            cout << endl << endl;
            while (true) {
                cout << "\t \t \t Directory tree:" << endl;
                root->printDirTree();
                int ID;
                cout << "ID: ";
                cin >> ID;

                system("cls");
                Component* temp = root->findWithID(ID);
                if (temp)
                    temp->printInfo();
                cout << endl;

                cout << "Continue or not (yes/no): ";
                cin >> cont1;
                if (cont1 == "no")
                {
                    break;
                }
                else system("cls");
            }
            break;
        }
        default:
        {
            exit(0);
            break;
        }
        }
        if (cont1 == "no")
            break;

        cout << "Continue or not (yes/no): ";
        cin >> cont;
        if (cont == "no")
        {
            break;
        }
        else system("cls");
	}

    myNTFS.~NTFS();
    //root->~Component();
    delete[] BPB;
    delete[] MFT;
    BPB = NULL;
    MFT = NULL;
}