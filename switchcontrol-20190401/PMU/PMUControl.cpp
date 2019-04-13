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

// ��ʼ��
TotalSegTime = 0.0;
NumTotalSamples = 0;
SampleRate = 200E+6;
RateFactor = 0;
SMUId = 0;
SMUPresent = FALSE;

// ��������Ϊ��
MeasStart = MeasStop = NULL;
SegTrigger = MeasType = NULL;

// ���PMU�Ƿ���ϵͳ����
if (!LPTIsInCurrentConfiguration(PMU_ID)) {
	printf("Instrument %s is not in system configuration", PMU_ID);
	return -1;
}

// ���ص������С�Ƿ��㹻�洢����
if (MaxSheetPoints > VMeasCh1_size || MaxSheetPoints > IMeasCh1_size || MaxSheetPoints > VMeasCh2_size || MaxSheetPoints > IMeasCh2_size || MaxSheetPoints > TimeOutput_size || MaxSheetPoints > StatusCh1_size || MaxSheetPoints > StatusCh2_size) {
	if (verbose)
		printf("One or more Output array size(s) < MaxSheetPoints. Increase size of all Output arrays to be at least %d", MaxSheetPoints);
	sprintf(ErrMsgChar, "One or more Output array size(s) < MaxSheetPoints. Increase size of all Output arrays to be at least %d", MaxSheetPoints)��
	logMsg(MF_Error, ERROR_STRING, ErrMsgChar);
	return ERR_PMU_EXAMPLES_OUTPUT_ARRAY_TOO_SMALL;
}

// ��ȡPMU���ڲ���ϵͳ�еı��
getinstid(PMU_ID, &InstId);
if (-1 == InstId)
	return -2;

// ��飺�Ƿ���SMUID������ǣ���chassis�з�
if (_stricmp(SMU_ID, No_SMU)) {
	if (verbose)
		printf("PMU_Control: SMU string present, %s", SMU_ID);
	if (!LPTIsInCurrentConfiguration(SMU_ID)) {
		printf("PMU_Control: Instrument %s is not in system configuration", PMU_ID);
		return ERR_PMU_EXAMPLES_WRONGCARDID;
	}

	// ��ȡSMUID��ϵͳ�еı��
	getinstid(SMU_ID, &SMUId);
	if (-1 == SMUId)
		return -2;
	SMUPresent = TRUE;
}
else {
	// û�ô���SMU_ID,����β����в�����SMU
	SMUPresent = FAlSE;
	if (verbose)
		ptintf("PMU_SegArb_Example: No SMU specified, SMU_ID = %s", SMU_ID);
}

// ���������С
AllocateArrays_SegArbEx(NumSegments);

// ȷ�� 4225-RPMs ģ�鴦�� pluse ģʽ
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

// ����PMUΪSegment ARBģʽ
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

// ����PMU������ʵֵ��������������ʱ��
status = setmode(InstId, KI_LIM_MODE, KI_VALUE);
if (status) {
	FreeArrays_SegArbEx();
	return status;
}

// 