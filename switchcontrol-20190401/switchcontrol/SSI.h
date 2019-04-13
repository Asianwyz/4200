#ifndef SERIALPORT_H
#define SERIALPORT_H


#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>	//Contain definations for handle, DWORD and DCB
#include <tchar.h>		//Contain definations for _T

//int portId;				//e.g Com#3 's portId is 3.
//TCHAR commPort[11];		//e.g "\\\\.\\com3"

/*
	Definitions for the methods about serial port operations.
*/
bool openPort(TCHAR *gszPort);

bool setupPort(int, int);

bool purgePort();

bool closePort();

bool sendSByte(unsigned char byteToWrite);

unsigned char readSByte();

int openSwitch(int input[5]);

int Switch_SSI(int source_i[32], int source_o[32], int x[32], int y[32]);

#endif  
/*-------------------- -
来源：CSDN
原文：https ://blog.csdn.net/u011680118/article/details/49208771 
版权声明：本文为博主原创文章，转载请附上博文链接！#pragma once*/
