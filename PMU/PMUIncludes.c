#include "keithley.h"
#include "PMU_examples_ulib_internal.h"
BOOL LPTIsInCurrentConfiguration(char* hrid);
void AllocateArrays_SegArbEx(int NumberofSegments);
void FreeArrays_SegArbEx();

double *MeasStart, *MeasStop;
unsigned long *SegTrigger, *MeasType;

#pragma warning( disable: 4996 )
