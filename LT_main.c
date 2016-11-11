#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"


/* LEPTON - Ax.
 * Lepton is a simple Valgrind tool based on the following tutorial : http://www.valgrind.org/docs/manual/writing-tools.html#writing-tools.writingcode
 * You can find the source code at https://github.com/Axi0m-S/Lepton
 */
static void LT_post_clo_init(void)
{
	return;
}

static IRSB* LT_instrument ( VgCallbackClosure* closure, IRSB* bb, const VexGuestLayout* layout, const VexGuestExtents* vge, const VexArchInfo* archinfo_host, IRType gWordTy, IRType hWordTy )
{
    return bb;
}

static void LT_fini(Int exitcode)
{
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


