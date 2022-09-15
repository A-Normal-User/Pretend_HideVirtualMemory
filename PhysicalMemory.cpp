#include <iostream>
#include <windows.h>
#include <sysinfoapi.h>
#include <memoryapi.h>

using namespace std;

void EnableSeLockMemoryPrivilege();

int main()
{
	SYSTEM_INFO sysInfo;
	ULONG_PTR PageArray = 0;
	ULONG_PTR PageArraySize = 1;
	PVOID Address = NULL;
	BOOL bRet = FALSE;
	
	//调整进程权限
	EnableSeLockMemoryPrivilege();
	//获取系统信息
	GetSystemInfo(&sysInfo);
	//输出PageSize
	cout << "当前系统的PageSize是：" << sysInfo.dwPageSize << "\n";
	//分配一块虚拟内存
	Address = VirtualAlloc(NULL, sysInfo.dwPageSize, MEM_RESERVE|MEM_PHYSICAL, PAGE_READWRITE);
	if (Address == NULL)
	{
		cout << "分配虚拟内存失败！\n";
		return 0;
	}
	//分配物理内存
	bRet = AllocateUserPhysicalPages(GetCurrentProcess(), &PageArraySize, &PageArray);
	if (!bRet) {
		cout << "分配虚拟内存失败！\n";
		return 0;
	}
	//cout << "分配的物理内存是：" << PageArray << "\n";
	//将物理内存映射到虚拟内存
	MapUserPhysicalPages(Address, 1, &PageArray);
	//向虚拟内存写入内容
	*(int*)Address = 114514;
	//解除物理内存映射
	MapUserPhysicalPages(Address, 1, NULL);
	cout << "写入的内容是整数：114514，写入地址：0x" << Address << "，你可以试试能不能搜索到数据，如果搜到了请修改，后面还会读取一次\n";
	system("pause");
	//再次将物理内存映射到虚拟内存
	MapUserPhysicalPages(Address, 1, &PageArray);
	cout << "读取的内容是整数：" << *(int*)Address << "\n";
	//释放物理内存
	FreeUserPhysicalPages(GetCurrentProcess(), &PageArraySize, &PageArray);
	//释放虚拟内存
	VirtualFree(Address, 0, MEM_RELEASE);
	cout << "测试结束\n";
	system("pause");
	return 0;
}

void EnableSeLockMemoryPrivilege() {
	//调整当前进程权限，使得进程有SeLockMemory的权限
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	memset(&tkp, 0, sizeof(tkp));
	// Get a token for this process.
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		cout << "OpenProcessToken failed" << endl;
		return;
	}
	// Get the LUID for the lock memory privilege.
	LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	// Get the lock memory privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS)
	{
		cout << "AdjustTokenPrivileges failed" << endl;
		return;
	}
}