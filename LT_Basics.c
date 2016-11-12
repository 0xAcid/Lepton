// Basic stuff to determine usage of malloc and free and function calls
static void CountCall(void);
static void CountMalloc(void);
static void CountCalloc(void);
static void CountFree(void);

static ULong Malloc_C = 0;
static ULong Free_C = 0; 
static ULong Call_C = 0;


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