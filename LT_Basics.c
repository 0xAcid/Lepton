// Basic stuff to determine usage of malloc and free and function calls

#define REPORT_NAME "/var/www/html/Lepton/Reports/Report-x.js"
#include "libvex_guest_amd64.h"

#define FUNCTIONS 4096
#define NAME_LENGTH 256
static char FunctionList[FUNCTIONS][NAME_LENGTH];
static Addr FunctionListAdd[FUNCTIONS] = {0};
static Int CallCounter = 0;
static char FName[NAME_LENGTH] = {0};
static Addr FAddr = 0;
static Bool EnableWrite = False;
static Bool Init = False;



static void CountCall(void);
static void CountMalloc(void);
static void CountCalloc(void);
static void CountFree(void);
static void CountMemory_Access(void);
static void StoreInstruction(Addr addr, Int Size);
static void LoadInstruction(Addr addr, Int Size);
static void NewFunction(Addr addr, HChar * FunctionName,Addr RAX, Addr RBX, Addr RCX,Addr RDX, Addr RBP, Addr RSP, IRSB *SuperBlockOut );
static void End(void);

static void printABC(Addr RAX, Addr RBX, Addr RCX);
static void printDBPSP(Addr RDX, Addr RBP, Addr RSP);

static ULong Malloc_C = 0;
static ULong Free_C = 0; 
static ULong Call_C = 0;
static ULong Memory_Access_C = 0;
static Int   Test  =0 ;
// static FILE * Report = NULL;




static void PRINTER(HChar *string)
{
	VG_(printf)(string);
	return;
}

static void IsStore(Addr addr)
{
	VG_(printf)("[STORE] %p\n", addr);
	return;
}

static void IsLoad(Addr addr)
{
	VG_(printf)("[LOAD] %p\n", addr);
	return;
}

static void CountMemory_Access(void)
{
	Memory_Access_C++;
	//VG_(printf)("x");
	return;
}

static void CountCall(void)
{
	Call_C++;
	return;
}

static void CountMalloc(void)
{
	Malloc_C++;
	return;
}
static void CountCalloc(void)
{
	Malloc_C++;
	return;
}

static void CountFree(void)
{
	Free_C++;
	return;
}


static void StoreInstruction(Addr addr, Int Size)
{
	if (Init)
	{
		VG_(printf)("{\"Store/Load\":\"S\", \"Address\":\"%p\", \"Size\":%d},", addr, Size);
	}
	// VG_(fprintf) (Report, "[\"S\", \"%p\", %d],", addr, Size);
}

static void LoadInstruction(Addr addr, Int Size)
{
	if (Init)
	{
		VG_(printf)("{\"Store/Load\":\"L\", \"Address\":\"%p\", \"Size\":%d},", addr, Size);
	}
	// VG_(fprintf) (Report, "[\"L\", \"%p\", %d],", addr, Size);
}

static void NewFunction(Addr addr, HChar * FunctionName,Addr RAX, Addr RBX, Addr RCX,Addr RDX, Addr RBP, Addr RSP, IRSB *SuperBlockOut )
{
	static Bool Init = False;
	IRExpr**   argv;
	IRDirty* di;
	if (!Init)
	{
		VG_(printf)("Report = [{\"Function name\":\"%s\", \"Address/Instruction pointer\":\"%p\",  \"Memory Access\":[", FunctionName, addr);
		// VG_(printf)("Report = [{\"Function name\":\"%s\", \"RIP\":\"%p\", \"RBP\":\"%p\", \"RSP\":\"%p\", \"RAX\":\"%p\", \"RBX\":\"%p\", \"RCX\":\"%p\", \"RDX\":\"%p\", \"Memory Access\":[", FunctionName, addr,RBP, RSP, RAX, RBX, RCX, RDX);

		// VG_(fprintf) (Report, "[\"%s\", \"%p\", [", FunctionName, addr);
	}
	else
	{
		VG_(printf)("]},{\"Function name\":\"%s\", \"Address/Instruction pointer\":\"%p\",", FunctionName, addr);
		// VG_(printf)("]},{\"Function name\":\"%s\", \"RIP\":\"%p\", \"RBP\":\"%p\", \"RSP\":\"%p\", \"RAX\":\"%p\", \"RBX\":\"%p\", \"RCX\":\"%p\", \"RDX\":\"%p\", \"Memory Access\":[", FunctionName, addr,RBP, RSP, RAX, RBX, RCX, RDX);

		// VG_(fprintf) (Report, "]],[\"%s\", \"%p\", [", FunctionName, addr);
	}
	argv = mkIRExprVec_3(  IRExpr_RdTmp(RAX),  IRExpr_RdTmp(RBX), IRExpr_RdTmp(RCX)   );
	di   = unsafeIRDirty_0_N( 3,  printABC, VG_(fnptr_to_fnentry)(printABC ), argv );
	addStmtToIRSB( SuperBlockOut, IRStmt_Dirty(di) );
	
	
	argv = mkIRExprVec_3(  IRExpr_RdTmp(RDX),  IRExpr_RdTmp(RBP), IRExpr_RdTmp(RSP)   );
	di   = unsafeIRDirty_0_N( 3,  printDBPSP, VG_(fnptr_to_fnentry)(printDBPSP ), argv );
	addStmtToIRSB( SuperBlockOut, IRStmt_Dirty(di) );
	Init = True;
}


static void DisableWrite()
{
	EnableWrite = False;
}

static void printABC(Addr RAX, Addr RBX, Addr RCX)
{
	if (EnableWrite)
	{
		VG_(printf)("\"RAX\":\"%p\", \"RBX\":\"%p\", \"RCX\":\"%p\",", RAX, RBX, RCX);
	}
}

static void printDBPSP(Addr RDX, Addr RBP, Addr RSP)
{
	if (EnableWrite)
	{
		VG_(printf)("\"RDX\":\"%p\", \"RBP\":\"%p\", \"RSP\":\"%p\", \"Memory Access\":[", RDX, RBP, RSP);
		DisableWrite();
	}
	
}




static printMemoryAccess()
{
	VG_(printf)("\"Memory Access\":[");
}



static void PrintREG(Int RAX, Int RBX, Int RCX, Int RDX, Int RSP, Int RBP)
{
	VG_(printf)("RAX : %p\nRBX : %p\nRCX : %p\nRDX : %p\nRSI : %p\nRDI : %p\nRSP : %p\nRBP : %p\n\n\n", RAX, RBX, RCX, RDX, RSP, RBP);
	
}

static void printX(Addr Test)
{
	VG_(printf)("IRExpr_Get( 184, gWordTy ) : %p \n", Test);
}

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

static void End(void)
{
	VG_(printf)("\r]}];//\t\t");
	return;
}