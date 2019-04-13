// switchcontrol.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include "SerialPort.h"

//SI=Source & Input
//BNC_1&4:train; BNC2&5:Read_V(需要读的点);BNC3&6:Read_V/2(不读的点，抑制漏电流);

//--------发出控制一次开关的信号-------------------------------------
int * openSwitch(int rst_n, int source_1, int source_2, int input_1, int input_2)
{ //将数组元素转化成二进制的串口通信元素;[0] for source_1(GND_1,BNC_1-3);[1] for source_2(GND_2;BNC_4-6); [2]for input_1(DUT输入0-31); [3] for input_2(DUT输出0-31);
	int output_x[4] = { 0b00000000, 0b00000000, 0b00000000,0b00000000 };//第一位判断是否打开开关；
	int output_y[4] = { 0b00000000, 0b00000000, 0b00000000,0b00000000 };//output_y的第一位永远为0，开关的打开与否由output_x判断
	static int output[5];
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
			output[0] = 0b01000000;//第2位是CLOCK

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
				output_y[0] = 0b00000011;
				output_y[1] = output_y[0];
				break;
			case 3:
				output_y[0] = 0b00000001;
				output_y[1] = output_y[0];
				break;
			default:
				output_y[0] = 0b00000000;
				output_y[1] = output_y[0];
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
				output_y[2] = 0b00000011;
				output_y[3] = output_y[2];
				break;
			case 3:
				output_y[2] = 0b00000001;
				output_y[3] = output_y[2];
				break;
			default:
				output_y[2] = 0b00000000;
				output_y[3] = output_y[2];
				break;
			}

			switch (input_1) //input_1控制source到DUT之间的连线；第一位1：控制开关是否打开
			{
			case 0: output_x[2] = 0b10111000; break;
			case 1: output_x[1] = 0b11111000; break;
			case 2: output_x[3] = 0b10111000; break;
			case 3: output_x[0] = 0b11111000; break;
			case 4: output_x[3] = 0b10110000; break;
			case 5: output_x[0] = 0b11110000; break;
			case 6: output_x[2] = 0b10110000; break;
			case 7: output_x[1] = 0b11110000; break;
			case 8: output_x[2] = 0b10101000; break;
			case 9: output_x[1] = 0b11101000; break;
			case 10: output_x[3] = 0b10101000; break;
			case 11: output_x[0] = 0b11101000; break;
			case 12: output_x[3] = 0b10100000; break;
			case 13: output_x[0] = 0b11100000; break;
			case 14: output_x[2] = 0b10100000; break;
			case 15: output_x[1] = 0b11100000; break;
			case 16: output_x[2] = 0b10011000; break;
			case 17: output_x[1] = 0b11011000; break;
			case 18: output_x[3] = 0b10011000; break;
			case 19: output_x[0] = 0b11011000; break;
			case 20: output_x[3] = 0b10010000; break;
			case 21: output_x[0] = 0b11010000; break;
			case 22: output_x[2] = 0b10010000; break;
			case 23: output_x[1] = 0b11010000; break;
			case 24: output_x[2] = 0b10001000; break;
			case 25: output_x[1] = 0b11001000; break;
			case 26: output_x[3] = 0b10001000; break;
			case 27: output_x[0] = 0b11001000; break;
			case 28: output_x[3] = 0b10000000; break;
			case 29: output_x[0] = 0b11000000; break;
			case 30: output_x[2] = 0b10000000; break;
			case 31: output_x[1] = 0b11000000; break;
			/*case 0:  output_x[0] = 0b10000000; break;
			case 1:  output_x[0] = 0b10001000; break;
			case 2:  output_x[0] = 0b10010000; break;
			case 3:  output_x[0] = 0b10011000; break;
			case 4:  output_x[0] = 0b10100000; break;
			case 5:  output_x[0] = 0b10101000; break;
			case 6:  output_x[0] = 0b10110000; break;
			case 7:  output_x[0] = 0b10111000; break;
			case 8:  output_x[0] = 0b11000000; break;
			case 9:  output_x[0] = 0b11001000; break;
			case 10: output_x[0] = 0b11010000; break;
			case 11: output_x[0] = 0b11011000; break;
			case 12: output_x[0] = 0b11100000; break;
			case 13: output_x[0] = 0b11101000; break;
			case 14: output_x[0] = 0b11110000; break;
			case 15: output_x[0] = 0b11111000; break;
			case 16: output_x[1] = 0b10000000; break;
			case 17: output_x[1] = 0b10001000; break;
			case 18: output_x[1] = 0b10010000; break;
			case 19: output_x[1] = 0b10011000; break;
			case 20: output_x[1] = 0b10100000; break;
			case 21: output_x[1] = 0b10101000; break;
			case 22: output_x[1] = 0b10110000; break;
			case 23: output_x[1] = 0b10111000; break;
			case 24: output_x[1] = 0b11000000; break;
			case 25: output_x[1] = 0b11001000; break;
			case 26: output_x[1] = 0b11010000; break;
			case 27: output_x[1] = 0b11011000; break;
			case 28: output_x[1] = 0b11100000; break;
			case 29: output_x[1] = 0b11101000; break;
			case 30: output_x[1] = 0b11110000; break;
			case 31: output_x[1] = 0b11111000; break;*/
			default:
				output_x[0] = 0b00000000;
				output_x[1] = 0b00000000;
				output_x[2] = 0b00000000;
				output_x[3] = 0b00000000;
				break;
			}

			switch (input_2) //input_2控制source到DUT之间的连线
			{
			case 0: output_x[2] = 0b11111000; break;
			case 1: output_x[1] = 0b10111000; break;
			case 2: output_x[3] = 0b11111000; break;
			case 3: output_x[0] = 0b10111000; break;
			case 4: output_x[3] = 0b11110000; break;
			case 5: output_x[0] = 0b10110000; break;
			case 6: output_x[2] = 0b11110000; break;
			case 7: output_x[1] = 0b10110000; break;
			case 8: output_x[2] = 0b11101000; break;
			case 9: output_x[1] = 0b10101000; break;
			case 10: output_x[3] = 0b11101000; break;
			case 11: output_x[0] = 0b10101000; break;
			case 12: output_x[3] = 0b11100000; break;
			case 13: output_x[0] = 0b10100000; break;
			case 14: output_x[2] = 0b11100000; break;
			case 15: output_x[1] = 0b10100000; break;
			case 16: output_x[2] = 0b11011000; break;
			case 17: output_x[1] = 0b10011000; break;
			case 18: output_x[3] = 0b11011000; break;
			case 19: output_x[0] = 0b10011000; break;
			case 20: output_x[3] = 0b11010000; break;
			case 21: output_x[0] = 0b10010000; break;
			case 22: output_x[2] = 0b11010000; break;
			case 23: output_x[1] = 0b10010000; break;
			case 24: output_x[2] = 0b11001000; break;
			case 25: output_x[1] = 0b10001000; break;
			case 26: output_x[3] = 0b11001000; break;
			case 27: output_x[0] = 0b10001000; break;
			case 28: output_x[3] = 0b11000000; break;
			case 29: output_x[0] = 0b10000000; break;
			case 30: output_x[2] = 0b11000000; break;
			case 31: output_x[1] = 0b10000000; break;
/*			case 0:  output_x[2] = 0b10000000; break;
			case 1:  output_x[2] = 0b10001000; break;
			case 2:  output_x[2] = 0b10010000; break;
			case 3:  output_x[2] = 0b10011000; break;
			case 4:  output_x[2] = 0b10100000; break;
			case 5:  output_x[2] = 0b10101000; break;
			case 6:  output_x[2] = 0b10110000; break;
			case 7:  output_x[2] = 0b10111000; break;
			case 8:  output_x[2] = 0b11000000; break;
			case 9:  output_x[2] = 0b11001000; break;
			case 10: output_x[2] = 0b11010000; break;
			case 11: output_x[2] = 0b11011000; break;
			case 12: output_x[2] = 0b11100000; break;
			case 13: output_x[2] = 0b11101000; break;
			case 14: output_x[2] = 0b11110000; break;
			case 15: output_x[2] = 0b11111000; break;
			case 16: output_x[3] = 0b10000000; break;
			case 17: output_x[3] = 0b10001000; break;
			case 18: output_x[3] = 0b10010000; break;
			case 19: output_x[3] = 0b10011000; break;
			case 20: output_x[3] = 0b10100000; break;
			case 21: output_x[3] = 0b10101000; break;
			case 22: output_x[3] = 0b10110000; break;
			case 23: output_x[3] = 0b10111000; break;
			case 24: output_x[3] = 0b11000000; break;
			case 25: output_x[3] = 0b11001000; break;
			case 26: output_x[3] = 0b11010000; break;
			case 27: output_x[3] = 0b11011000; break;
			case 28: output_x[3] = 0b11100000; break;
			case 29: output_x[3] = 0b11101000; break;
			case 30: output_x[3] = 0b11110000; break;
			case 31: output_x[3] = 0b11111000; break;*/
			default:
				output_x[0] = 0b00000000;
				output_x[1] = 0b00000000;
				output_x[2] = 0b00000000;
				output_x[3] = 0b00000000;
				break;
			}

			for (int i = 0; i <= 3; i = i + 1) {
				output[i + 1] = output_x[i] | output_y[i];
				if (output[i + 1] < 128) {
					output[i + 1] = 0;//此处电路设计有个bug，无法单独操控某一个芯片，所以不操作默认为关闭0000000，如需打开0000000，最后时刻再打开
				}
			}
		}
		return output;
} 

//--------输入阵列中需要控制器件的坐标，输出所需的Pin和Source--------
int * getSI_single(int state, int x, int y) { 
	//to get parameters for openSwitch缺省值为不对openSwitch做任何操作
	//state:0:train;1:read_V;2:read_V/2
	static int outputForSwitch[5] = {0,5,5,32,32};
	if (x == 32 && y == 32)  //reset;当输入信号x,y为32时，执行初始化操作
	{
		outputForSwitch[0] = 1;// Rst_n = 1;
		return outputForSwitch;
	}
	else {
		outputForSwitch[0] = 0;// Rst_n = 0;
		if (x <= 31 && x >= 0 && y <= 31 && y >= 0)//train_x,train_y的编号为0-31；
		{
			switch (state) {
			case 0:
				outputForSwitch[1] = 1;// Source_1 = 1;
			    outputForSwitch[2] = 1;// Source_2 = 1;
				break;
			case 1:
				outputForSwitch[1] = 2;// Source_1 = 2;
				outputForSwitch[2] = 2;// Source_2 = 2;
				break;
			case 2:
				outputForSwitch[1] = 3;// Source_1 = 3;
				outputForSwitch[2] = 3;// Source_2 = 3;
				break;
			default:
				outputForSwitch[1] = 0;// Source_1 = 0;
				outputForSwitch[2] = 0;// Source_2 = 0;
				break;
			}
			
			outputForSwitch[3] = x;// Input_1 = train_x;
			outputForSwitch[4] = y;// Input_2 = train_y;

			return outputForSwitch;
		}
		else {
			printf("端口输入有误");
			return outputForSwitch;
		}
	}
}

//--------------一次打开所有开关---------------
int * getSI_OpenAll() {
	static int output_OpenAll[5120];
	int *temp_0,*temp;

	temp_0=getSI_single(0,32, 32);//Initialization
	temp=openSwitch(*temp_0, *(temp_0+1), *(temp_0 + 2), *(temp_0 + 3), *(temp_0 + 4));
	for (int i = 0; i <= 4; i = i + 1) {
		output_OpenAll[i] = *(temp + i);
	}

	for (int i = 0; i <= 31; i = i + 1) {
		for (int j = 0; j <= 31; j = j + 1) {
			temp_0= getSI_single(0,i, j);
			temp=openSwitch(*temp_0, *(temp_0 + 1), *(temp_0 + 2), *(temp_0 + 3), *(temp_0 + 4));
			output_OpenAll[160*i+5*j+5] = *(temp);
			output_OpenAll[160 * i + 5 * j + 6] = *(temp + 1);
			output_OpenAll[160 * i + 5 * j + 7] = *(temp + 2);
			output_OpenAll[160 * i + 5 * j + 8] = *(temp + 3);
			output_OpenAll[160 * i + 5 * j + 9] = *(temp + 4);
			}
	}
	return output_OpenAll;
}

//---------------单器件训练--------------------
int * Switch_Train_single(int x, int y) { //to get parameters for openSwitch缺省值为不对openSwitch做任何操作
	static int output_Train_single[10] = { 128,0,0,0,0,128,0,0,0,0 }; //第一个信号RESET
	int *temp = getSI_single(0, x, y);
	int *temp_output = openSwitch(*temp,*(temp+1),*(temp+2),*(temp+3),*(temp+4));
	for (int i = 0; i <= 4; i = i + 1) {
		output_Train_single[i + 5] = *(temp_output+i);
	}

	return output_Train_single;
}


//--------------单器件READ-----------------------
int * getSI_Read_single() {
	static int output_Read_single[10];
	return output_Read_single;
}

int main()
{
	int start_time = GetTickCount();
	int x, y;
	int *output_switch,*temp_0;
	
	//------------单开关验证--------------
	printf("please input train_x and train_y\n");
	scanf_s("%d%d",&x,&y);	
	temp_0=getSI_single(0, x, y);
	output_switch = openSwitch(*temp_0, *(temp_0 + 1), *(temp_0 + 2), *(temp_0 + 3), *(temp_0 + 4));
	for (int i = 0; i <= 9; i = i + 1) {
		printf("output[%d] are %d \n", i, *(output_switch + i));
	} //单开关验证结尾
	
	//------------一次打开所有开关验证--------------
	/*output_switch=getSI_OpenAll(); 
	for (int i = 0; i <= 1023; i = i + 1) {
		printf("Switch[%d]:\t ",i);
		for (int j = 0; j <= 4; j = j + 1) {
            printf("[%d]  %d \t",j,*(output_switch+5*i+j));
	  }
		printf("\n");
	}   //循环验证结尾*/

	//-------------单个器件训练验证-----------------
	/*printf("please input train_x and train_y\n");
	scanf_s("%d%d", &x, &y);
	output_switch = Switch_Train_single(x,y);
	for (int i = 0; i <= 9; i = i + 1) {
		printf("output[%d] are %d \n", i, *(output_switch + i));
	}  //单个器件训练验证结尾

	int end_time = GetTickCount(); //结束计时
	int time_all = end_time - start_time;
	printf( "The run time is: %d ms",time_all); //输出时间*/

	//-----------给出串口信号--------------------
	//char portId[5];
	TCHAR gszPort[20]="COM3";   //The port number for serial communication, please change it to the right one.

	/*printf("input the port COM number: \n");
	scanf("%s", &portId);

	strcpy(gszPort, "COM");
	strcat(gszPort, portId);

	printf("port Id: %s\n", gszPort);//setting the port.*/

	//unsigned char result;

	unsigned int input[5] = { 0 };
	//printf("Input the content: \n");
	for (int in = 0; in <= 4; in++) {
		input[in] = output_switch[in];
		//scanf("%d", &input[in]);
	}


	if (openPort(gszPort)) {

		if (setupPort(115200,8)) {

			int i = 0;
			while (i < 5) {
				sendSByte(input[i]);
				i = i + 1;
			}
			readSByte();
		}
		//system("PAUSE");
		closePort();
	}
	return 1;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
