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

    //Initialize variables
//    verbose = 1;        //prints out status messages to Message Console (type msgcon at command prompt)
    TotalSegTime = 0.0;
    NumTotalSamples = 0;
    SampleRate = 200E+6;
    RateFactor = 0;
    SMUId = 0;
    SMUPresent = FALSE;

    //Set all global arrays to null
    MeasStart = MeasStop = NULL;
    SegTrigger = MeasType = NULL;

    //Check to see if requested PMU is in system
    if ( !LPTIsInCurrentConfiguration(PMU_ID) )
    {
        printf("Instrument %s is not in system configuration", PMU_ID);
        return -1;
    }

    //Determine if return array sizes are big enough to contain
    //the desired number of rows (from MaxSheetPoints)
    if (MaxSheetPoints > VMeasCh1_size || MaxSheetPoints > IMeasCh1_size || MaxSheetPoints > VMeasCh2_size || MaxSheetPoints > IMeasCh2_size || MaxSheetPoints >  TimeOutput_size || MaxSheetPoints > StatusCh1_size  || MaxSheetPoints > StatusCh2_size)
    {
        if (verbose)
            printf("One or more Output array size(s) < MaxSheetPoints. Increase size of all Output arrays to be at least %d", MaxSheetPoints);
        sprintf(ErrMsgChar, "One or more Output array size(s) < MaxSheetPoints. Increase size of all Output arrays to be at least %d.", MaxSheetPoints);
        logMsg(MF_Error, ERROR_STRING, ErrMsgChar); 
        return ERR_PMU_EXAMPLES_OUTPUT_ARRAY_TOO_SMALL;
    }

    //Get internal handle for PMU        
    getinstid(PMU_ID, &InstId);
    if ( -1 == InstId )
        return -2;

    //Check: is a SMU ID set (SMUx or NONE)?  If a SMU string, is in the the chassis?
    if ( _stricmp (SMU_ID, No_SMU) )
    {
        if ( verbose )
            printf("PMU_SegArb_Example: SMU string present, %s", SMU_ID);

        if ( !LPTIsInCurrentConfiguration(SMU_ID) )
        {
            printf("PMU_SegArb_Example: Instrument %s is not in system configuration", PMU_ID);
            return ERR_PMU_EXAMPLES_WRONGCARDID;
        }

        //Convert SMU card string into identifying instrument card number
        getinstid(SMU_ID, &SMUId);
        if ( -1 == SMUId )
            return -2;
        SMUPresent = TRUE;
    }
    else    //NONE specified, do not use SMU in this test
    {
        SMUPresent = FALSE;
        if ( verbose )
            printf("PMU_SegArb_Example: No SMU specified, SMU_ID = %s", SMU_ID);
    }

    //Set size of arrays
    AllocateArrays_SegArbEx(NumSegments);

    //Ensure that 4225-RPMs (if attached) are in the pulse mode
    status = rpm_config(InstId, 1, KI_RPM_PATHWAY, KI_RPM_PULSE);
    if ( status )
    {
        FreeArrays_SegArbEx();        
        return status ;
    }

    status = rpm_config(InstId, 2, KI_RPM_PATHWAY, KI_RPM_PULSE);
    if ( status )
    {
        FreeArrays_SegArbEx();        
        return status ;
    }


    //Set PMU into Segment ARB mode
    status = pg2_init(InstId, PULSE_MODE_SARB);
    if ( status )
    {
        printf("SegArb_Ex Error: Pg2_init status= %d, PULSE_MODE_SARB=%d", status, PULSE_MODE_SARB);
        FreeArrays_SegArbEx();
        return status ;
    }

    if ( SMUPresent )
    {
        if ( verbose )
            printf("SegArb_Ex: SMU present, V= %g, Irange= %g", SMU_V, SMU_Irange);

        status = rangei(SMUId, SMU_Irange);
        if ( status )
        {
            FreeArrays_SegArbEx();        
            return status ;
        }

        status = limiti(SMUId, SMU_Icomp);
        if ( status )
        {
            FreeArrays_SegArbEx();        
            return status ;
        }


        status = forcev(SMUId, SMU_V);
        if ( status )
        {
            FreeArrays_SegArbEx();        
            return status ;
        }
    }

    //Set PMU to return actual values when measurement overflow occurs
    status = setmode(InstId, KI_LIM_MODE, KI_VALUE);
    if ( status )
    {
        FreeArrays_SegArbEx();        
        return status ;
    }

    //Program PMU with resistance of connected DUT 
    status = pulse_load(InstId, 1, DUTResCh1);
    if ( status )
    {
        printf("SegArb_Ex Error: Ch1 pulse_load status= %d", status);
        FreeArrays_SegArbEx();        
        return status ;
    }

    //Program PMU with resistance of connected DUT 
    status = pulse_load(InstId, 2, DUTResCh2);
    if ( status )
    {
        printf("SegArb_Ex Error: Ch2 pulse_load status= %d", status);
        FreeArrays_SegArbEx();        
        return status ;
    }


    //Program the PMU voltage source and current measure ranges
    status = pulse_ranges(InstId, 1, VRangeCh1, PULSE_MEAS_FIXED, VRangeCh1, PULSE_MEAS_FIXED, IRangeCh1);
    if ( status )
    {
        printf("SegArb_Ex Error: Ch1 pulse_ranges status= %d", status);
        FreeArrays_SegArbEx();        
        return status ;
    }

    status = pulse_ranges(InstId, 2, VRangeCh2, PULSE_MEAS_FIXED, VRangeCh2, PULSE_MEAS_FIXED, IRangeCh2);
    if ( status )
    {
        logMsgGet(ermessage, status);
        printf("SegArb_Ex: logMsgGet, string= %c20, status= %d", ermessage, status);
        printf("SegArb_Ex: Ch2 pulse_ranges status= %d", status);
        FreeArrays_SegArbEx();        
        return status ;
    }
    
    //Calculate and set the sample rate for the PMU
    //Calculate total Segment ARB time
    for(i=1; i<NumSegments; i++)
        TotalSegTime += SegTime[i];        
    if (verbose)
        printf("SegArb_Ex: TotalSegTime = %g, SampleRate= %g", TotalSegTime, SampleRate);

    //Calculate number of samples (rows in sheet)
    NumTotalSamples = (TotalSegTime * NumWaveforms * SampleRate);    

    if (verbose)
        printf("SegArb_Ex: TotalSegTime = %g, NumTotalSamples= %g, MaxSheetPts= %d", TotalSegTime, NumTotalSamples, MaxSheetPoints);

    //if total samples for the test > MaxSamplesPerAtoD, then set sample_rate to lower value
    if ((NumTotalSamples) > MaxSamplesPerAtoD)
    {
        RateFactor = (int)((NumTotalSamples / MaxSamplesPerAtoD) + 1);      
        SampleRate = (SampleRate / RateFactor);
        if ( verbose )
            printf("SegArb_Ex: NumSamplesTimeCaptured (%g) > MaxSamplesPerAtoD (%d), Ratefactor= %d", NumTotalSamples, MaxSamplesPerAtoD, RateFactor);
    }

    //If number of samples is greater than the max number of rows in the sheet, set a lower sample rate
    if (NumTotalSamples > MaxSheetPoints)
    {
        RateFactor = (int)((NumTotalSamples / MaxSheetPoints) + 1);      
        SampleRate = (SampleRate / RateFactor);
        if (verbose)
            printf("SegArb_Ex:  NumSamples > MaxsheetPts, Ratefactor= %d", RateFactor);
    }

    if (verbose)
        printf("NumTotalSamples= %g, MaxSheetPts= %d, SampleRate= %g", NumTotalSamples, MaxSheetPoints, SampleRate);

    NumTotalSamples = (int)(TotalSegTime * SampleRate);    
    if (verbose)
        printf("SegArb_Ex: Using new sample rate, NumTotalSamples= %g, SampleRate= %g", NumTotalSamples, SampleRate);

    status = pulse_sample_rate(InstId, (long)SampleRate);
    if (verbose)
        printf("SegArb_Ex: sample_rate status= %d", status);
    if ( status )
    {
        printf("SegArb_Ex Error: sample_rate status= %d", status);
        FreeArrays_SegArbEx();        
        return status;
    }

    //Program number of Segment ARB waveforms to output
    status = pulse_burst_count(InstId, 1, 1);
    if ( status )
    {
        printf("SegArb_Ex Error: Ch1 burst count status= %d", status);
        FreeArrays_SegArbEx();        
        return status ;
    }

    status = pulse_burst_count(InstId, 2, 1);
    if ( status )
    {
        printf("SegArb_Ex Error: Ch2 burst count status= %d", status);
        FreeArrays_SegArbEx();        
        return status ;
    }

    //Set Array names so that data can be automatically returned to the data sheet
    status = pulse_measrt(InstId, 1, "VMeasCh1", "IMeasCh1", "TimeOutput", NULL);
    if ( status )
    {
        FreeArrays_SegArbEx();        
        return status ;
    }
       
    status = pulse_measrt(InstId, 2, "VMeasCh2", "IMeasCh2", "", NULL);
    if ( status )
    {
        FreeArrays_SegArbEx();        
        return status ;
    }
   
    //Fill Trigger and MeasType array for Seg-Arb.  Ensure that first entry in Trigger Out array is 1.
    i = 0;
    SegTrigOut[i] = 1;

    //Populate Sequence List, only 1 sequence in this example
    SeqList[i] = 1;

    //Set number of waveforms to output.  Use Sequence Looping (instead of pulse_burst).
    LoopCountList[0] =  NumWaveforms;   

    //Fill Measure Type, start and stop arrays for the sequences
    for(i=0; i<NumSegments; i++)
    {
        MeasType[i] = PULSE_MEAS_WFM_PER;
        MeasStart[i] = 0.0;
        MeasStop[i] = SegTime[i];
    }

    //Program Seg-Arb Sequences (1 per channel)
    status = seg_arb_sequence(InstId,  1, 1,  NumSegments, StartVCh1, StopVCh1, SegTime, SegTrigOut, SSRCtrlCh1, MeasType, MeasStart, MeasStop);
    if ( status )
    {
        printf("SegArb_Ex Error: Ch1 segarb sequence status= %d", status);
        FreeArrays_SegArbEx();                
        return status;
    }
               
    status = seg_arb_sequence(InstId,  2, 1,  NumSegments, StartVCh2, StopVCh2, SegTime, SegTrigOut, SSRCtrlCh2, MeasType, MeasStart, MeasStop);
    if ( status )
    {
        printf("SegArb_Ex Error: Ch2 segarb sequence status= %d", status);
        FreeArrays_SegArbEx();        
        return status;
    }
    
    //Program Seg-Arb Waveform (1 per channel)
    status = seg_arb_waveform(InstId, 1, 1, SeqList, LoopCountList);
    if ( status )
    {
        printf("SegArb_Ex Error: Ch1 segarb waveform status= %d", status);
        FreeArrays_SegArbEx();        
        return status;
    }

    status = seg_arb_waveform(InstId, 2, 1, SeqList, LoopCountList);
    if ( status )
    {
        printf("SegArb_Ex Error: Ch2 segarb waveform status= %d", status);
        FreeArrays_SegArbEx();        
        return status;
    }

    //Turn on outputs    
    status = pulse_output(InstId, 1, 1);
    if ( status )
    {
        FreeArrays_SegArbEx();        
        return status ;
    }
    status = pulse_output(InstId, 2, 1);
    if ( status )
    {
        printf("SegArb_Ex Error: attemping to run pulse_exec, status= %d", status);
        FreeArrays_SegArbEx();        
        return status ;
    }

    if (verbose)
        printf("SegArb_Ex:  Just before pulse_exec0");

    //Run test:  output Seg-Arb waveform while measuring
    status = pulse_exec(PULSE_MODE_SIMPLE);
    if ( status )
    {
        printf("SegArb_Ex Error: attemping to run pulse_exec, status= %d", status);
        FreeArrays_SegArbEx();        
        return status ;
    }

    if (verbose)
        printf("SegArb_Ex:  Just after pulse_exec");

    //Wait for test to complete
    while ( pulse_exec_status(&t) == 1 )
    {
        Sleep(100);
    }

    //NOTE:  No need to fetch data because pulse_measrt was configured before test execution
    //to automatically tranfer data to KITE sheet (NOTE:  pulse_measrt is not compatible with KXCI)

    if ( SMUPresent )
    {
        if ( verbose )
            printf("SegArb_Ex: SMU present, setting voltage = 0");

        status = forcev(SMUId, 0);
        if ( status )
        {
            FreeArrays_SegArbEx();        
            return status ;
        }
    }

    FreeArrays_SegArbEx();
    return 0;

}

void AllocateArrays_SegArbEx(int NumberofSegments)
{
    //Allocate arrays for Seg-arb: trigger, measure type, measure start, measure stop.  
    SegTrigger = (long *)calloc(NumberofSegments, sizeof(long));
    MeasType = (long *)calloc(NumberofSegments, sizeof(long));
    MeasStart = (double *)calloc(NumberofSegments, sizeof(double));
    MeasStop = (double *)calloc(NumberofSegments, sizeof(double));

}
void FreeArrays_SegArbEx()
{
    //Free memory for arrays before exiting UTM
    if (SegTrigger != NULL)
        free(SegTrigger);
    if (MeasType != NULL)
        free(MeasType);
    if (MeasStart != NULL)
        free(MeasStart);
    if (MeasStop != NULL)
        free(MeasStop);

