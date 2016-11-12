/* Linked list */

// This will be used later
typedef struct FunctionCall_S{
	HChar * FunctionName;
	struct FunctionCall_S *BLink;
	struct FunctionCall_S *FLink;
} FunctionCall;

FunctionCall *CreateFC(FunctionCall *Tail, HChar * FunctionName);
void UpdateTail(FunctionCall * Tail);
FunctionCall *Tail = NULL;
FunctionCall *Head =  NULL;


FunctionCall *CreateFC(FunctionCall *Tail, HChar * FunctionName)
{
		FunctionCall * New = NULL;
		New = (FunctionCall*)VG_(malloc) (1);
		return New;
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
		if (FunctionName)
		{
			/* Function names are always null terminated, no need to worry about strlen or strncpy instead of strcpy */
			New->FunctionName = (FunctionCall*)VG_(malloc) (VG_(strlen)(FunctionName)*sizeof(HChar));
			VG_(strcpy)(New->FunctionName, FunctionName);
		}
		return New;
}

void UpdateTail(FunctionCall * Tail)
{
	Tail = Tail->FLink;
	return;
}

