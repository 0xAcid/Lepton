#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"
#include "pub_tool_libcassert.h"
#include "pub_tool_libcprint.h"
#include "pub_tool_debuginfo.h"
#include "pub_tool_libcbase.h"
#include "pub_tool_options.h"
#include "pub_tool_machine.h"
#include <stdio.h>
#include <stdlib.h>

#define FUNCTIONS 4096
#define NAME_LENGTH 256
/* LEPTON - Ax.
 * Lepton is a simple Valgrind tool based on the following tutorial : http://www.valgrind.org/docs/manual/writing-tools.html#writing-tools.writingcode
 * You can find the source code at https://github.com/Axi0m-S/Lepton
 */
 

 

HChar FunctionList[FUNCTIONS][NAME_LENGTH] = {0};
/*
// Before using Linked List We neeed to figure out how to use malloc properly.
// There is probably another implementation of calloc by Valgrind so it does not mess with the original program heap

typedef struct FunctionCall_S{
	char * FunctionName;
	struct FunctionCall_S *BLink;
	struct FunctionCall_S *FLink;
} FunctionCall;
FunctionCall *CreateFC(FunctionCall *Tail, char * FunctionName);
void UpdateTail(FunctionCall * Tail);
 


FunctionCall *CreateFC(FunctionCall *Tail, char * FunctionName)
{
		FunctionCall * New = NULL;
		New = (FunctionCall*) malloc (1);
		if (!New)
		{
			return NULL;
		}
		New->FunctionName = NULL;
		New->FLink = NULL;
		New->BLink = Tail;
		if (Tail)
		{
			Tail->FLink = New;
		}
		return New;
}

void UpdateTail(FunctionCall * Tail)
{
	Tail = Tail->FLink;
	return;
}
 */
 
// Basic stuff to determine usage of malloc and free
static ULong Malloc_C = 0;
static ULong Free_C = 0; 
static ULong Test_C = 0;

// static void Test(void)
// {
	// Test_C++;
	// return;
// }

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
 
static Bool Malloc_Free    = True;
static Bool Function_Trace    = False;

 
static Bool LT_Commands(const HChar* arg)
{
   if VG_BOOL_CLO(arg, "--basic-mallocfree", Malloc_Free) {}
   else if VG_BOOL_CLO(arg, "--function-trace", Function_Trace) {}

   else
      return False;
   return True;
}

static void LT_Usage(void)
{  
   VG_(printf)(
"    --basic-mallocfree=no|yes     count free and malloc/calloc [yes]\n"
"    --function-trace=no|yes  trace function calls [no]\n"
   );
}

static void LT_Debug_Usage(void)
{  
   VG_(printf)(
"    (none)\n"
   );
}
 
 
 
 
 
static void LT_post_clo_init(void)
{
	return;
}

static IRSB* LT_instrument ( VgCallbackClosure* closure, IRSB* sbIn, const VexGuestLayout* layout, const VexGuestExtents* vge, const VexArchInfo* archinfo_host, IRType gWordTy, IRType hWordTy )
{
	IRDirty*   di;
	IRStmt* st;
	Int i=0, j=0;
	const HChar FREE[] = "free";
	const HChar MALLOC[] = "malloc";
	const HChar CALLOC[] = "calloc";
	
	
	if (gWordTy != hWordTy) 
	{
      /* We don't currently support this case. */
      VG_(tool_panic)	("host/guest word size mismatch");
   }
   
   
	for (; i < sbIn->stmts_used; i++) 
	{
		st = sbIn->stmts[i];
		/* If empty or No-Operation */
		
		if (!st || st->tag == Ist_NoOp)
		{
		  continue;
		}
		/* If potential function call */
		if (st->tag == Ist_IMark)
		{
			//CountFree();
			const HChar *fnname;
			if (VG_(get_fnname_if_entry)	(st->Ist.IMark.addr, &fnname))
			{
				if (Function_Trace)
				{
						VG_(strncpy)(FunctionList[j], fnname,NAME_LENGTH -1);
				}
				
				if (Malloc_Free)
				{
					
						if (0 == VG_(strcmp)	(FREE, fnname))
						{
							//CountFree();
							di = unsafeIRDirty_0_N(  0, "CountFree",  VG_(fnptr_to_fnentry)( &CountFree ),  mkIRExprVec_0() );
						}
						else if (0 == VG_(strcmp)	(CALLOC,fnname))
						{
							//CountCalloc();
							di = unsafeIRDirty_0_N(  0, "CountCalloc",  VG_(fnptr_to_fnentry)( &CountCalloc ),  mkIRExprVec_0() );
						}
						else if (0 == VG_(strcmp)	(MALLOC, fnname))
						{
							//CountMalloc();
							di = unsafeIRDirty_0_N(  0, "CountMalloc",  VG_(fnptr_to_fnentry)( &CountMalloc ),  mkIRExprVec_0() );
						}
						else
						{
							continue;
						}
				}

				
			}
		}
	}	
	return sbIn;
}


static void LT_fini(Int exitcode)
{
	Int j=0, i=0;
	VG_(umsg)("-------- Report --------\n");
	if (Malloc_Free)
	{
		VG_(umsg)("-------- Malloc and Free usage --------\n");
		if (Malloc_C == Free_C)
		{
			VG_(umsg)("\t[+] Lepton detected the same number of Malloc/Calloc and Free. (%ld)\n", Malloc_C);
		}
		else
		{
			if (Malloc_C > Free_C)
			{
				VG_(umsg)("\t[-] Lepton detected more Malloc/Calloc than free.\n\t\t [*] Malloc/Calloc ===> %ld\n\t\t [*] Free ===> %ld\n", Malloc_C, Free_C);
			}
			else
			{
				VG_(umsg)("\t[-] Lepton detected more free than Malloc/Calloc.\n\t\t [*] Free ===> %ld\n\t\t [*] Malloc/Calloc ===> %ld\n", Malloc_C, Free_C);
			}
		}
		VG_(umsg)("----------------------------------\n");
	}
	
	if (Function_Trace)
	{
		VG_(umsg)("-------- Functions trace --------\n");
		// for (i=0; i < FUNCTIONS; i++)
		// {
			// for (j=0; j < NAME_LENGTH; j++)
			// {
				// if(!FunctionList[i][j])
				// {
					// break;
				// }
				// VG_(umsg)("\t [%d] 0x%x\n", i, FunctionList[i][j]);
			// }
		// }
		for (j=0; j < FUNCTIONS; j++)
		{
			if (! FunctionList[j][0])
			{
				continue;
			}
			VG_(umsg)("\t [%d] %s\n",j, FunctionList[j]);
			// VG_(umsg)("\t [%d]\n",j );
		}
		VG_(umsg)("----------------------------------\n");
	}
	return;
}

static void LT_pre_clo_init(void)
{
	VG_(details_name)	("Lepton");
	VG_(details_version)	(NULL);
	VG_(details_description)	("This tool is made as an exercice for a QuarksLab internship");
	VG_(details_copyright_author)	("Ax.");
	VG_(details_bug_reports_to)	(VG_BUGS_TO);
	VG_(details_avg_translation_sizeB)	( 275 );
	VG_(basic_tool_funcs)	(LT_post_clo_init,  LT_instrument, LT_fini);
	VG_(needs_command_line_options)(LT_Commands, LT_Usage,  LT_Debug_Usage);
	return;
}

VG_DETERMINE_INTERFACE_VERSION(LT_pre_clo_init)


