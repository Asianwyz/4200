int status, i;
int InstId, SMUId;
boolean SMUPresent;
int verbose = 0;
char ErrMsgChar[150];
double NumTotalSamples;
int RateFactor;
long SeqList[1];
double t;
double LoopCountList[1];
double TotalSegTime, SampleRate;
char ermessage[100];
char No_SMU[10] = "NONE";

// 初始化
TotalSegTime = 0.0;
NumTotalSamples = 0;
SampleRate = 200E+6;
RateFactor = 0;
SMUId = 0;
SMUPresent = FALSE;

// 设置数组为空
MeasStart = MeasStop = NULL;
SegTrigger = MeasType = NULL;

// 检查PMU是否与系统连接
if (!LPTIsInCurrentConfiguration(PMU_ID)) {
	printf("Instrument %s is not in system configuration", PMU_ID);
	return -1;
}

// 返回的数组大小是否足够存储数据
if (MaxSheetPoints > VMeasCh1_size || MaxSheetPoints > IMeasCh1_size || MaxSheetPoints > VMeasCh2_size || MaxSheetPoints > IMeasCh2_size || MaxSheetPoints > TimeOutput_size || MaxSheetPoints > StatusCh1_size || MaxSheetPoints > StatusCh2_size) {
	if (verbose)
		printf("One or more Output array size(s) < MaxSheetPoints. Increase size of all Output arrays to be at least %d", MaxSheetPoints);
	sprintf(ErrMsgChar, "One or more Output array size(s) < MaxSheetPoints. Increase size of all Output arrays to be at least %d", MaxSheetPoints)；
	logMsg(MF_Error, ERROR_STRING, ErrMsgChar);
	return ERR_PMU_EXAMPLES_OUTPUT_ARRAY_TOO_SMALL;
}

// 获取PMU的在测试系统中的编号
getinstid(PMU_ID, &InstId);
if (-1 == InstId)
	return -2;

// 检查：是否有SMUID，如果是，在chassis中否？
if (_stricmp(SMU_ID, No_SMU)) {
	if (verbose)
		printf("PMU_Control: SMU string present, %s", SMU_ID);
	if (!LPTIsInCurrentConfiguration(SMU_ID)) {
		printf("PMU_Control: Instrument %s is not in system configuration", PMU_ID);
		return ERR_PMU_EXAMPLES_WRONGCARDID;
	}

	// 获取SMUID在系统中的编号
	getinstid(SMU_ID, &SMUId);
	if (-1 == SMUId)
		return -2;
	SMUPresent = TRUE;
}
else {
	// 没用传入SMU_ID,在这次测试中不适用SMU
	SMUPresent = FAlSE;
	if (verbose)
		ptintf("PMU_SegArb_Example: No SMU specified, SMU_ID = %s", SMU_ID);
}

// 设置数组大小
AllocateArrays_SegArbEx(NumSegments);

// 确认 4225-RPMs 模块处于 pluse 模式
status = rpm_config(InstId, 1, KI_RPM_PATHWAY, KI_RPM_PULSE);
if (status) {
	FreeArrays_SegArbEx();
	return status;
}
status = rpm_config(InstId, 2, KI_RPM_PATHWAY, KI_RPM_PULSE);
if (status) {
	FreeArrays_SegArbEx();
	return status;
}

// 设置PMU为Segment ARB模式
status = pg2_init(InstId, PULSE_MODE_SARB);
if (status) {
	printf("SegArb_EX Error: Pg2_init status = %d, PULSE_MODE_SARB=%d", status, PULSE_MODE_SARB);
	FreeArrays_SegArbEx();
	return status;
}

if (SMUPresent) {
	if (verbose) {
		printf("SegArb_Ex : SMU present, V = %g, Irange = %g", SMU_V, SMU_Irange);

		status = rangei(SMUId, SMU_Irange);
		if (status) {
			FreeArrays_SegArbEx();
			return status;
		}

		status = limiti(SMUId, SMU_Icomp);
		if (status) {
			FreeArrays_SegArbEx();
			return status;
		}

		status = forcev(SMUId, SMU_V);
		if (status) {
			FreeArrays_SegArbEx();
			return status;
		}
	}
}

// 设置PMU返回真实值，当测试溢流的时候
status = setmode(InstId, KI_LIM_MODE, KI_VALUE);
if (status) {
	FreeArrays_SegArbEx();
	return status;
}

// 