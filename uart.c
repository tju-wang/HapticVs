#include "uart.h"

extern var_t var;
extern var_q varq;
extern const double pi;
extern force_t F1,F2,F3;
extern const double s;	//偏距等效


BOOL SendData(HANDLE hComm,double F1,double F2,double F3,unsigned int sort,unsigned char* wData)
{	
	BOOL Status;
	unsigned char tempVal;
	unsigned char Data[15] = {0};

	Data[0] 		= 0x7B;
	Data[1]	= 0;
	Data[2]  = 0x11;
	tempVal = 3;
	Double2Char(F1,&(Data[tempVal]),&(Data[tempVal+1]),&(Data[tempVal+2]));
	tempVal = 6;
	Double2Char(F2,&(Data[tempVal]),&(Data[tempVal+1]),&(Data[tempVal+2]));
	tempVal = 9;
	Double2Char(F3,&(Data[tempVal]),&(Data[tempVal+1]),&(Data[tempVal+2]));
	Data[12] = sort;
	Data[13] = dataCheck(&Data[0],15);
	Data[14] = 0x7D;

	for(tempVal=0;tempVal<15;tempVal++)	{
		// printf("%d  ",Data[tempVal]);
		*(wData+tempVal) = Data[tempVal];
	}
	// printf("\r\n");
	Status = writeData(hComm,Data,(15));
	return TRUE;

}


BOOL SerialInit(HANDLE *hC)
{
	int PortSort = 0;
    int i,j;
    HANDLE hComm;
    BOOL Status;
 
    char *port = malloc (sizeof (char) * 12); // Just for "straight to test purposes", in production, use the port detection functions instead
    strcpy(port, "\\\\.\\COM13");

	 AVAILABLE_PORTS *ports;                         // Initializing the struct values
    ports = malloc(sizeof(AVAILABLE_PORTS));

    updateAvailablePortsInfo(ports);                // Setup/load time, the function will update the living struct

    printf("Please choose the port: \n");
    scanf("%d",&PortSort);

    Status = InitSerialPort(formatSerialPortName(PortSort),115200,&hComm);
	if(Status==TRUE)	{
		*hC = hComm;
	}
	else {
		printf("Open Serial Fail");
	}
	free(ports);
    return Status;

}


BOOL Update_q(unsigned char *data,var_q *Lq,char *pSort)	//更新杆长数据  处理由下位机发来的数据
{
	var_q temp;
	BOOL Status = TRUE;
	unsigned int Sum = 0;
	int i;
	char tempval;
	static char UartError = 0;

    //输出接收到的数据
	// for(i=0;i<13;i++)	{
    //     printf("%d ",*(data+i));
    // }
    // printf("\n");

	for(i=0;i<11;i++)		//求和  去掉首尾 和check位
	{
		Sum += *(data + i);
	}
	Sum = (unsigned int)Sum%100;

	if(Sum==*(data+11))
		Status = TRUE;
	else
		Status = !TRUE;

	if(Status==TRUE)	{

		temp.q1 = *(data+2)*10 + (float)*(data+3)/10;
		temp.q2 = *(data+4)*10 + (float)*(data+5)/10;
		temp.q3 = *(data+6)*10 + (float)*(data+7)/10;
		// printf("q1 = %6.3f  q2 = %6.3f  q3 = %6.3f \n",temp.q1,temp.q2,temp.q3);

		if(temp.q1<170)	{
			temp.q1 = 170;
			Status = FALSE;
		}
		else if(temp.q1>330)	{
			temp.q1 = 330;
			Status = FALSE;
		}
		if(temp.q2<85)	{
			temp.q2 = 85;
			Status = FALSE;
		}
		else if(temp.q2>290)	{
			temp.q2 = 290;
			Status = FALSE;
		}
		if(temp.q3<140)	{
			temp.q3 = 140;
			Status = FALSE;
		}
		else if(temp.q3>330)	{
			temp.q3 = 330;
			Status = FALSE;
		}

		varq.q1 = sqrt(pow(temp.q1,2)+pow(s,2));
		varq.q2 = sqrt(pow(temp.q2,2)+pow(s,2));
		varq.q3 = sqrt(pow(temp.q3,2)+pow(s,2));

		UartError = *(data+8);
		if(UartError!=0)	{
			ErrorCheck(UartError);
		}
		tempval = *(data+9);
		*pSort = *(data+10);		//更新数据排序
		// printf("%d  \n",tempval);
		// printf("sort = %d  \n",*pSort);
	}
	else {
		printf("Data Check Error!\n");
	}
    if(Status==!TRUE)
    {
        printf("q out of range!\n");
		printf("q %6.3f  %6.3f  %6.3f",temp.q1,temp.q2,temp.q3);
    }
	return Status;

}

void ErrorCheck(unsigned char err)
{
	char i = 0;
	for(i=0;i<8;i++)	{
		if(((err>>i)&(1<<0)))
		{
			switch (i)
			{
			case 0:
				break;
			case 3:
				printf("UartError SortError! \n");
				break;
			case 4:
				printf("UartError MCU RxError! \n");
				break;
			case 5:
				printf("UartError MCU TxError! \n");
				break;
			case 6:
				printf("UartError MCU RxOutofRange! ");
				break;
			
			default:
				break;
			}

		}
	
	}

}
unsigned char dataCheck(unsigned char* buff,char len)
{
	unsigned int ret = 0;
	int i;
	for(i=1;i<len-2;i++)		//求和  去掉首尾 和check位
	{
		ret += *(buff + i);
	}
	ret = (unsigned int)ret%100;
	// printf("%d\n",ret);
	return (unsigned char)ret;
}

void Double2Char(double doub,unsigned char* val_1,unsigned char* val_2,unsigned char* val_3)
{
	if(doub<0)
	{
		doub = -doub;
		
		*val_3 = (unsigned char)(((unsigned int)(doub*10))%100);		//数据类型转换的优先级高于一般运算符
		*val_2 = (unsigned char)(((unsigned int)(doub/10))%100);
		*val_1 = (unsigned char)(((int)(doub/1000))%100);
		
		if(*val_1>127)
		{
			printf("1.Data out of range.");
		}
		else {
			*val_1 |= (1<<7);
		}
		// printf("\n  val_3 = %d,  val_2 = %d ,val_1 = %d  \n",*val_3,*val_2,*val_1);
	}
	else
	{
		*val_3 = (unsigned char)(((unsigned int)(doub*10))%100);		//数据类型转换的优先级高于一般运算符
		*val_2 = (unsigned char)(((unsigned int)(doub/10))%100);
		*val_1 = (unsigned char)(((int)(doub/1000))%100);
		if(*val_1>127)
		{
			printf("2.Data out of range.");
		}
	}
	
}

