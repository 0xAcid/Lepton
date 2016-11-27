// Basic stuff to determine usage of malloc and free and function calls

#define REPORT_NAME "/var/www/html/Lepton/Reports/Report-x.js"
#include "libvex_guest_amd64.h"
#define NAME_LENGTH 256

/* FAddr is a static variable that is used within the JSON report (it represents the address where a function is being called ) */
static Addr FAddr = 0;
/* FName is a static variable used to store called function names */
static char FName[NAME_LENGTH] = {0};
/* EnableWrite is a static boolean used to get rid of noisy Instructions */
static Bool EnableWrite = False;
/* Init is a static boolean used to start the JSON formatting */
static Bool Init = False;



static void CountCall(void);
static void CountMalloc(void);
static void CountCalloc(void);
static void CountFree(void);
static void CountMemory_Access(void);
static void StoreInstruction(void *addr, Int Size);
static void LoadInstruction(void* addr, Int Size);
static void End(void);
static void DisableWrite();
static void FNAME();


static void printABC(Addr RAX, Addr RBX, Addr RCX);
static void printDBPSP(Addr RDX, Addr RBP, Addr RSP);

static ULong Malloc_C = 0;
static ULong Free_C = 0; 
static ULong Call_C = 0;
static ULong Memory_Access_C = 0;


/* Function that counts memory accesses */
static void CountMemory_Access(void)
{
	Memory_Access_C++;
	return;
}

/* Function that counts Calls  */

static void CountCall(void)
{
	Call_C++;
	return;
}

/* Function that counts Malloc */

static void CountMalloc(void)
{
	Malloc_C++;
	return;
}
/* Function that counts Calloc */

static void CountCalloc(void)
{
	Malloc_C++;
	return;
}

/* Function that counts free */
static void CountFree(void)
{
	Free_C++;
	return;
}


/* Function that is supposed to generate JSON with the following variables :
 * Store
 * Store Address
 * Store Size
 */
static void StoreInstruction(void* addr, Int Size)
{
	if (Init)
	{
		switch (Size)
		{
			/* We need to switch depending on variable size because sometimes we are getting out of range (like reading 16 bytes when only 1 is available).
			 * If we weren't doing the switch stuff we would have error like :
			 ==27048== Process terminating with default action of signal 7 (SIGBUS)
			==27048==  Non-existent physical address at address 0x4FCB000
			==27048==    at 0x401861A: memset (rtld-memset.S:33)
			*/
			case 1:
				VG_(printf)("{\"Store/Load\":\"S\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"%1x\"},", addr, Size, *(unsigned char*)addr);
				break;
			case 2:
				VG_(printf)("{\"Store/Load\":\"S\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"%2x\"},", addr, Size, *(unsigned short*)addr);
				break;
			case 4:
				VG_(printf)("{\"Store/Load\":\"S\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"%4x\"},", addr, Size, *(unsigned int*)addr);
				break;
			case 8:
				VG_(printf)("{\"Store/Load\":\"S\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"%8x\"},", addr, Size, *(unsigned long int*)addr);
				break;
			case 16:
				VG_(printf)("{\"Store/Load\":\"S\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"%8llx%8llx\"},", addr, Size, ((unsigned long long int*)addr)[0], ((unsigned long long int*)addr)[1]);
				break;
			/* A size we cannot handle, never encountered it once but just in case */
			default:
				VG_(printf)("{\"Store/Load\":\"S\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"ERROR\"},", addr, Size, *(unsigned char*)addr);
				break;
		}

	}
}


/* Function that is supposed to generate JSON with the following variables :
 * Load
 * Load Address
 * Load Size
 */
static void LoadInstruction(void* addr, Int Size)
{
	if (Init)
	{
		/* We need to switch depending on variable size because sometimes we are getting out of range (like reading 16 bytes when only 1 is available).
		 * If we weren't doing the switch stuff we would have error like :
		 ==27048== Process terminating with default action of signal 7 (SIGBUS)
		==27048==  Non-existent physical address at address 0x4FCB000
		==27048==    at 0x401861A: memset (rtld-memset.S:33)
		*/
		switch (Size)
		{
			case 1:
				VG_(printf)("{\"Store/Load\":\"L\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"%1x\"},", addr, Size, *(unsigned char*)addr);
				break;
			case 2:
				VG_(printf)("{\"Store/Load\":\"L\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"%2x\"},", addr, Size, *(unsigned short*)addr);
				break;
			case 4:
				VG_(printf)("{\"Store/Load\":\"L\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"%4x\"},", addr, Size, *(unsigned int*)addr);
				break;
			case 8:
				VG_(printf)("{\"Store/Load\":\"L\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"%8x\"},", addr, Size, *(unsigned long int*)addr);
				break;
			case 16:
				VG_(printf)("{\"Store/Load\":\"L\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"%8llx%8llx\"},", addr, Size, ((unsigned long long int*)addr)[0], ((unsigned long long int*)addr)[1]);
				break;
				
			/* A size we cannot handle, never encountered it once but just in case */
			default:
				VG_(printf)("{\"Store/Load\":\"L\", \"Address\":\"%p\", \"Size\":%d,  \"Data\":\"ERROR\"},", addr, Size);
				break;
		}

	}
}



static void DisableWrite()
{
	EnableWrite = False;
}

/* Function that is supposed to generate JSON with the following variables :
 * RAX
 * RBX
 * RCX
 */
static void printABC(Addr RAX, Addr RBX, Addr RCX)
{
	if (EnableWrite)
	{
		VG_(printf)("\"RAX\":\"%p\", \"RBX\":\"%p\", \"RCX\":\"%p\",", RAX, RBX, RCX);
	}
}


/* Function that is supposed to generate JSON with the following variables :
 * RDX
 * RBP
 * RSP
 * MemoryAccess
 */
static void printDBPSP(Addr RDX, Addr RBP, Addr RSP)
{
	if (EnableWrite)
	{
		VG_(printf)("\"RDX\":\"%p\", \"RBP\":\"%p\", \"RSP\":\"%p\", \"Memory Access\":[", RDX, RBP, RSP);
		DisableWrite();
	}
	
}



/* Simple function for JSON formatting */
static printMemoryAccess()
{
	VG_(printf)("\"Memory Access\":[");
}


/* Function that is supposed to generate JSON with the following variables :
 * Function name
 * RIP
 */
static void FNAME()
{
	if (EnableWrite)
	{
		if (!Init)
		{
			VG_(printf)("Report = [{\"Function name\":\"%s\", \"Address/Instruction pointer\":\"%p\",", FName, FAddr);
			Init = True;
		}
		else
		{
			VG_(printf)("]},{\"Function name\":\"%s\", \"Address/Instruction pointer\":\"%p\",", FName, FAddr);
		}
	}
	
}

/* Simple function for JSON formatting */
static void End(void)
{
	VG_(printf)("\r]}];//\t\t");
	return;
}
