// Basic stuff to determine usage of malloc and free and function calls

#define REPORT_NAME "/var/www/html/Lepton/Reports/Report-x.js"
static void CountCall(void);
static void CountMalloc(void);
static void CountCalloc(void);
static void CountFree(void);
static void CountMemory_Access(void);
static void StoreInstruction(Addr addr, Int Size);
static void LoadInstruction(Addr addr, Int Size);
static void NewFunction(Addr addr, HChar * FunctionName, VexGuestLayout * layout);
static void End(void);


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
	VG_(printf)("{\"Store/Load\":\"S\", \"Address\":\"%p\", \"Size\":%d},", addr, Size);
	// VG_(fprintf) (Report, "[\"S\", \"%p\", %d],", addr, Size);
}

static void LoadInstruction(Addr addr, Int Size)
{
	VG_(printf)("{\"Store/Load\":\"L\", \"Address\":\"%p\", \"Size\":%d},", addr, Size);
	// VG_(fprintf) (Report, "[\"L\", \"%p\", %d],", addr, Size);
}

static void NewFunction(Addr addr, HChar * FunctionName, VexGuestLayout * layout)
{
	static Bool Init = False;
	if (!Init)
	{
		VG_(printf)("Report = [{\"Function name\":\"%s\", \"Address/Instruction pointer\":\"%p\", \"Stack Pointer\":\"%p\", \"Frame Pointer\":\"%p\", \"Memory Access\":[", FunctionName, addr, layout->offset_SP, layout->offset_FP);
		// VG_(fprintf) (Report, "[\"%s\", \"%p\", [", FunctionName, addr);
	}
	else
	{
		VG_(printf)("]},{\"Function name\":\"%s\", \"Address/Instruction pointer\":\"%p\", \"Stack Pointer\":\"%p\", \"Frame Pointer\":\"%p\", \"Memory Access\":[", FunctionName, addr, layout->offset_SP, layout->offset_FP);
		// VG_(fprintf) (Report, "]],[\"%s\", \"%p\", [", FunctionName, addr);
	}
	
	
	Init = True;
}

static void PrintREG(VexGuestAMD64State REG)
{
	VG_(printf)("RAX : %p\nRBX : %p\nRCX : %p\nRDX : %p\nRSI : %p\nRDI : %p\nRBP : %p\nRSP : %p\n\n\n", REG.guest_RAX, REG.guest_RBX, REG.guest_RCX, REG.guest_RDX, REG.guest_RSI, REG.guest_RDI, REG.guest_RBP, REG.guest_RSP);
	
}

static void printX(Int R)
{
	VG_(printf)("Test : %p\n", R);
}

static void End(void)
{
	VG_(printf)("\r]}];//\t\t");
	return;
}