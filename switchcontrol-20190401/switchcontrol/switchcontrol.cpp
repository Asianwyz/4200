﻿// switchcontrol.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include "SSI.h"

//--------发出控制一次开关的信号-------------------------------------




int main()
{
	int s_i[32] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	int s_o[32] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	int x[32] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32 };
	int y[32] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32 };
	int result;
	//-------配置串口------------
	TCHAR gszPort[20] = "COM1";
	openPort(gszPort);
	setupPort(115200, 8);
	//------------打开开关--------------
	result=Switch_SSI(s_i,s_o, x, y);

	closePort();//关闭串口
	
	return 1;
}