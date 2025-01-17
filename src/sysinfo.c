/*****************************************
		NanoShell Operating System
		  (C) 2023 iProgramInCpp

        System Information  module
******************************************/
#include <main.h>
#include <misc.h>
#include <time.h>
#include <console.h>
#include <print.h>
#include <memory.h>
#include <multiboot.h>
#include <string.h>
#include <task.h>
#include <vga.h>
#include <config.h>

extern multiboot_info_t* g_pMultibootInfo;//main.c
extern Console* g_currentConsole;

extern uint32_t g_cpuidLastLeaf;
extern char g_cpuidNameEBX[];
extern char g_cpuidBrandingInfo[];
extern CPUIDFeatureBits g_cpuidFeatureBits;
extern uint32_t g_cpuidFeatureBits2;

extern void KeCPUIDAsm();

const char* GetCPUType()
{
	return g_cpuidNameEBX;
}

const char* GetCPUName()
{
	return g_cpuidBrandingInfo;
}

CPUIDFeatureBits GetCPUFeatureBits()
{
	return g_cpuidFeatureBits;
}

void KeCPUID()
{
	KeCPUIDAsm();
	
	if (g_cpuidFeatureBits2 < 0x80000004)
	{
		SLogMsg("CPUID doesn't support branding info, making up one of our own");
		snprintf(g_cpuidBrandingInfo, 49, "%s Fam %d Mod %d Step %d", g_cpuidNameEBX, g_cpuidFeatureBits.m_familyID, g_cpuidFeatureBits.m_model, g_cpuidFeatureBits.m_steppingID);
	}
}


void KePrintMemoryMapInfo()
{
	multiboot_info_t* mbi = g_pMultibootInfo;
	int len, addr;
	len = mbi->mmap_length, addr = mbi->mmap_addr;
	
	//turn this into a virt address:
	multiboot_memory_map_t* pMemoryMap;
	
	LogMsg("mmapAddr=%x mmapLen=%x", addr, len);
	addr += 0xC0000000;
	
	for (pMemoryMap = (multiboot_memory_map_t*)addr;
		 (unsigned long) pMemoryMap < addr + mbi->mmap_length;
		 pMemoryMap = (multiboot_memory_map_t*) ((unsigned long) pMemoryMap + pMemoryMap->size + sizeof(pMemoryMap->size)))
	{
		LogMsg("S:%x A:%x%x L:%x%x T:%x", pMemoryMap->size, 
			(unsigned)(pMemoryMap->addr >> 32), (unsigned)pMemoryMap->addr,
			(unsigned)(pMemoryMap->len  >> 32), (unsigned)pMemoryMap->len,
			pMemoryMap->type
		);
	}
}

void KePrintSystemInfoAdvanced()
{
	//oldstyle:
	/*
	LogMsg("Information about the system:");
	LogMsg("CPU Type:        %s", g_cpuidNameEBX);
	LogMsg("CPU Branding:    %s", g_cpuidBrandingInfo);
	LogMsg("Feature bits:    %x", *((int*)&g_cpuidFeatureBits));
	LogMsgNoCr("x86 Family   %d ", g_cpuidFeatureBits.m_familyID);
	LogMsgNoCr("Model %d ", g_cpuidFeatureBits.m_model);
	LogMsg("Stepping %d", g_cpuidFeatureBits.m_steppingID);
	LogMsg("g_cpuidLastLeaf: %d", g_cpuidLastLeaf);*/
	
	//nativeshell style:
	LogMsg("\e[96mNanoShell Operating System " VersionString);
	LogMsg("\e[91mVersion Number: %d", VersionNumber);
	
	LogMsg("\e[97m-------------------------------------------------------------------------------");
	LogMsg("\e[94m[CPU] Name: %s", GetCPUName());
	LogMsg("\e[94m[CPU] x86 Family %d Model %d Stepping %d.  Feature bits: %d",
			g_cpuidFeatureBits.m_familyID, g_cpuidFeatureBits.m_model, g_cpuidFeatureBits.m_steppingID);
	LogMsg("\e[92m[RAM] PageSize: 4K. Physical pages: %d. Total usable RAM: %d Kb", MpGetNumAvailablePages(), MpGetNumAvailablePages()*4);
	LogMsg("\e[92m[VID] Screen resolution: %dx%d.  Textmode size: %dx%d characters, of type %d.", GetScreenSizeX(), GetScreenSizeY(), 
													g_currentConsole->width, g_currentConsole->height, g_currentConsole->type);
	LogMsg("\e[97m");
}
void KePrintSystemInfo()
{
    // neofetch style:
    int npp = MpGetNumAvailablePages(), nfpp = MpGetNumFreePages();
    
    char timingInfo[128];
    timingInfo[0] = 0;
    FormatTime(timingInfo, FORMAT_TYPE_VAR, GetTickCount() / 1000);
    LogMsg("\e[93m X    X");
    LogMsg("\e[93m XX   X");
    LogMsg("\e[93m X X  X");
    LogMsg("\e[93m X  X X");
    LogMsg("\e[93m X   XX");
    LogMsg("\e[93m X    X\e[95m SSS");
    LogMsg("\e[95m      S    S");
    LogMsg("\e[95m      S     ");
    LogMsg("\e[95m       SSS  ");
    LogMsg("\e[95m          S ");
    LogMsg("\e[95m      S    S");
    LogMsg("\e[95m       SSS  ");
    
    // go up a bunch
    LogMsgNoCr("\e[A\e[A\e[A\e[A\e[A\e[A\e[A\e[A\e[A\e[A\e[A\e[A");
    
    LogMsg("\e[14G\e[91m OS:       \e[97mXShell Operating System");
    LogMsg("\e[14G\e[91m Kernel:   \e[97m%s (%d)", VersionString, VersionNumber);
    LogMsg("\e[14G\e[91m Uptime:   \e[97m%s", timingInfo);
    LogMsg("\e[14G\e[91m CPU:      \e[97m%s", GetCPUName());
    LogMsg("\e[14G\e[91m CPU type: \e[97m%s", GetCPUType());
    LogMsg("\e[14G\e[91m Memory:   \e[97m%d KB / %d KB", (npp-nfpp)*4, npp*4);
    LogMsg("\e[14G\e[91m ");
    LogMsg("\e[14G\e[91m ");
    LogMsg("\e[14G\e[91m ");
    LogMsg("\e[14G\e[91m ");
    LogMsg("\e[14G\e[91m ");
    LogMsg("\e[14G\e[91m ");
    LogMsg("\e[0m");
}

bool KiEmergencyMode()
{
    bool textMode = true;
    ConfigEntry *pEntry = CfgGetEntry("emergency");
    if (pEntry)
    {
        if (strcmp(pEntry->value, "yes") == 0)
        {
            ILogMsg("run text mode emergency");
        }
        else
        {
            textMode = false;
        }
    }
    else
    {
        ILogMsg("No 'emergency' config key found, using text mode");
    }
    return textMode;
}

void KePrintSystemVersion()
{
    LogMsg("XShell (TM), December 2023 - " VersionString);
    LogMsg("[%d Kb System Memory, %d Kb Usable Memory]", g_pMultibootInfo->mem_upper, MpGetNumAvailablePages() * 4);
    LogMsg("Built on: %s %s", __DATE__, __TIME__);
}

void MbSetup(uint32_t check, uint32_t mbaddr)
{
    if (check != 0x2badb002)
    {
        ILogMsg("XShell has not booted from a Multiboot-compatible bootloader. A bootloader such as GRUB is required to run XShell.");
        KeStopSystem();
    }
    
    // Read the multiboot data:
    multiboot_info_t *mbi = (multiboot_info_t *)(mbaddr + KERNEL_BASE_ADDRESS);
    g_pMultibootInfo = mbi;
}

void MbCheckMem()
{
    if (g_pMultibootInfo->mem_upper < 15360)
    {
        SwitchMode(0);
        CoInitAsText(&g_debugConsole);
        ILogMsg("XShell has not found enough extended memory. 16Mb of extended "
                "memory is\nrequired to run XShell. You may need to upgrade "
                "your computer.");
        KeStopSystem();
    }
}

void MbCheckCmdLine()
{
    if (strcmp(g_cmdline, "None!") == 0 || g_cmdline[0] == 0)
    {
        ILogMsg("XShell cannot boot, because either:");
        ILogMsg("- no cmdline was passed");
        ILogMsg("- cmdline's address was %x%s", g_pMultibootInfo->cmdline, g_pMultibootInfo->cmdline >= 0x100000 ? " (was bigger than 8 MB)" : "");
        KeStopSystem();
    }
}
