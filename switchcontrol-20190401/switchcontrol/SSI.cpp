#include "SSI.h"
#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>	//Contain definations for handle, DWORD and DCB
#include <tchar.h>		//Contain definations for _T


HANDLE hComm;
//SYSTEMTIME etime;

bool openPort(TCHAR *gszPort) {
	printf("try to open the port %s \n", gszPort);

	hComm = CreateFile(gszPort,                                     // pointer to name of the file
		GENERIC_READ | GENERIC_WRITE,								// access (read-write) mode
		0,                                                          // share mode
		0,                                                          // pointer to security attributes
		OPEN_EXISTING,												// how to create
		0,                                                          // file attributes
		0);															// handle to file with attributes to copy

	if (hComm == INVALID_HANDLE_VALUE) {
		printf("failed to open serial port %s \n", gszPort);
		return 0;
	}
	else {
		printf("serial port %s opened \n", gszPort);
		return 1;
	}
}

bool setupPort(int bps=115200, int bytesize=8) {
	printf("try to set up the port \n");

	DCB dcb;

	if (!GetCommState(hComm, &dcb)) {
		printf("getDCB failed\n");
		return 0;
	}

	dcb.BaudRate = bps;
	dcb.fParity = FALSE;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.ByteSize = bytesize;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;

	if (!SetCommState(hComm, &dcb)) {
		printf("failed to set port state (%d)\n", GetLastError());
		return 0;
	}
	else {
		printf("Port setup complete\n");
		return 1;
	}
}

/*
	Discards all characters from the output or input buffer of a specified communications resource.
	It can also terminate pending read or write operations on the resource.
*/
bool purgePort() {
	if (PurgeComm(hComm, PURGE_RXCLEAR)) {
		return 1;
	}
	else {
		printf("Port purge failed\n");
		return 0;
	}
}

bool closePort() {
	printf("try to close the port \n");

	if (CloseHandle(hComm)) {
		printf("Port closed\n");
		return 1;
	}
	else {
		printf("Port close failed\n");
		return 0;
	}
}

bool sendSByte(unsigned char byteToWrite) {
	DWORD dwWritten;
	if (WriteFile(hComm, &byteToWrite, sizeof(byteToWrite), &dwWritten, 0)) {
		printf("wrote byte %Xh (%c) to serial port\n", byteToWrite, byteToWrite);
		return 1;
	}
	else {
		printf("serial port write failed\n");
		return 0;
	}
}

unsigned char readSByte() {
	DWORD dwRead;
	unsigned char lpBuf;

	ReadFile(hComm,                     // handle of file to read
		&lpBuf,                         // address of buffer that receives data
		sizeof(lpBuf),                  // number of bytes to read
		&dwRead,                        // address of number of bytes read
		0);                             // address of structure for data
	printf("%d \n", lpBuf);
	return lpBuf;
}

int openSwitch(int input[5])//input_1 for I port; input_2 for O port.
{ //将数组元素转化成二进制的串口通信元素;[0] for source_1(GND_1,BNC_1-3);[1] for source_2(GND_2;BNC_4-6); [2]for input_1(DUT输入1-31); [3] for input_2(DUT输出1-32);
	int rst_n = input[0], source_1 = input[1], source_2 = input[2], input_1 = input[3], input_2 = input[4];
	int output_x[4] = { 0b00000000, 0b00000000, 0b00000000,0b00000000 };//第一位判断是否打开开关；
	int output_y[4] = { 0b00000000, 0b00000000, 0b00000000,0b00000000 };//output_y的第一位永远为0，开关的打开与否由output_x判断
	int output[5];
	for (int t = 0; t <= 4; t = t + 1)
	{
		output[t] = 0b00000000;//output[0]控制rst_n,output[1][2]控制DUT的输入端； output[3][4]控制DUT的输出端
	}
	if (rst_n)   //只有当rst_n不为1的时候才开始赋值
	{
		output[0] = 0b10000000;//第1位是RST
	}
	else
	{
		output[0] = 0b01000000;//I的第2位是CLOCK
		switch (source_1)//source_1: 0.BNC_1; 1.BNC_2; 2.BNC_3; 3. BNC_4;
		{
		case 0:
			output_y[0] = 0b00000111; //第一位1：开关开，2-4位：输入口
			output_y[1] = output_y[0];
			break;
		case 1:
			output_y[0] = 0b00000101;
			output_y[1] = output_y[0];
			break;
		case 2:
			output_y[0] = 0b00000110;
			output_y[1] = output_y[0];
			break;
		case 3:
			output_y[0] = 0b00000100;
			output_y[1] = output_y[0];
			break;
		default:
			output_y[0] = 0b00000000;
			output_y[1] = output_y[0];
			break;
		}
		switch (input_1) //input_1控制source到DUT之间的连线；第一位1：控制开关是否打开
		{
		case 1: output_x[1] = 0b10001000; break;
		case 2: output_x[0] = 0b10000000; break;
		case 3: output_x[0] = 0b10001000; break;
		case 4: output_x[1] = 0b10000000; break;
		case 5: output_x[0] = 0b11001000; break;
		case 6: output_x[1] = 0b11000000; break;
		case 7: output_x[1] = 0b11001000; break;
		case 8: output_x[0] = 0b11000000; break;
		case 9: output_x[1] = 0b10101000; break;
		case 10: output_x[0] = 0b10100000; break;
		case 11: output_x[0] = 0b10101000; break;
		case 12: output_x[1] = 0b10100000; break;
		case 13: output_x[0] = 0b11101000; break;
		case 14: output_x[1] = 0b11100000; break;
		case 15: output_x[1] = 0b11101000; break;
		case 16: output_x[0] = 0b11100000; break;
		case 17: output_x[1] = 0b10011000; break;
		case 18: output_x[0] = 0b10010000; break;
		case 19: output_x[0] = 0b10011000; break;
		case 20: output_x[1] = 0b10010000; break;
		case 21: output_x[0] = 0b11011000; break;
		case 22: output_x[1] = 0b11010000; break;
		case 23: output_x[1] = 0b11011000; break;
		case 24: output_x[0] = 0b11010000; break;
		case 25: output_x[1] = 0b10111000; break;
		case 26: output_x[0] = 0b10110000; break;
		case 27: output_x[0] = 0b10111000; break;
		case 28: output_x[1] = 0b10110000; break;
		case 29: output_x[0] = 0b11111000; break;
		case 30: output_x[1] = 0b11110000; break;
		case 31: output_x[1] = 0b11111000; break;
		case 32: output_x[0] = 0b11110000; break;
		default:
			output_x[0] = 0b00000000;
			output_x[1] = 0b00000000;
			break;
		}


		switch (source_2)//source_2: 0.BNC_5; 1.BNC_6; 2.BNC_7; 3. BNC_8;
		{
		case 0:
			output_y[2] = 0b00000111; //第一位0：source端不控制开关，2-4位：输入口
			output_y[3] = output_y[2];
			break;
		case 1:
			output_y[2] = 0b00000101;
			output_y[3] = output_y[2];
			break;
		case 2:
			output_y[2] = 0b00000110;
			output_y[3] = output_y[2];
			break;
		case 3:
			output_y[2] = 0b00000100;
			output_y[3] = output_y[2];
			break;
		default:
			output_y[2] = 0b00000000;
			output_y[3] = output_y[2];
			break;
		}
		switch (input_2) //input_2控制source到DUT之间的连线
		{
		case 1: output_x[3] = 0b10000000; break;
		case 2: output_x[2] = 0b10001000; break;
		case 3: output_x[2] = 0b10000000; break;
		case 4: output_x[3] = 0b10001000; break;
		case 5: output_x[2] = 0b11000000; break;
		case 6: output_x[3] = 0b11001000; break;
		case 7: output_x[3] = 0b11000000; break;
		case 8: output_x[2] = 0b11001000; break;
		case 9: output_x[3] = 0b10100000; break;
		case 10: output_x[2] = 0b10101000; break;
		case 11: output_x[2] = 0b10100000; break;
		case 12: output_x[3] = 0b10101000; break;
		case 13: output_x[2] = 0b11100000; break;
		case 14: output_x[3] = 0b11101000; break;
		case 15: output_x[3] = 0b11100000; break;
		case 16: output_x[2] = 0b11101000; break;
		case 17: output_x[3] = 0b10010000; break;
		case 18: output_x[2] = 0b10011000; break;
		case 19: output_x[2] = 0b10010000; break;
		case 20: output_x[3] = 0b10011000; break;
		case 21: output_x[2] = 0b11010000; break;
		case 22: output_x[3] = 0b11011000; break;
		case 23: output_x[3] = 0b11010000; break;
		case 24: output_x[2] = 0b11011000; break;
		case 25: output_x[3] = 0b10110000; break;
		case 26: output_x[2] = 0b10111000; break;
		case 27: output_x[2] = 0b10110000; break;
		case 28: output_x[3] = 0b10111000; break;
		case 29: output_x[2] = 0b11110000; break;
		case 30: output_x[3] = 0b11111000; break;
		case 31: output_x[3] = 0b11110000; break;
		case 32: output_x[2] = 0b11111000; break;
		default:
			output_x[2] = 0b00000000;
			output_x[3] = 0b00000000;
			break;
		}



		for (int i = 0; i <= 3; i = i + 1) {
			output[i + 1] = output_x[i] | output_y[i];
			if (output[i + 1] < 128) {
				output[i + 1] = 0;//此处电路设计有个bug，无法单独操控某一个芯片，所以不操作默认为关闭0000000，故这一路不用
			}
		}
	}

	int uart = 0;//发送串口指令
	while (uart < 5) {
		sendSByte(output[uart]);
		uart = uart + 1;
	}
	readSByte();
	return 1;
}

//--------输入阵列中需要控制器件的坐标，输出所需的Pin和Source--------
//无需解决电路bug的问题
int Switch_SSI(int source_i[32], int source_o[32], int x[32], int y[32]) {
	//to get parameters for openSwitch缺省值为不对openSwitch做任何操作
	//state:0:train;1:read_V;2:read_V/2
	int output[5] = { 1,0,0,0,0 };
	int size = 0;//确认非零数目

	openSwitch(output);//初始化

	for (int j = 0; j < 32; j++) {
		if (x[j] != 0 || y[j] != 0) {
			size = size + 1;
		}
	}

	output[0] = 0;// Rst_n = 0;
	for (int i = 0; i < size; i++) {
		if (x[i] <= 32 && x[i] >= 0 && y[i] <= 32 && y[i] >= 0) {  //train_x,train_y的编号为1-32；
			output[1] = source_i[i];
			output[2] = source_o[i];
			output[3] = x[i];// Input_1 = x;
			output[4] = y[i];// Input_2 = y;
			openSwitch(output);
		}

		else {
			printf("端口输入有误");
			return 0;
		}
	}

	return 1;
}