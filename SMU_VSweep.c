#include "keithley.h"
#include <time.h>

int index,index1;       // 索引
int ASMUId, BSMUId;     // SMU代号
int indexNum_p3,indexNum_p4,indexNum_n3,indexNum_n4;    // 数组大小
char  TempBuf[20] = "";     // 接收getinstattr


double idummy, TempReading1;  // 接收读取的电流
int numberofpoint_p = (int)fabs((Vstop_p - Vstart)/Vstep_p) + 1;    // 计算数据点数
int numberofpoint_n = (int)fabs((Vstop_n - Vstart)/Vstep_n) + 1;    // 计算数据点数
int indexNum_p1 = numberofpoint_p;
int indexNum_p2 = indexNum_p1 + numberofpoint_p;

int indexNum_n1 = numberofpoint_n;
int indexNum_n2 = indexNum_n1 + numberofpoint_n;

// 数组初始化
for (index = 0; index < AVSize; index++)
{
    AV[index] = DBL_NAN;
    AI[index] = DBL_NAN;
    R[index] = DBL_NAN;
}

// 检查输入参数是否合理
if (Vstart > Vstop_p) return -1;
if (Vstart < Vstop_n) return -2;
if (numberofpoint_p < 2) return -3;
if (numberofpoint_n < 2) return -3;
if ((AISize & AVSize) != RSize) return -5;

// 获取SMU属性
getinstid(ASMU, &ASMUId);
getinstattr(ASMUId, "MODELNUM", TempBuf );
if (TempBuf[0] == '\0') return -6;

getinstid(BSMU, &BSMUId);
getinstattr(BSMUId, "MODELNUM", TempBuf );
if (TempBuf[0] == '\0') return -6;


// 设置时间？ 默认一个ACcycle时间
setmode(ASMUId, KI_INTGPLC, nPLC);
if(BSMUId != 4096)
    forcev(BSMUId,0);


//limiti(ASMUId, ComplianceI_p); 限流
switch (RangeI)
    {
        case 1: // auto range
        {
            setauto(ASMUId); //重新启用自动量程并取消指定仪器的任何先前rangeX命令。
            break;
        }
        case 2: // limited auto 1uA
        {
            lorangei(ASMUId, 1e-6);
            break;
        }
        case 3: // limited auto 10uA
        {
            lorangei(ASMUId, 1e-5);
            break;
        }
        case 4: // limited auto 100uA
        {
            lorangei(ASMUId, 1e-4);
            break;
        }
        case 5: // limited auto 1mA
        {
            lorangei(ASMUId, 1e-3);
            break;
        }
        case 6: // limited auto 10mA
        {
            lorangei(ASMUId, 1e-2);
            break;
        }
        default: //limited auto 10mA
        {
            lorangei(ASMUId, 1e-2);
            break;
        }
    }
    
    //active range
    forcev(ASMUId, Vstart);
    delay((int)(Holdtime*1000));  // 延迟等待range改变生效
    intgi(ASMUId, &idummy);     //对于手动量程，rangeX功能用于选择量程。对于自动量程，intgi或intgv函数将在执行测量之前触发所需的范围更改。
    //能够读取电流值
    enable(TIMER1);             // 启动计时器？//

//main loop
if(SweepType == 0)
{
    // 正向循环施加电压读取电流
    for (index1 = 0; index1 < indexNum_p1; index1++)
    {
            AV[index1] = Vstart+(Vstep_p*index1);
            forcev(ASMUId, AV[index1]);
            limiti(ASMUId, ComplianceI_p);
            delay((int)(SweepDelay_p*1000));
            intgi(ASMUId,&TempReading1);        // 读取电流值
            
            if( TempReading1 > 9.9e21){
                   TempReading1 = ComplianceI_p;
                   AI[index1] = TempReading1;
                   indexNum_p1 = index1;
                   indexNum_p2 = 2*index1;
                   Vstop_p = AV[index1-1];
                   break;                   
            }else{
                   AI[index1] = TempReading1;  
            }
            
           R[index1] = AV[index1]/AI[index1];
           PostDataDouble("AV",AV[index1]);
           PostDataDouble("AI",AI[index1]);
           PostDataDouble("R",R[index1]);       // 实时传递数据
    }
        
    for (index1 = indexNum_p1; index1 < indexNum_p2; index1++)
        {
                AV[index1] = Vstop_p-(Vstep_p*(index1-indexNum_p1));
                forcev(ASMUId, AV[index1]);
                delay((int)(SweepDelay_p*1000));
                intgi(ASMUId,&TempReading1);   
                if( TempReading1 > 9.9e21){
                   TempReading1 = ComplianceI_p;
                   AI[index1] = TempReading1;
                }else{
                   AI[index1] = TempReading1;  
                }

                R[index1] = AV[index1]/AI[index1];
                PostDataDouble("AV",AV[index1]);
                PostDataDouble("AI",AI[index1]);
                PostDataDouble("R",R[index1]);
        }

//Backword Sweep
    indexNum_p3 = indexNum_p2 + numberofpoint_n;  // 因为是同一个数据表，避免数据覆盖，从indexNum_p2 开始计数存储
    indexNum_p4 = indexNum_p3 + numberofpoint_n;
    for (index1 = indexNum_p2; index1 < indexNum_p3; index1++)
        {
                AV[index1] = Vstart+(Vstep_n*(index1-indexNum_p2));
                forcev(ASMUId, AV[index1]);
                delay((int)(SweepDelay_n*1000));
                limiti(ASMUId, ComplianceI_n);
                intgi(ASMUId,&TempReading1);
                //printf("Ib %f",TempReading1);
                if( TempReading1 > 9.9e21){
                   TempReading1 = ComplianceI_n;
                   AI[index1] = TempReading1;
                     indexNum_p3 = index1;
                     indexNum_p4 = index1+(index1-indexNum_p2);
                     Vstop_n = AV[index1-1];
                     break;                   
                }else{
                   AI[index1] = TempReading1;  
                }
                     R[index1] = AV[index1]/AI[index1];
                     PostDataDouble("AV",AV[index1]);
                     PostDataDouble("AI",AI[index1]);
                     PostDataDouble("R",R[index1]);
        }
        
    for (index1 = indexNum_p3; index1 < indexNum_p4; index1++)
        {
                AV[index1] = Vstop_n-(Vstep_n*(index1-indexNum_p3));
                forcev(ASMUId, AV[index1]);
                delay((int)(SweepDelay_n*1000));
                intgi(ASMUId,&TempReading1);
                if( TempReading1 > 9.9e21){
                   TempReading1 = ComplianceI_n;
                   AI[index1] = TempReading1;
                }else{
                   AI[index1] = TempReading1;  
                }                

                R[index1] = AV[index1]/AI[index1];
                PostDataDouble("AV",AV[index1]);
                PostDataDouble("AI",AI[index1]);
                PostDataDouble("R",R[index1]);             
        }
}
else
{
    //Backword Sweep 先从负向扫，再从正向扫
    for (index1 = 0; index1 < indexNum_n1; index1++)
        {
                AV[index1] = Vstart+(Vstep_n*index1);
                forcev(ASMUId, AV[index1]);
                limiti(ASMUId, ComplianceI_n);
                delay((int)(SweepDelay_n*1000));
                intgi(ASMUId,&TempReading1);
                if( TempReading1 > 9.9e21){
                   TempReading1 = ComplianceI_n;
                   AI[index1] = TempReading1;
                     indexNum_n1 = index1;
                     indexNum_n2 = 2*index1;
                     Vstop_n = AV[index1-1];
                     break;                   
                }else{
                   AI[index1] = TempReading1;  
                }
                     R[index1] = AV[index1]/AI[index1];
                     PostDataDouble("AV",AV[index1]);
                     PostDataDouble("AI",AI[index1]);
                     PostDataDouble("R",R[index1]);
        }
        
    for (index1 = indexNum_n1; index1 < indexNum_n2; index1++)
        {
                AV[index1] = Vstop_n-(Vstep_n*(index1-indexNum_n1));
                forcev(ASMUId, AV[index1]);
                delay((int)(SweepDelay_n*1000));
                intgi(ASMUId,&TempReading1);
                if( TempReading1 > 9.9e21){
                   TempReading1 = ComplianceI_n;
                   AI[index1] = TempReading1;
                }else{
                   AI[index1] = TempReading1;  
                }                

                R[index1] = AV[index1]/AI[index1];
                PostDataDouble("AV",AV[index1]);
                PostDataDouble("AI",AI[index1]);
                PostDataDouble("R",R[index1]);
        }
        
    //Forward Sweep
    indexNum_n3 = indexNum_n2 + numberofpoint_p;
    indexNum_n4 = indexNum_n3 + numberofpoint_p;
    for (index1 = indexNum_n2; index1 < indexNum_n3; index1++)
        {
                AV[index1] = Vstart+(Vstep_p*(index1-indexNum_n2));
                forcev(ASMUId, AV[index1]);
                limiti(ASMUId, ComplianceI_p);
                delay((int)(SweepDelay_p*1000));
                intgi(ASMUId,&TempReading1);
                if( TempReading1 > 9.9e21){
                   TempReading1 = ComplianceI_p;
                   AI[index1] = TempReading1;
                   indexNum_n3 = index1;
                   indexNum_n4 = index1+(index1-indexNum_n2);
                   Vstop_p = AV[index1-1];
                   break;                   
                }else{
                   AI[index1] = TempReading1;  
                }     
                     R[index1] = AV[index1]/AI[index1];
                     PostDataDouble("AV",AV[index1]);
                     PostDataDouble("AI",AI[index1]);
                     PostDataDouble("R",R[index1]);
        }
        
    for (index1 = indexNum_n3; index1 < indexNum_n4; index1++)
        {
                AV[index1] = Vstop_p-(Vstep_p*(index1-indexNum_n3));
                forcev(ASMUId, AV[index1]);
                delay((int)(SweepDelay_p*1000));
                intgi(ASMUId,&TempReading1);
                if( TempReading1 > 9.9e21){
                   TempReading1 = ComplianceI_p;
                   AI[index1] = TempReading1;
                }else{
                   AI[index1] = TempReading1;  
                }                

                R[index1] = AV[index1]/AI[index1];
                PostDataDouble("AV",AV[index1]);
                PostDataDouble("AI",AI[index1]);
                PostDataDouble("R",R[index1]);             
        }
}

// 恢复至初始状态
devclr();
return 0;
