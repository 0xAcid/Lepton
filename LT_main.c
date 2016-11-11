#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"
#include "pub_tool_libcassert.h"
#include "pub_tool_libcprint.h"
#include "pub_tool_debuginfo.h"
#include "pub_tool_libcbase.h"
#include "pub_tool_options.h"
#include "pub_tool_machine.h"


/* LEPTON - Ax.
 * Lepton is a simple Valgrind tool based on the following tutorial : http://www.valgrind.org/docs/manual/writing-tools.html#writing-tools.writingcode
 * You can find the source code at https://github.com/Axi0m-S/Lepton
 */
 
 
 
// Basic stuff to determine usage of malloc and free
static ULong Malloc_C = 0;
static ULong Free_C = 0; 
static ULong Test_C = 0;

static void Test(void)
{
	Test_C++;
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
 
 
 
 
 
static void LT_post_clo_init(void)
{
	return;
}

static IRSB* LT_instrument ( VgCallbackClosure* closure, IRSB* sbIn, const VexGuestLayout* layout, const VexGuestExtents* vge, const VexArchInfo* archinfo_host, IRType gWordTy, IRType hWordTy )
{
	IRDirty*   di;
	IRStmt* st;
	Int i=0;
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
				if (0 == VG_(strcmp)	(FREE, fnname))
				{
					CountFree();
					di = unsafeIRDirty_0_N(  0, "CountFree",  VG_(fnptr_to_fnentry)( &CountFree ),  mkIRExprVec_0() );
				}
				else if (0 == VG_(strcmp)	(CALLOC,fnname))
				{
					CountCalloc();
					di = unsafeIRDirty_0_N(  0, "CountCalloc",  VG_(fnptr_to_fnentry)( &CountCalloc ),  mkIRExprVec_0() );
				}
				else if (0 == VG_(strcmp)	(MALLOC, fnname))
				{
					CountMalloc();
					di = unsafeIRDirty_0_N(  0, "CountMalloc",  VG_(fnptr_to_fnentry)( &CountMalloc ),  mkIRExprVec_0() );
				}
				else
				{
					continue;
				}
			}
		}
	}	
	return sbIn;
}


static void LT_fini(Int exitcode)
{
	VG_(umsg)("-------- END --------\n");
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
	return;
}

VG_DETERMINE_INTERFACE_VERSION(LT_pre_clo_init)


