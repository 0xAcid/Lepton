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
/* This is not really clean, but it's getting messy linking header files with Valgrind */
#include "LT_Basics.c"
#include "LT_LL.c"

#define FUNCTIONS 4096
#define NAME_LENGTH 256
/* LEPTON - Ax.
 * Lepton is a simple Valgrind tool based on the following tutorial : http://www.valgrind.org/docs/manual/writing-tools.html#writing-tools.writingcode
 * You can find the source code at https://github.com/Axi0m-S/Lepton
 */
 
// TO CHECK
// VG_(m_state_static)
// vg_do_register_allocation
 

char FunctionList[FUNCTIONS][NAME_LENGTH];
Addr FunctionListAdd[FUNCTIONS] = {0};


 

 
static Bool Malloc_Free    = True;
static Bool Function_Trace    = False;
static Bool Memory_Access    = False;


 
static Bool LT_Commands(const HChar* arg)
{
   if VG_BOOL_CLO(arg, "--basic-mallocfree", Malloc_Free) {}
   else if VG_BOOL_CLO(arg, "--function-trace", Function_Trace) {}
   else if VG_BOOL_CLO(arg, "--memory-access", Memory_Access) {}

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
	Int i;
	static Int j =0;
	IRDirty* di;
	IRSB* sbOut;
	Addr iaddr = 0;

	if (gWordTy != hWordTy) 
	{
		VG_(tool_panic)("host/guest word size mismatch");
	}
	
	sbOut = deepCopyIRSBExceptStmts(sbIn);

	i = 0;
	while (i < sbIn->stmts_used && sbIn->stmts[i]->tag != Ist_IMark) 
	{
		addStmtToIRSB( sbOut, sbIn->stmts[i] );
		i++;
	}


	for (; i < sbIn->stmts_used; i++) 
	{
		IRStmt* st = sbIn->stmts[i];
		if (!st || st->tag == Ist_NoOp)
		{
			continue;
		}
	
		const HChar *fnname;
		if (st->tag == Ist_WrTmp || st->tag == Ist_Store || st->tag == Ist_StoreG || st->tag == Ist_LoadG)
		{
			if (Memory_Access)
			{
				di = unsafeIRDirty_0_N( 0, "CountMemory_Access", VG_(fnptr_to_fnentry)( &CountMemory_Access ), mkIRExprVec_0() );
				addStmtToIRSB( sbOut, IRStmt_Dirty(di) );
				// CountMemory_Access();
			}
		}
		if (st->tag == Ist_IMark)
		{
			
			if (VG_(get_fnname_if_entry)(st->Ist.IMark.addr, &fnname)) 
			{
				
				di = unsafeIRDirty_0_N( 0, "CountCall", VG_(fnptr_to_fnentry)( &CountCall ), mkIRExprVec_0() );
				addStmtToIRSB( sbOut, IRStmt_Dirty(di) );
				if (Malloc_Free)
				{
					if (0 == VG_(strcmp)(fnname, "malloc"))
					{
						CountMalloc();						
					}
					else if (0 == VG_(strcmp)(fnname, "calloc") )
					{
						CountCalloc();
					}
					else if (0 == VG_(strcmp)(fnname, "free") )
					{
						CountFree();
						
					}
				}
				
				if (Function_Trace)
				{
					FunctionListAdd[j] = st->Ist.IMark.addr;
					VG_(strcpy)(FunctionList[j], fnname); // function name < NAME_LENGTH
					j++;
				}
			}
			
			
			
		}
		addStmtToIRSB( sbOut, st );
	}


	return sbOut;
}






static void LT_fini(Int exitcode)
{
	Int j=0, i=0;
	VG_(umsg)("\n\n\n\n-------- Report --------\n\n");
	VG_(umsg)("\t[+] Lepton detected %ld call(s)\n", Call_C);
	if (Malloc_Free)
	{
		VG_(umsg)("\n-------- Malloc and Free usage --------\n");
		
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
		
		VG_(umsg)("---------------------------------------\n");
	}
	
	if (Function_Trace)
	{
		VG_(umsg)("\n-------- Functions trace (named calls) --------\n");
		for (i=0; i < FUNCTIONS; i++)
		{
			if (!FunctionList[i][0])
			{
				continue;
			}
			if (!VG_(strcmp)(FunctionList[i], "main"))
			{
				VG_(umsg)("\n\n\t\t ----- MAIN START HERE -----\n\n");
			}
			VG_(umsg)("\t[%d] %s @ 0x%x\n", i, FunctionList[i], FunctionListAdd[i]);
		}
		VG_(umsg)("---------------------------------------\n");
	}
	if (Memory_Access)
	{
		VG_(umsg)("\n-------- Memory accesses --------\n");
		VG_(umsg)("\t[+] Lepton detected %ld memory access\n", Memory_Access_C);
		VG_(umsg)("---------------------------------------\n");
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
	// Head = CreateFC(NULL, "LeptonStart");
	// Tail = Head;
	return;
}

VG_DETERMINE_INTERFACE_VERSION(LT_pre_clo_init)


