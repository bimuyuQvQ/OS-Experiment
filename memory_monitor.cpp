#include <iostream>
#include <sysinfoapi.h>
#include <psapi.h>
#include <windows.h>
#include <string>
#include <vector>
#include "afxres.h"
#include <TlHelp32.h>
#include <shlwapi.h>
#include <Psapi.h>
#define DIV 1048576 // convert to Mb
using namespace std;

void showSysInfo()
{
    SYSTEM_INFO systemInfo;
    GetNativeSystemInfo(&systemInfo);
    long long int mem_size = (char *)systemInfo.lpMaximumApplicationAddress - (char *)systemInfo.lpMinimumApplicationAddress;
    cout << "处理器个数：" << systemInfo.dwNumberOfProcessors << endl;
    cout << "物理页大小：" << systemInfo.dwPageSize << "Byte" << endl;
    cout << "进程最低内存地址：" << systemInfo.lpMinimumApplicationAddress << endl;
    cout << "进程最高内存地址：" << systemInfo.lpMaximumApplicationAddress << endl;
    cout << "进程可用空间大小：" << mem_size / DIV / DIV << "TB" << endl;
    return;
}

void showMemoryUsed()
{
    MEMORYSTATUSEX globalMemory;

    globalMemory.dwLength = sizeof(globalMemory);
    // performanceInfo.cb=sizeof(performanceInfo);

    GlobalMemoryStatusEx(&globalMemory);
    // GetPerformanceInfo(&performanceInfo, performanceInfo.cb);

    cout << "物理内存使用情况：" << endl;
    cout << "实际物理内存：" << globalMemory.ullTotalPhys / DIV << "Mb" << endl;
    cout << "剩余可用物理内存：" << globalMemory.ullAvailPhys / DIV << "Mb" << endl;
    cout << "使用百分比：" << globalMemory.dwMemoryLoad << endl;
    cout << "全部虚拟内存：" << globalMemory.ullTotalVirtual / DIV << "Mb" << endl;
    cout << "剩余可用虚拟内存：" << globalMemory.ullAvailVirtual / DIV << "Mb" << endl;
}

void showAllProce()
{
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        printf("CreateToolhelp32Snapshot调用失败\n");
        return;
    }
    BOOL bMore = ::Process32First(hProcessSnap, &pe32);
    printf("进程名称\t\t进程ID号\t线程数\n");
    while (bMore)
    {   
        printf("%s\t\t", pe32.szExeFile);
        printf("%u\t\t", pe32.th32ProcessID);
        printf("%d\n", pe32.cntThreads);

        bMore = ::Process32Next(hProcessSnap, &pe32);
    }

    ::CloseHandle(hProcessSnap);
}

void printPageProtection(DWORD dwTarget)
{
    switch (dwTarget)
    {
    case (PAGE_READONLY):
    {
        printf("\tREADONLY");
        break;
    }
    case (PAGE_GUARD):
    {
        printf("\tGUARD");
        break;
    }
    case (PAGE_NOCACHE):
    {
        printf("\tNOCACHE");
        break;
    }
    case (PAGE_NOACCESS):
    {
        printf("\tNOACCESS");
        break;
    }
    case (PAGE_READWRITE):
    {
        printf("\tREADWRITE");
        break;
    }
    case (PAGE_WRITECOPY):
    {
        printf("\tWRITECOPY");
        break;
    }
    case (PAGE_EXECUTE):
    {
        printf("\tEXECUTE");
        break;
    }
    case (PAGE_EXECUTE_READ):
    {
        printf("\tEXECUTE_READ");
        break;
    }
    case (PAGE_EXECUTE_READWRITE):
    {
        printf("\tEXECUTE_READWRITE");
        break;
    }
    case (PAGE_EXECUTE_WRITECOPY):
    {
        printf("\tEXECUTE_WRITECOPY");
        break;
    }
    default:
        break;
    }
}

void showProceDetail(int PID)
{
    printf("内存\t\t\t\t\t块大小\t块内页状态\t保护方式\t\t块类型\n");
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);

    LPCVOID pbegin = (LPVOID)si.lpMinimumApplicationAddress;

    HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
    if (hprocess == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to OpenProcess" << endl;
        exit(-1);
    }

    MEMORY_BASIC_INFORMATION mem;
    while (pbegin < (LPVOID)si.lpMaximumApplicationAddress)
    {
        VirtualQueryEx(hprocess, pbegin, &mem, sizeof(mem));
        LPCVOID p_end = (PBYTE)pbegin + mem.RegionSize;

        printf("%012X-%012X", (DWORD *)pbegin, (DWORD *)p_end);
        //输出块大小
        printf("\t%dKb", mem.RegionSize/1024);

        //输出块的状态

        if (mem.State == MEM_COMMIT)
        {
            printf("\t已提交");
        }
        else if (mem.State == MEM_FREE)
        {
            printf("\t空闲");
        }
        else if (mem.State == MEM_RESERVE)
        {
            printf("\t保留");
        }

        //显示块内页的保护方式
        if (mem.Protect == 0 && mem.State != MEM_FREE)
        {
            mem.Protect = PAGE_READONLY;
        }
        printPageProtection(mem.Protect);

        //显示块的类型 邻近页面物理存储器类型指的是与给定地址所在页面相同的存储器类型
        if (mem.Type == MEM_IMAGE)
        {
            std::cout << "\t\tImage";
        }
        else if (mem.Type == MEM_PRIVATE)
        {
            std::cout << "\t\tPrivate";
        }
        else if (mem.Type == MEM_MAPPED)
        {
            std::cout << "\t\tMapped";
        }
        cout << endl;

        //移动块指针获得下一个块
        if (pbegin == p_end) 
            break;
        pbegin = p_end;
    }
}

int main()
{
    while (1)
    {
        printf("0--退出\n1--系统地址空间基本信息\n2--内存使用情况\n3--所有运行中进程\n4--某进程信息\n");
        int s;
        cin >> s;
        if (s == 0)
        {
            return 0;
        }
        if (s == 1)
        {
            showSysInfo();
            continue;
        }
        if (s == 2)
        {
            showMemoryUsed();
            continue;
        }
        if (s == 3)
        {
            showAllProce();
            continue;
        }
        if (s == 4)
        {
            int PID;
            showAllProce();
            cout<<"请输入想查询的PID"<<endl;
            cin >> PID;
            showProceDetail(PID);
            continue;
        }
    }
}