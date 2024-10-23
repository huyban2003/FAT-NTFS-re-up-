#include<iostream>
#include<cstdlib>
#include<stdio.h>
#include<vector>
#include<windows.h>
#include<iomanip>
#include<sstream>
#include <string>
#include <map>

#include "FAT32.h"
using namespace std;



int main()
{
    wstring disk_name;
    cout << "Input volume name: ";
    wcin >> disk_name;
    disk_name = L"\\\\.\\" + disk_name + L":";
    LPCWSTR drive = disk_name.c_str(); //chuyen tu string -> cstring

    // khai bao
    int choice;
    string cont;

    FAT32 myFAT32(drive);
    BYTE* RDET = new BYTE[512];
    Component* root = myFAT32.getRoot();

    myFAT32.readRDET(RDET, myFAT32.GetFirstSectorRDET(), root);
    Component* tmp = NULL;

    // menu 
    while (true)
    {
        cout << "1. Print data of bootsector " << endl;
        cout << "2. Directory tree " << endl;
        cout << "3. Exit" << endl;
        cout << "Input your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            system("cls");
            cout << "\t \t \t Data of bootsector " << endl;
            myFAT32.printDataFromBootSector();
            cout << endl;
            break;
        }
        case 2:
        {
            system("cls");
            cout << "\t \t \t Directory tree " << endl;
            int idx = 0; // so thu tu cua folder/file trong cay thu muc
            root->printDirTree(idx);
            int ID; 
            cout << "Input ID: "; // nhap so thu tu
            cin >> ID;
            system("cls");
            int cnt = 0; // bien dem de tim id cua folder/file
            tmp = root->findWithID(ID, cnt);
            tmp->printInfo();
            cout << endl;
            myFAT32.ReadFileData(tmp);
            cout << endl << endl;

            break;
        }
        default:
        {
            exit(0);
            break;
        }
        }
        cout << "Continue or not (yes/no): ";
        cin >> cont;
        if (cont == "no")
        {
            break;
        }
        else system("cls");
    }
    // delete
    if (tmp)
    {
        tmp->~Component();
    }
    delete[]RDET;
    RDET = NULL;
    root->~Component();
    myFAT32.~FAT32();
}