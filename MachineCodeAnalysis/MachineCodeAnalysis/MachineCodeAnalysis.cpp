// MachineCodeAnalysis.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long LONG;

//汇编指令
char *AssemblyCode(FILE *file,BYTE operationCode,char *chAssemblyCode) {

	switch (operationCode)
	{
		case 0x89:
		{

			//先读取一个字节的数据
			BYTE tagTemp;
			fread(&tagTemp, sizeof(tagTemp), 1, file);

			switch (tagTemp)
			{
				case 0xD6:   //MOV  SI, DX
				{
					sprintf(chAssemblyCode, "%s SI,DX", chAssemblyCode);
				}
				break;
				case 0xF3:   //MOV  BX, SI
				{
					sprintf(chAssemblyCode, "%s BX,SI", chAssemblyCode);
				}
				break;
				case 0xFE:   //MOV   SI, DI
				{
					sprintf(chAssemblyCode, "%s SI,DI", chAssemblyCode);
				}
				break;
				case 0x57:   //MOV [BX + XX], DX
				{
					BYTE assemblyCodeTemp;
					fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

					sprintf(chAssemblyCode, "%s [BX + %x],DX", chAssemblyCode, assemblyCodeTemp);
				}
				break;
				case 0x56:   //MOV [BP + XX], DX
				{
					BYTE assemblyCodeTemp;
					fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

					sprintf(chAssemblyCode, "%s [BP + %x],DX", chAssemblyCode, assemblyCodeTemp);
				}
				break;
				case 0x46:   //MOV [BP + XX], AX
				{
					BYTE assemblyCodeTemp;
					fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

					sprintf(chAssemblyCode, "%s [BP + %x],AX", chAssemblyCode, assemblyCodeTemp);
				}
				break;
				default:
					break;
			}

			
		}
		break;
		case 0x88:
		{
			//当操作码为0x88时,往后读取一个字节的数据
			BYTE assemblyCodeTemp;
			fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);
			switch (assemblyCodeTemp)
			{
			case 0xC1:   //MOV  CL, AL
			{
				sprintf(chAssemblyCode, "%s CL,AL", chAssemblyCode);
			}
			break;
			default:
				break;
			}
		}
		break;
		case 0x30:
		{
			//当操作码为0x30时,往后读取一个字节的数据
			BYTE assemblyCodeTemp;
			fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);
			switch (assemblyCodeTemp)
			{
			case 0xED:   //XOR  CH, CH
			{
				sprintf(chAssemblyCode, "%s CH,CH", chAssemblyCode);
			}
			break;
			default:
				break;
			}
		}
		break;
		case 0x83:
		{
			//当操作码为0x83时,往后读一个字节的数据
			//如果该字节为 F9 ,则 再往后读一个字节    CMP指令
			//如果该字节为 7F ,则 再往后读两个字节    CMP指令
			//如果该字节为 C3 , 在 +0B 的时是         ADD 指令

			BYTE tagTemp;
			fread(&tagTemp, sizeof(tagTemp), 1, file);

			switch (tagTemp)
			{
				case 0xF9:    //CMP  CX, XXX
				{
					BYTE operationTemp;
					fread(&operationTemp, sizeof(operationTemp), 1, file);

					if (operationTemp == '\0') {
					
						sprintf(chAssemblyCode, "CMP CX,00", operationTemp);
					}
					else {
					
						sprintf(chAssemblyCode, "CMP CX,%s", operationTemp);
					}
				}
				break;
				case 0x7F:    //CMP WORD PTR [BX + XXX],XXX
				{
					WORD assemblyCodeTemp;
					fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

					BYTE frontByte = (assemblyCodeTemp & 0xFF00) >> 8;
					BYTE backByte = (assemblyCodeTemp & 0x00FF);

					sprintf(chAssemblyCode, "CMP WORD PTR [BX + %x],%x", backByte, frontByte);
				}
				break;
				case 0xC3:    //83C30B        ADD     BX, +0B
				{
					BYTE assemblyCodeTemp;
					fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

					sprintf(chAssemblyCode, "ADD BX,%x", assemblyCodeTemp);
				}
				break;
				default:
					break;
			}
		}
		break;
		case 0xBF:
		{
			//当操作码为0xBF时,往后读取两个字节的数据
			WORD assemblyCodeTemp;
			fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

			BYTE frontByte = (assemblyCodeTemp & 0xFF00) >> 8;
			BYTE backByte = (assemblyCodeTemp & 0x00FF);

			//小堆倒序
			sprintf(chAssemblyCode, "%s DI,%x%x", chAssemblyCode, frontByte , backByte);
		}
		break;
		case 0xB9:
		{
			//当操作码为0xB9时,往后读取两个字节的数据
			WORD assemblyCodeTemp;
			fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

			BYTE frontByte = (assemblyCodeTemp & 0xFF00) >> 8;
			BYTE backByte = (assemblyCodeTemp & 0x00FF);

			//小堆倒序
			sprintf(chAssemblyCode, "%s ,%x%x", chAssemblyCode, frontByte , backByte);
		}
		break;
		case 0xF6:
		{
			//当操作码为0xF6时,往后读取三个字节的数据
			//第一个字节判定为 BX BP SI DI 中的哪一个
			//第二个字节跟第三个字节为进行比较的两个数

			BYTE segTemp;
			fread(&segTemp, sizeof(segTemp), 1, file);

			WORD assemblyCodeTemp;
			fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

			BYTE frontByte = (assemblyCodeTemp & 0xFF00) >> 8;
			BYTE backByte = (assemblyCodeTemp & 0x00FF);

			switch (segTemp)
			{
				case 0x47:  //0x47 表示 BX
				{
					//小堆倒序
					sprintf(chAssemblyCode, "%s BYTE PTR [BX + %x],%x", chAssemblyCode, backByte , frontByte);
				}
				break;
				case 0x46:  //0x46 表示 BP
				{
					//小堆倒序
					sprintf(chAssemblyCode, "%s BYTE PTR [BP + %x],%x", chAssemblyCode, backByte , frontByte);
				}
				break;
				default:
					break;
			}
		}
		break;
		case 0x31:
		{
			//当操作码为0x31时,往后读取一个字节的数据
			BYTE assemblyCodeTemp;
			fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);
			switch (assemblyCodeTemp)
			{
				case 0xC0:   //XOR     AX, AX
				{
					sprintf(chAssemblyCode, "%s AX,AX", chAssemblyCode);
				}
				break;
				case 0xDB:   //XOR     BX, BX
				{
					sprintf(chAssemblyCode, "%s BX,BX", chAssemblyCode);
				}
				break;
				default:
					break;
			}
		}
		break;
		case 0x8C:
		{
			//当操作码为0x8C时,往后读取两个字节的数据

			WORD assemblyCodeTemp;
			fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

			BYTE frontByte = (assemblyCodeTemp & 0xFF00) >> 8;
			BYTE backByte = (assemblyCodeTemp & 0x00FF);

			switch (backByte)
			{
				case 0x4E:
				{
					sprintf(chAssemblyCode, "%s [BP + %x],CS", chAssemblyCode, backByte);
				}
				break;
				case 0x4F:
				{
					sprintf(chAssemblyCode, "%s [BX + %x],CS", chAssemblyCode, backByte);
				}
				break;
				default:
					break;
			}
		}
		break;
		case 0x8B:      
		{

			//当操作码为0x8B时,往后读取三个字节的数据
			//第一个字节判定为 通用寄存器 中的哪一个
			//第二个字节跟第三个字节为操作数

			BYTE regTemp;
			fread(&regTemp, sizeof(regTemp), 1, file);

			WORD assemblyCodeTemp;
			fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

			BYTE frontByte = (assemblyCodeTemp & 0xFF00) >> 8;
			BYTE backByte = (assemblyCodeTemp & 0x00FF);

			switch (regTemp)
			{
				case 0x16:  //0x16 表示 DX
				{
					//小堆倒序
					sprintf(chAssemblyCode, "%s DX , [%x%x]", chAssemblyCode, frontByte , backByte);
				}
				break;
				case 0x1E:  //0x1E 表示 BX
				{
					//小堆倒序
					sprintf(chAssemblyCode, "%s DX , [%x%x]", chAssemblyCode, frontByte , backByte);
				}
				break;
				default:
					break;
			}
		}
		break;
		case 0x8A:
		{

			//当操作码为0x8B时,往后读取三个字节的数据
			//第一个字节判定为 通用寄存器 中的哪一个
			//第二个字节跟第三个字节为操作数

			BYTE regTemp;
			fread(&regTemp, sizeof(regTemp), 1, file);

			WORD assemblyCodeTemp;
			fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

			BYTE frontByte = (assemblyCodeTemp & 0xFF00) >> 8;
			BYTE backByte = (assemblyCodeTemp & 0x00FF);

			switch (regTemp)
			{
				case 0x16:  //0x16 表示 DL
				{
					//小堆倒序
					sprintf(chAssemblyCode, "%s DL , [%x%x]", chAssemblyCode, frontByte , backByte);
				}
				break;
				case 0x36:  //0x36 表示 DH
				{
					//小堆倒序
					sprintf(chAssemblyCode, "%s DH , [%x%x]", chAssemblyCode, frontByte , backByte);
				}
				break;
				default:
					break;
			}
		}
		break;
		case 0xC6:    
		{

			//当操作码为0x8B时,往后读取三个字节的数据
			//第一个字节判断 字长
			//第二个字节跟第三个字节加起来为偏移量
			//如果 字长 为 BYTE
				//读取第四个字节
		    //如果 字长 为 WORD
				//读取第四跟第五个字节

			BYTE regTemp;
			fread(&regTemp, sizeof(regTemp), 1, file);

			WORD assemblyCodeTemp;
			fread(&assemblyCodeTemp, sizeof(assemblyCodeTemp), 1, file);

			BYTE frontByte = (assemblyCodeTemp & 0xFF00) >> 8;
			BYTE backByte = (assemblyCodeTemp & 0x00FF);

			switch (regTemp)
			{
				case 0x06:  //0x16 表示   MOV  BYTE PTR[XXXX], XX
				{
					BYTE valueTemp;
					fread(&valueTemp, sizeof(valueTemp), 1, file);

					//小堆倒序
					sprintf(chAssemblyCode, "%s BYTE PTR[%x%x], %x", chAssemblyCode, frontByte , backByte, valueTemp);
				}
				break;
				default:
					break;
			}
		}
		break;
	default:
		break;
	}

	return chAssemblyCode;
}

//操作码
void operationCode(FILE *file,BYTE &operationCode) {

	//读取1个字节的操作码
	BYTE operationCodeTemp;
	fread(&operationCode, sizeof(operationCode), 1, file);
	operationCodeTemp = operationCode;

	//BYTE frontByte = (machineCode & 0xFF00)>>8;  //D6
	//BYTE backByte = (machineCode & 0x00FF);  //89

	//拼接成的单条汇编指令
	char chAssemblyCodeTemp[256] = { 0 };

	switch (operationCodeTemp)
	{
		case 0x89:  //mov 指令
		{
			sprintf(chAssemblyCodeTemp, "%s", "MOV");

			AssemblyCode(file, operationCodeTemp,chAssemblyCodeTemp);
		}
		break;
		case 0xAD:  //LODS 类指令
		{
			sprintf(chAssemblyCodeTemp, "%s", "LODSW");
		}
		break;
		case 0x50:  //PUSH 类指令
		{
			sprintf(chAssemblyCodeTemp, "%s", "PUSH AX");
		}
		break;
		case 0xAC:  //LODS 类指令
		{
			sprintf(chAssemblyCodeTemp, "%s", "LODSB");
		}
		break;
		case 0x88:  //MOV 指令
		{
			sprintf(chAssemblyCodeTemp, "%s", "MOV");
			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0x30:  //XOR 类指令
		{
			sprintf(chAssemblyCodeTemp, "%s", "XOR");
			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0x58:  //POP AX
		{
			sprintf(chAssemblyCodeTemp, "%s", "POP AX");
		}
		break;
		case 0x83:  //CMP 跟 ADD 类指令
		{

			// CMP 跟 ADD
			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0xBF:  //MOVSX 
		{
			sprintf(chAssemblyCodeTemp, "%s", "MOV");

			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0x57:  //PUSH DI
		{
			sprintf(chAssemblyCodeTemp, "%s", "PUSH DI");
		}
		break;
		case 0x51:  //PUSH CX
		{
			sprintf(chAssemblyCodeTemp, "%s", "PUSH CX");
		}
		break;
		case 0xB9:  //MOV CX
		{
			sprintf(chAssemblyCodeTemp, "%s", "MOV CX");

			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0xF3:  //REPZ
		{
			sprintf(chAssemblyCodeTemp, "%s", "REPZ");
		}
		break;
		case 0xA4:  //MOVSB
		{
			sprintf(chAssemblyCodeTemp, "%s", "MOVSB");
		}
		break;
		case 0xF6:  //TEST 比较的内存大小为 BYTE
		{
			sprintf(chAssemblyCodeTemp, "%s", "TEST");

			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0x31:  //XOR 类指令
		{
			sprintf(chAssemblyCodeTemp, "%s", "XOR");

			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0xAB:  //STOSW
		{
			sprintf(chAssemblyCodeTemp, "%s", "STOSW");
		}
		break;
		case 0xAA:  //STOSB
		{
			sprintf(chAssemblyCodeTemp, "%s", "STOSB");
		}
		break;
		case 0x5F:  //POP DI
		{
			sprintf(chAssemblyCodeTemp, "%s", "POP DI");
		}
		break;
		case 0x8C:  //MOV 类操作
		{
			sprintf(chAssemblyCodeTemp, "%s", "MOV");

			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0x59:  //POP  CX
		{
			sprintf(chAssemblyCodeTemp, "%s", "POP CX");
		}
		break;
		case 0x8B:  //MOV 类操作
		{
			sprintf(chAssemblyCodeTemp, "%s", "MOV");

			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0x8A:  //MOV 类操作
		{
			sprintf(chAssemblyCodeTemp, "%s", "MOV");

			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0xC6:  //MOV 类操作
		{
			sprintf(chAssemblyCodeTemp, "%s", "MOV");

			AssemblyCode(file, operationCodeTemp, chAssemblyCodeTemp);
		}
		break;
		case 0x90:  //NOP
		{
			sprintf(chAssemblyCodeTemp, "%s", "NOP");
		}
		break;
		case 0xC3:  //RET
		{
			sprintf(chAssemblyCodeTemp, "%s", "RET");
		}
		break;
		default:
			break;
	}

	printf("%s\r\n", chAssemblyCodeTemp);

}

int main()
{
	FILE *file;

	if ((file = fopen("TEST.COM", "rb")) == NULL)
	{
		printf("打开机器码文件失败!\n");
		return 0;
	}

	//指向文件首部
	fseek(file, 0, SEEK_SET);

	while (true)
	{
		BYTE operationCodeTemp = {0};

		operationCode(file, operationCodeTemp);

		if ((int)operationCodeTemp == 0xC3) {
		
			break;
		}
	}

	//关闭打开的文件
	fclose(file);

	system("pause");

    return 0;
}

