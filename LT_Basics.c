// Basic stuff to determine usage of malloc and free and function calls
static void CountCall(void);
static void CountMalloc(void);
static void CountCalloc(void);
static void CountFree(void);
static void CountMemory_Access(void);
static void StoreInstruction(Addr addr, Int Size);
static void LoadInstruction(Addr addr, Int Size);
static void NewFunction(Addr addr, HChar * FunctionName);
static void End(void);


static ULong Malloc_C = 0;
static ULong Free_C = 0; 
static ULong Call_C = 0;
static ULong Memory_Access_C = 0;
static Int   Test  =0 ;

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
	VG_(printf)("[\"S\", \"%p\", %d],", addr, Size);
}

static void LoadInstruction(Addr addr, Int Size)
{
	VG_(printf)("[\"L\", \"%p\", %d],", addr, Size);
}

static void NewFunction(Addr addr, HChar * FunctionName)
{
	static Bool Init = False;
	if (!Init)
	{
		VG_(printf)("[\"%s\", \"%p\", [", FunctionName, addr);
	}
	else
	{
		VG_(printf)("]],[\"%s\", \"%p\", [", FunctionName, addr);
	}
	
	
	Init = True;
}

static void End(void)
{
	VG_(printf)("]]\n");
	return;
}