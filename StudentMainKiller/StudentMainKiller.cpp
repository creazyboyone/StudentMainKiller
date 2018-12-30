#include <windows.h>
#include <tlhelp32.h>	//���̿��պ���ͷ�ļ�
#include <iostream>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"
#include <tchar.h>     //_T��
#define MAXPATH  1024
using namespace std;


bool releaseNTSD();

int main(int argc, char const *argv[])
{
	int progressID = -1;
	int countProcess=0;									//��ǰ����������������
	PROCESSENTRY32 currentProcess;						//��ſ��ս�����Ϣ��һ���ṹ��
	currentProcess.dwSize = sizeof(currentProcess);		//��ʹ������ṹ֮ǰ�����������Ĵ�С
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//��ϵͳ�ڵ����н�����һ������
	
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		cout << "CreateToolhelp32Snapshot()����ʧ��!" << endl;
		return -1;
	}
	
	BOOL bMore=Process32First(hProcess,&currentProcess);	//��ȡ��һ��������Ϣ
	while(bMore)
	{
		wcout << "PID=  " << currentProcess.th32ProcessID << "  PName= " << currentProcess.szExeFile << endl;	//�������̿��գ�������ʾÿ��������Ϣ
		//win7+��StudentMain.exe,winxp��StudentEx.exe
		if(wcscmp(currentProcess.szExeFile,L"StudentMain.exe")==0 || wcscmp(currentProcess.szExeFile, L"StudentEx.exe") == 0)
		{
			progressID = currentProcess.th32ProcessID;
		}
		bMore=Process32Next(hProcess,&currentProcess);	//������һ��
		countProcess++;
	}
	
	CloseHandle(hProcess);	//���hProcess���
	cout << "�������� " << countProcess << " ������������\n";
	if(progressID != -1)
	{
		cout << "���̱�ʶ��(PID)�� " << progressID << ",���ڽ�������...\n";
		if (releaseNTSD())
		{
			char pid[10];
			_itoa_s(progressID, pid, 10);
			char head[25] = "ntsd -c q -p ";
			strcat_s(head, sizeof(head), pid);
			int i = system(head);
			if (i >= 0)
			{
				cout << "�ɹ�����" << endl;
				Sleep(1000);
			}
			else
			{
				cout << "������ô���" << endl;
				system("pause");
			}
			//���������ɣ��Ƴ������ļ�
			if (remove("ntsd.exe") == EOF) {
				cout << "�����ļ�����" << endl;
				system("pause");
			}
		}
		else
		{
			cout << "�ͷ���Դ����" << endl;
		}
		
	}else
	{
		cout << "û�д˽��̣�" << endl;
		system("pause");
	}
	
	
	return 0;
}

bool releaseNTSD() {
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_EXE1), _T("EXE"));
	if (NULL == hRes)
		return false;

	DWORD dwSize = SizeofResource(NULL, hRes);
	if (0 == dwSize)
		return false;

	HGLOBAL hGlobal = LoadResource(NULL, hRes);
	if (NULL == hGlobal)
		return false;

	LPVOID lp = LockResource(hGlobal);
	if (NULL == lp)
		return false;

	HANDLE fp = CreateFile(L"ntsd.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);  // CREATE_ALWAYSΪ�����ļ��治���ڶ��������ļ���
	DWORD a;
	//sizeofResource �õ���Դ�ļ��Ĵ�С
	if (!WriteFile(fp, lp, dwSize, &a, NULL))
		return false;
	CloseHandle(fp);//�رվ��
	FreeResource(hGlobal);//�ͷ��ڴ�
	return true;
}