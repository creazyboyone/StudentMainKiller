#include <windows.h>
#include <tlhelp32.h>	//进程快照函数头文件
#include <iostream>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"
#include <tchar.h>     //_T宏
#define MAXPATH  1024
using namespace std;


bool releaseNTSD();

int main(int argc, char const *argv[])
{
	int progressID = -1;
	int countProcess=0;									//当前进程数量计数变量
	PROCESSENTRY32 currentProcess;						//存放快照进程信息的一个结构体
	currentProcess.dwSize = sizeof(currentProcess);		//在使用这个结构之前，先设置它的大小
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//给系统内的所有进程拍一个快照
	
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		cout << "CreateToolhelp32Snapshot()调用失败!" << endl;
		return -1;
	}
	
	BOOL bMore=Process32First(hProcess,&currentProcess);	//获取第一个进程信息
	while(bMore)
	{
		wcout << "PID=  " << currentProcess.th32ProcessID << "  PName= " << currentProcess.szExeFile << endl;	//遍历进程快照，轮流显示每个进程信息
		//win7+是StudentMain.exe,winxp是StudentEx.exe
		if(wcscmp(currentProcess.szExeFile,L"StudentMain.exe")==0 || wcscmp(currentProcess.szExeFile, L"StudentEx.exe") == 0)
		{
			progressID = currentProcess.th32ProcessID;
		}
		bMore=Process32Next(hProcess,&currentProcess);	//遍历下一个
		countProcess++;
	}
	
	CloseHandle(hProcess);	//清除hProcess句柄
	cout << "共有以上 " << countProcess << " 个进程在运行\n";
	if(progressID != -1)
	{
		cout << "进程标识符(PID)是 " << progressID << ",正在结束进程...\n";
		if (releaseNTSD())
		{
			char pid[10];
			_itoa_s(progressID, pid, 10);
			char head[25] = "ntsd -c q -p ";
			strcat_s(head, sizeof(head), pid);
			int i = system(head);
			if (i >= 0)
			{
				cout << "成功！！" << endl;
				Sleep(1000);
			}
			else
			{
				cout << "程序调用错误" << endl;
				system("pause");
			}
			//程序调用完成，移除依赖文件
			if (remove("ntsd.exe") == EOF) {
				cout << "依赖文件错误" << endl;
				system("pause");
			}
		}
		else
		{
			cout << "释放资源错误！" << endl;
		}
		
	}else
	{
		cout << "没有此进程！" << endl;
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

	HANDLE fp = CreateFile(L"ntsd.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);  // CREATE_ALWAYS为不管文件存不存在都产生新文件。
	DWORD a;
	//sizeofResource 得到资源文件的大小
	if (!WriteFile(fp, lp, dwSize, &a, NULL))
		return false;
	CloseHandle(fp);//关闭句柄
	FreeResource(hGlobal);//释放内存
	return true;
}