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




/* LEPTON - Ax.
 * Lepton is a simple Valgrind tool based on the following tutorial : http://www.valgrind.org/docs/manual/writing-tools.html#writing-tools.writingcode
 * You can find the source code at https://github.com/Axi0m-S/Lepton
 */
 
// TO CHECK
// VG_(m_state_static)
// vg_do_register_allocation
 




 

 
static Bool Malloc_Free    = True;
static Bool Function_Trace    = True;
static Bool Memory_Access    = True;

 
static Bool LT_Commands(const HChar* arg)
{
   if VG_BOOL_CLO(arg, "--basic-mallocfree", Malloc_Free) {}
   else if VG_BOOL_CLO(arg, "--function-trace", Function_Trace) {} /* Not used anymore, used as default */
   else if VG_BOOL_CLO(arg, "--memory-access", Memory_Access) {} /* Not used anymore, used as default */

   else
      return False;
   return True;
}

static void LT_Usage(void)
{  
   VG_(printf)(
"    --basic-mallocfree=no|yes     count free and malloc/calloc [yes]\n"
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



/*
 * This is the instrumentation function and is the core of the program
 */
static IRSB* LT_instrument ( VgCallbackClosure* closure, IRSB* SuperBlockIn, const VexGuestLayout* layout, const VexGuestExtents* vge, const VexArchInfo* archinfo_host, IRType gWordTy, IRType hWordTy )
{
	Int i;
	IRSB *SuperBlockOut;
	IRDirty* di;
	IRExpr**   argv;
	IRTemp RAX, RBX, RCX, RDX, RSP, RBP, RIP;
	
	/* Init SuperBlockOut */
	SuperBlockOut = deepCopyIRSBExceptStmts(SuperBlockIn);
	
	/* Default mesure if something is wrong with the loaded program */
	if (gWordTy != hWordTy) 
	{
		VG_(tool_panic)("host/guest word size mismatch");
	}
	
	
	
	while (i < SuperBlockIn->stmts_used && SuperBlockIn->stmts[i]->tag != Ist_IMark) 
	{
		addStmtToIRSB( SuperBlockOut, SuperBlockIn->stmts[i] );
		i++;
	}
	
	
	
	
	/* Iterate statements */
	for (; i < SuperBlockIn->stmts_used; i++)
	{
		IRStmt * Statement = SuperBlockIn->stmts[i];
		IRExpr* Data;
		/* If no statement */
		if (!Statement)
		{
			continue;
		}
		
			/* 
			* This switch case is where all the instrumentation is going on.
			* We can use callback function based on the instruction type to define what we need to do.
			*/
			switch (Statement->tag)
			{
				HChar *fnname;
				/* No operation, continue to next iteration */
				case Ist_NoOp:
				
					continue; 
				
				
				/*
				 * Ist_Store : "Write a value to memory"
				 * This is simply a store statement that we will log
				 */
				case Ist_Store:
					argv = mkIRExprVec_2( Statement->Ist.Store.addr, mkIRExpr_HWord(sizeofIRType(typeOfIRExpr(SuperBlockIn->tyenv, Statement->Ist.Store.data))) );
					di   = unsafeIRDirty_0_N( 2,  StoreInstruction, VG_(fnptr_to_fnentry)(StoreInstruction ), argv );
					addStmtToIRSB( SuperBlockOut, IRStmt_Dirty(di) );
					break;
				
				
				/*
				 * Ist_Tmp : Seems to be the only statement where we can find loads. There is also LoadG but we never had something correct with it.
				 * See : "IISWC-2006 - Building Workload Characterization Tool with Valgrind"
				 * This is simply a load statement that we will log
				 */
				case Ist_WrTmp:
					Data = Statement->Ist.WrTmp.data;
					if (Data->tag == Iex_Load)
					{
						
						// argv = mkIRExprVec_1( (Data->Iex.Load.addr));
						// di   = unsafeIRDirty_0_N( 1,  Test, VG_(fnptr_to_fnentry)(Test ), argv );
						// addStmtToIRSB( SuperBlockOut, IRStmt_Dirty(di) );
						// Test(Data);
						argv = mkIRExprVec_2( Data->Iex.Load.addr, mkIRExpr_HWord(sizeofIRType(Data->Iex.Load.ty)) );
						di   = unsafeIRDirty_0_N( 2,  LoadInstruction, VG_(fnptr_to_fnentry)(LoadInstruction ), argv );
						addStmtToIRSB( SuperBlockOut, IRStmt_Dirty(di) );
					}
					
				
					break;


				/*
				* Ist_IMark : "Marks the start of the statements that represent a single machine instruction"
				* We are using IMark to determine whether we can get a function name or not.
				* Based on this we can do a trace of called function.
				* This part may need to be improved with unnammed calls.
				*
				* We also use it to check for calloc/malloc and free, in order to have an idea if the program is well written or if we could find use-after-free etc.
				*/
				
				case Ist_IMark:
				
					if (VG_(get_fnname_if_entry)(Statement->Ist.IMark.addr, &fnname)) 
					{
						CountCall();
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
							
							VG_(strncpy)(FName, fnname,NAME_LENGTH -1);
							FAddr = Statement->Ist.IMark.addr;
							EnableWrite = True;
							
							  // /*  16 */ ULong  guest_RAX;
							  // /*  24 */ ULong  guest_RCX;
							  // /*  32 */ ULong  guest_RDX;
							  // /*  40 */ ULong  guest_RBX;
							  // /*  48 */ ULong  guest_RSP;
							  // /*  56 */ ULong  guest_RBP;
							  // /*  64 */ ULong  guest_RSI;
							  // /*  72 */ ULong  guest_RDI;
							  
							RSP = newIRTemp(SuperBlockOut->tyenv, Ity_I64);
							addStmtToIRSB (SuperBlockOut, IRStmt_WrTmp(RSP, IRExpr_Get( 48, Ity_I64 )) );
							
							
							RBP = newIRTemp(SuperBlockOut->tyenv, Ity_I64);
							addStmtToIRSB (SuperBlockOut, IRStmt_WrTmp(RBP, IRExpr_Get( 56, Ity_I64 )) );
							
							
							RAX = newIRTemp(SuperBlockOut->tyenv, Ity_I64);
							addStmtToIRSB (SuperBlockOut, IRStmt_WrTmp(RAX, IRExpr_Get( 16, Ity_I64 )) );
							
							
							RBX = newIRTemp(SuperBlockOut->tyenv, Ity_I64);
							addStmtToIRSB (SuperBlockOut, IRStmt_WrTmp(RBX, IRExpr_Get( 40, Ity_I64 )) );
							
							
							RCX = newIRTemp(SuperBlockOut->tyenv, Ity_I64);
							addStmtToIRSB (SuperBlockOut, IRStmt_WrTmp(RCX, IRExpr_Get( 24, Ity_I64 )) );
							
							
							RDX = newIRTemp(SuperBlockOut->tyenv, Ity_I64);
							addStmtToIRSB (SuperBlockOut, IRStmt_WrTmp(RDX, IRExpr_Get( 32, Ity_I64 )) );
							
							RIP = newIRTemp(SuperBlockOut->tyenv, Ity_I64);
							addStmtToIRSB (SuperBlockOut, IRStmt_WrTmp(RIP, IRExpr_Get( 184, Ity_I64 )) );
							
							
							/* Call to FNAME */
							di   = unsafeIRDirty_0_N( 0,  FNAME, VG_(fnptr_to_fnentry)(FNAME ), mkIRExprVec_0() );
							addStmtToIRSB( SuperBlockOut, IRStmt_Dirty(di) );
							
							/* Call to printABC */
							argv = mkIRExprVec_3(  IRExpr_RdTmp(RAX),  IRExpr_RdTmp(RBX), IRExpr_RdTmp(RCX)   );
							di   = unsafeIRDirty_0_N( 3,  printABC, VG_(fnptr_to_fnentry)(printABC ), argv );
							addStmtToIRSB( SuperBlockOut, IRStmt_Dirty(di) );
							
							/* Call to printDBPSP */
							argv = mkIRExprVec_3(  IRExpr_RdTmp(RDX),  IRExpr_RdTmp(RBP), IRExpr_RdTmp(RSP)   );
							di   = unsafeIRDirty_0_N( 3,  printDBPSP, VG_(fnptr_to_fnentry)(printDBPSP ), argv );
							addStmtToIRSB( SuperBlockOut, IRStmt_Dirty(di) );	
						}

					}
				
				break;
				
					default:
				break;
			}
		
		
		
		addStmtToIRSB(SuperBlockOut, Statement);

	}
	

	return SuperBlockOut;
}


static void LT_fini(Int exitcode)
{
	Int j=0, i=0;
	/* JSON formatting */
	End();
	
	/* Reporting stuff */
	VG_(umsg)("\r//\t\t\r//\t\t\r//\t\t\r//\t\t-------- Report --------\r//\t\t\r//\t\t");
	VG_(umsg)("\t[+] Lepton detected %ld call(s)\r//\t\t", Call_C);
	if (Malloc_Free)
	{
		VG_(umsg)("\r//\t\t-------- Malloc and Free usage --------\r//\t\t");
		
		if (Malloc_C == Free_C)
		{
			VG_(umsg)("\t[+] Lepton detected the same number of Malloc/Calloc and Free. (%ld)\r//\t\t", Malloc_C);
		}
		else
		{
			if (Malloc_C > Free_C)
			{
				VG_(umsg)("\t[-] Lepton detected more Malloc/Calloc than free.\r//\t\t\t\t [*] Malloc/Calloc ===> %ld\r//\t\t\t\t [*] Free ===> %ld\r//\t\t", Malloc_C, Free_C);
			}
			else
			{
				VG_(umsg)("\t[-] Lepton detected more free than Malloc/Calloc.\r//\t\t\t\t [*] Free ===> %ld\r//\t\t\t\t [*] Malloc/Calloc ===> %ld\r//\t\t", Malloc_C, Free_C);
			}
		}
		
		VG_(umsg)("---------------------------------------\r//\t\t");
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


