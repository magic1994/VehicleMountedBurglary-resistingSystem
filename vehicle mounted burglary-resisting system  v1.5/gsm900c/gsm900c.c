#include "STC12C5A60S2.H"
#include "uart2_STC12C5A60S2.h"
#include <stdio.h>
#include "Delay.h"
#include "uart.h"
#include "gsm900c.h"
#include "car.h"
#include "gps.h"

unsigned char xdata GPGGA_Buffer[68] = {0};

unsigned char SEC_TEMP[2] = {0};

int SEC1 = 0;
// unsigned char xdata GPRMC_Buffer[68] = {0};

unsigned char a[4] = {0};

unsigned char xdata Control[2][20] = {"AT\r",
	                                  "AT+CMGF=0\r"
                                     };
/*unsigned char xdata C_Word[8][40] = {"AT+CMGS=105\r",
	                               "0011000D91688160063821F90008AA",
	                               "5A00000000003A00000000003A0000000000",
	                               "208F665DF288AB76D78BF753CA65F662A58B66",
	                               "00207EAC5EA6004E000000005EA600000000",
	                               "52060000000079D200207ECF",
	                               "5EA600450000000000005EA600000000",
	                               "52060000000079D2"
                                  };*/

unsigned char xdata C_Word[8][40] = {"AT+CMGS=105\r",
	                               "0011000D91688153910588F20008AA",
								   "5A00000000003A00000000003A0000000000",
	                               "208F665DF288AB76D78BF753CA65F662A58B66",
	                               "00207EAC5EA6004E000000005EA600000000",
	                               "52060000000079D200207ECF",
	                               "5EA600450000000000005EA600000000",
	                               "52060000000079D2"
                                  };
unsigned char xdata Send[3] = {0x1A, '\r', '\n'};
unsigned char xdata C_Words[250] = {0};

unsigned char xdata Rewrite_Close[7] = {"ATE0\r\n"};

unsigned char xdata E_Control[2][20] = {"AT\r",
	                                  "AT+CMGF=1\r"
                                      };

unsigned char xdata Unread_Message[25] = {"AT+CMGL=\"REC UNREAD\"\r\n"};

unsigned char xdata Read_Message_Clean[15] = {"AT+CMGD=1,2\r\n"};

unsigned char xdata Message_Received[200] = {0};

void message_send()
{
	sprintf(C_Words, "%s%s\0", C_Word[1], C_Word[2]);
	sprintf(C_Words, "%s%s\0", C_Words, C_Word[3]);
	sprintf(C_Words, "%s%s\0", C_Words, C_Word[4]);
	sprintf(C_Words, "%s%s\0", C_Words, C_Word[5]);
	sprintf(C_Words, "%s%s\0", C_Words, C_Word[6]);
	sprintf(C_Words, "%s%s\0", C_Words, C_Word[7]);
	uart2_send_str2(Control[0]);
	Delay_s(2);
	RI = 0;
	uart2_send_str2(Control[1]);
	Delay_s(2);
	RI = 0;
	uart2_send_str2(C_Word[0]);
	Delay_s(2);
	RI = 0;
	uart2_send_str2(C_Words);
	Delay_s(2);
	RI = 0;
	uart2_send_str1(2, Send);
	Delay_s(2);
	RI = 0;
}

void DEC_change_to_HEX(unsigned char b)
{
	unsigned char temp = 0;
	
	if(b >= '0' && b <= '9')
	{
		a[0] = '0';
		a[1] = '0';
		a[2] = '3';
		a[3] = b;
	}
	else
	{
		if(b >= 'A' && b <= 'O')
		{
			a[0] = '0';
		    a[1] = '0';
		    a[2] = '4';
			
			if((temp = b - 'A') < 9)
			{
				a[3] = temp + 1 + '0';
			}
			else
			{
				temp = b - 'J';
				a[3] = temp + 'A';
			}
		}
		if(b >= 'P' && b <= 'Z')
		{
			a[0] = '0';
		    a[1] = '0';
		    a[2] = '5';
			
			if((temp = b - 'P') < 10)
			{
				a[3] = temp + '0';
			}
			else
			{
				a[3] = 'A';
			}
		}
	}
}

void MIN_change_to_SEC()
{
	SEC1 = ((GPGGA_Buffer[22] - '0') * 100 + (GPGGA_Buffer[23] - '0') * 10 + (GPGGA_Buffer[24] - '0')) * 60 / 1000;
	
	SEC_TEMP[0] = SEC1 / 10 + '0';
	SEC_TEMP[1] = SEC1 % 10 + '0';
	
	DEC_change_to_HEX(SEC_TEMP[0]);
	C_Word[5][4] = a[0];
	C_Word[5][5] = a[1];
	C_Word[5][6] = a[2];
	C_Word[5][7] = a[3];
	
	DEC_change_to_HEX(SEC_TEMP[1]);
	C_Word[5][8] = a[0];
	C_Word[5][9] = a[1];
	C_Word[5][10] = a[2];
	C_Word[5][11] = a[3];
	
	
	
	SEC1 = ((GPGGA_Buffer[36] - '0') * 100 + (GPGGA_Buffer[37] - '0') * 10 + (GPGGA_Buffer[38] - '0')) * 60 / 1000;
	
	SEC_TEMP[0] = SEC1 / 10 + '0';
	SEC_TEMP[1] = SEC1 % 10 + '0';
	
	DEC_change_to_HEX(SEC_TEMP[0]);
	C_Word[7][4] = a[0];
	C_Word[7][5] = a[1];
	C_Word[7][6] = a[2];
	C_Word[7][7] = a[3];
	
	DEC_change_to_HEX(SEC_TEMP[1]);
	C_Word[7][8] = a[0];
	C_Word[7][9] = a[1];
	C_Word[7][10] = a[2];
	C_Word[7][11] = a[3];
	
}

void handle_time()
{
	int hour = 0;
	
	hour = (((GPGGA_Buffer[8] - '0') + 8) + (GPGGA_Buffer[7] - '0') * 10) % 24;
	
	DEC_change_to_HEX(hour / 10 + '0');
	C_Word[2][2] = a[0];
	C_Word[2][3] = a[1];
	C_Word[2][4] = a[2];
	C_Word[2][5] = a[3];
	
	DEC_change_to_HEX(hour % 10 + '0');
	C_Word[2][6] = a[0];
	C_Word[2][7] = a[1];
	C_Word[2][8] = a[2];
	C_Word[2][9] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[9]);
	C_Word[2][14] = a[0];
	C_Word[2][15] = a[1];
	C_Word[2][16] = a[2];
	C_Word[2][17] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[10]);
	C_Word[2][18] = a[0];
	C_Word[2][19] = a[1];
	C_Word[2][20] = a[2];
	C_Word[2][21] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[11]);
	C_Word[2][26] = a[0];
	C_Word[2][27] = a[1];
	C_Word[2][28] = a[2];
	C_Word[2][29] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[12]);
	C_Word[2][30] = a[0];
	C_Word[2][31] = a[1];
	C_Word[2][32] = a[2];
	C_Word[2][33] = a[3];
}

void full_C_Word()
{
	handle_time();
	
	DEC_change_to_HEX(GPGGA_Buffer[17]);
	C_Word[4][16] = a[0];
	C_Word[4][17] = a[1];
	C_Word[4][18] = a[2];
	C_Word[4][19] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[18]);
	C_Word[4][20] = a[0];
	C_Word[4][21] = a[1];
	C_Word[4][22] = a[2];
	C_Word[4][23] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[19]);
	C_Word[4][28] = a[0];
	C_Word[4][29] = a[1];
	C_Word[4][30] = a[2];
	C_Word[4][31] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[20]);
	C_Word[4][32] = a[0];
	C_Word[4][33] = a[1];
	C_Word[4][34] = a[2];
	C_Word[4][35] = a[3];
	
	
	DEC_change_to_HEX(GPGGA_Buffer[30]);
	C_Word[6][8] = a[0];
	C_Word[6][9] = a[1];
	C_Word[6][10] = a[2];
	C_Word[6][11] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[31]);
	C_Word[6][12] = a[0];
	C_Word[6][13] = a[1];
	C_Word[6][14] = a[2];
	C_Word[6][15] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[32]);
	C_Word[6][16] = a[0];
	C_Word[6][17] = a[1];
	C_Word[6][18] = a[2];
	C_Word[6][19] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[33]);
	C_Word[6][24] = a[0];
	C_Word[6][25] = a[1];
	C_Word[6][26] = a[2];
	C_Word[6][27] = a[3];
	
	DEC_change_to_HEX(GPGGA_Buffer[34]);
	C_Word[6][28] = a[0];
	C_Word[6][29] = a[1];
	C_Word[6][30] = a[2];
	C_Word[6][31] = a[3];
	
	MIN_change_to_SEC();
	
	message_send();
}

void message_find()
{
	unsigned char x = 0;
	
	unsigned char i = 0;

    uart2_receive_str(Message_Received);
	
	com_send_str(200, Message_Received);
	
	for(x = 0; x < 200; x++)
	{
		if(Message_Received[x] == '%')
		{
			x++;
			if(Message_Received[x] == 'W')
			{
				com_send_byte('A');
				
				while(Flag_GPS_OK != 1)
				{
					gps_receive();
				}
				
				for(i = 0; i < 68; i++)
				{
					GPGGA_Buffer[i] = RX_Buffer[i];
				}
				
				full_C_Word();
				
				Flag_GPS_OK = 0;
				
				GPGGA_Buffer[17] = 'A';
				
				Message_Received[x - 1] = 0;
			}
			else if(Message_Received[x] == 'S')
			{
				com_send_byte('A');
				if(Flag_SWITCH)
			    {
				    Flag_SWITCH = 0;
			    }
			    else
			    {
				    Flag_SWITCH = 1;
			    }
				
				Message_Received[x - 1] = 0;
			}
		}
	}
}

void message_receive()
{
	uart2_send_str2(Rewrite_Close);
	Delay_s(1);
	
	uart2_send_str2(E_Control[0]);
	Delay_s(1);
	
	uart2_send_str2(E_Control[1]);
	Delay_s(1);
	
	uart2_send_str2(Unread_Message);
	message_find();
	
//	uart2_send_str2(Read_Message_Clean);
}

