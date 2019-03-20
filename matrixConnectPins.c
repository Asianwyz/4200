#include "keithley.h"
#include "ulib_internal.h"

int TermId1, TermId2, TermId3, TermId4;
int TermId5, TermId6, TermId7, TermId8;						// 探针ID ？
int PinCount = 0;											// 探针总数

char CountStr[8] = "0";										// 探针总数字符串，之后转换成整形
char MatrixStr[10] = "";									// 矩阵属性字符串
char TempBuf[20] = "";										// 仪器属性字符串

/* 检查是否存在开关矩阵 */

getinstattr(MATRIX1, "MODELNUM", MatrixStr);				// 给定仪器标识代码和属性名称字符串，此模块返回指定的属性值字符串
if (MatrixStr == 0x00) 
	return (NO_SWITCH_MATRIX);


// 查看测试系统有多少探针
getinstattr(PROBER1, "NUMOFPINS", CountStr);
PinCount = atoi(CountStr);
if (PinCount <= 0)
	return (NO_MATRIX_CARDS);

// 验证探针 ?????  Pin1 大小表示所代表的引脚？
if ((Pin1 > PinCount) || (Pin2 > PinCount) || (Pin3 > PinCount) ||
	(Pin4 > PinCount) || (Pin5 > PinCount) || (Pin6 > PinCount) ||
	(Pin7 > PinCount) || (Pin8 > PinCount) ) 
	return (INVAL_PIN_SPEC);

#define MAX_PINS 72
if ((Pin1 > MAX_PINS) || (Pin2 > MAX_PINS) || (Pin3 > MAX_PINS) ||
	(Pin4 > MAX_PINS) || (Pin5 > MAX_PINS) || (Pin6 > MAX_PINS) ||
	(Pin7 > MAX_PINS) || (Pin8 > MAX_PINS) ) 
	return (INVAL_PARAM);

if (OpenAll)												// 如果值为1，清除之前所有连接
	clrcon();
else 
	set_clrcon_flag(0); // PR40457;   ???????

//验证TermIdStr1和Connect
if (Pin1 > 0){
	getinstid(TermIdStr1, &TermId1);						// 从探针名称字符串中获取探针标识符（ID） ？
	getinstattr(TermId1, "MODELNUM", TempBuf);				// 根据探针ID获取其属性字符串 ？
	if (TempBuf[0] == '\0') 
		return (INVAL_INST_ID);								// 无效ID返回
	if (TermId1 > 0){
		conpin(TermId1, Pin1, 0);							// 连接探针和仪器
	}
}


//验证TermIdStr2和Connect
if (Pin2 > 0){
	getinstid(TermIdStr2, &TermId2);					
	getinstattr(TermId2, "MODELNUM", TempBuf);			
	if (TempBuf[0] == '\0') 
		return (INVAL_INST_ID);							
	if (TermId2 > 0){
		conpin(TermId2, Pin2, 0);						
	}
}

//验证TermIdStr3和Connect
if (Pin3 > 0){
	getinstid(TermIdStr3, &TermId3);					
	getinstattr(TermId3, "MODELNUM", TempBuf);			
	if (TempBuf[0] == '\0') 
		return (INVAL_INST_ID);							
	if (TermId3 > 0){
		conpin(TermId3, Pin3, 0);						
	}
}


//验证TermIdStr4和Connect
if (Pin4 > 0){
	getinstid(TermIdStr4, &TermId4);					
	getinstattr(TermId4, "MODELNUM", TempBuf);			
	if (TempBuf[0] == '\0') 
		return (INVAL_INST_ID);							
	if (TermId4 > 0){
		conpin(TermId4, Pin4, 0);						
	}
}

//验证TermIdStr5和Connect
if (Pin5 > 0){
	getinstid(TermIdStr5, &TermId5);					
	getinstattr(TermId5, "MODELNUM", TempBuf);			
	if (TempBuf[0] == '\0') 
		return (INVAL_INST_ID);							
	if (TermId5 > 0){
		conpin(TermId5, Pin5, 0);						
	}
}

//验证TermIdStr6和Connect
if (Pin6 > 0){
	getinstid(TermIdStr6, &TermId6);					
	getinstattr(TermId6, "MODELNUM", TempBuf);			
	if (TempBuf[0] == '\0') 
		return (INVAL_INST_ID);							
	if (TermId6 > 0){
		conpin(TermId6, Pin6, 0);						
	}
}

//验证TermIdStr7和Connect
if (Pin7 > 0){
	getinstid(TermIdStr7, &TermId7);					
	getinstattr(TermId7, "MODELNUM", TempBuf);			
	if (TempBuf[0] == '\0') 
		return (INVAL_INST_ID);							
	if (TermId7 > 0){
		conpin(TermId7, Pin7, 0);						
	}
}

//验证TermIdStr8和Connect
if (Pin8 > 0){
	getinstid(TermIdStr8, &TermId8);					
	getinstattr(TermId8, "MODELNUM", TempBuf);			
	if (TempBuf[0] == '\0') 
		return (INVAL_INST_ID);							
	if (TermId8 > 0){
		conpin(TermId8, Pin8, 0);						
	}
}

return (OK);