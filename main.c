/*
* game_intro.c
*
* Created: 2019-11-07 오후 8:40:28
* Author : david
*/

#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "_main.h"
#include "_glcd2.h"
#include "bit_map_character2.h"
#include "_buzzer.h"

unsigned int Data_ADC3 = 0;//조이스틱 상하 데이터 값 저장용
unsigned int Data_ADC4 = 0;//조이스틱 좌우 데이터 값 저장용
#define ADC_VREF_TYPE 0x00//AD 컨버터 사용 기준 전압 REFS 설정
#define ADC_AVCC_TYPE 0x40//AD 컨버터 사용 기준 전압 AVCC 설정
#define ADC_RES_TYPE 0x80//AD 컨버터 사용 기준 전압 RES 설정
#define ADC_2_56_TYPE 0xC0//AD 컨버터 사용 기준 전압 2.56V 설정

char block[9][16];
char blank[9];
char blocktype, x1, y1, x_left;
int main_x, main_y;
int level = 1;
int score;

void Adc_init(void)
{
	ADCSRA = 0x00;//disable adc
	ADMUX = 0x00;//select adc input 0
	ACSR = 0x80;//1000 0000
	ADCSRA = 0xC3;//1100 0011
}
unsigned int Read_Adc_Data(unsigned char adc_input)//ADC 값 읽는 부분
{
	unsigned int adc = 0;
	ADCSRA = 0xC3;
	ADMUX = adc_input | ADC_AVCC_TYPE;//adc 사용핀 설정
	ADCSRA |= 0x40;
	while((ADCSRA & 0x10) != 0x10);//0100 0000; wait for the AD conversion to complete
	
	adc += ADCL + (ADCH * 256);
	ADCSRA = 0x00;
	return adc;
	
}
void joystick_control(void)//조이스틱의 움직임을 인식해 블럭을 움직이는 함수
{
	short Voltage1 = 0;
	short Voltage2 = 0;
	Data_ADC3 = Read_Adc_Data(3) / 14;
	Data_ADC4 = Read_Adc_Data(4) /  8;
	for(int i = 0; i <= 9 - level; i++)
		_delay_ms(50);
	
	if(Data_ADC3 < 20)//우(-->) 조작
	{
		if(main_x > 0)
		{
			main_x--;
			x_left--;
		}
		for(int i = 0; i <= 10 - level; i++)
			_delay_ms(50);
	}
	else if(Data_ADC3 > 50)//좌(<--) 조작
	{
		
		if(x_left < 8)
		{
			x_left++;
			main_x++;
		}
		for(int i = 0; i <= 9 - level; i++)
			_delay_ms(50);
	}
	else if(Data_ADC4 > 90)//하 조작
	{
		main_y++;
		for(int i = 0; i <= 9 - level; i++)
			_delay_ms(50);
	}
	else if(Data_ADC4 < 20)//상 조작
	{		
		if(blocktype == 2) blocktype = 6;
		else if(blocktype == 6) blocktype = 7;
		else if(blocktype == 7) blocktype = 8;
		else if(blocktype == 8) blocktype = 2;
		else if(blocktype == 3) blocktype = 9;
		else if(blocktype == 9) blocktype = 10;
		else if(blocktype == 10) blocktype = 11;
		else if(blocktype == 11) blocktype = 3;
		else if(blocktype == 4) blocktype = 12;
		else if(blocktype == 12) blocktype = 13;
		else if(blocktype == 13) blocktype = 14;
		else if(blocktype == 14) blocktype = 4;
		else if(blocktype == 5) blocktype = 15;
		else if(blocktype == 15) blocktype = 5;
		for(int i = 0; i <= 10 - level; i++)
			_delay_ms(40);
	}
	else
		for(int i = 0; i <= 9 - level; i++)
			_delay_ms(50);
}

void Port_init(void)
{
	
	PORTA = 0x00; DDRA = 0xFF;
	PORTB = 0xFF; DDRB = 0xFF;
	PORTC = 0x00; DDRC = 0xF0;
	PORTD = 0x00; DDRD = 0x00;
	PORTE = 0x00; DDRE = 0xFF;
	PORTF = 0x00; DDRF = 0x00;
}
void init_devices(void)
{
	cli();
	Port_init();
	lcd_init();
	Adc_init();
	sei();
}
int block0(unsigned char x, unsigned char y)//기본 블럭으로 사용되는 단위 블럭
{
	x1 = 7 * x;
	y1 = y + 2;
	GLCD_Rectangle_black(x1, 1 + 7*y1, x1 + 6, 7 + 7*y1);
	return 0;
}
int block1(unsigned char x, unsigned char y)//정사각형 2*2
{
	block0(x, y);
	block0(x, y + 1);
	block0(x + 1, y);
	block0(x + 1, y + 1);
	return 1;
}
int block2(unsigned char x, unsigned char y)//ㅗ모양 블럭   블럭 회전 시 block 6,7,8로 변환
{
	block0(x + 1, y);
	block0(x, y + 1);
	block0(x + 1, y + 1);
	block0(x + 2, y + 1);
	return 2;
}
int block3(unsigned char x, unsigned char y)//ㄴ모양 블럭   블럭 회전 시 block 9, 10, 11로 변환
{
	block0(x, y);
	block0(x, y + 1);
	block0(x + 1, y + 1);
	block0(x + 2, y + 1);
	return 3;
}
int block4(unsigned char x, unsigned char y)//ㄱㄴ모양 블럭  블럭 회전 시 block 12, 13, 14로 변환
{
	block0(x + 1, y);
	block0(x + 2, y);
	block0(x, y + 1);
	block0(x + 1, y + 1);
	return 4;
}
int block5(unsigned char x, unsigned char y)//ㅣ모양 블럭    블럭 회전 시 block 15로 변환
{
	block0(x, y);
	block0(x, y + 1);
	block0(x, y + 2);
	block0(x , y +3);
	return 5;
}
int block6(unsigned char x, unsigned char y)
{
	block0(x + 1, y);
	block0(x + 1, y + 1);
	block0(x + 1, y + 2);
	block0(x, y + 1);
	return 6;
}
int block7(unsigned char x, unsigned char y)
{
	block0(x, y);
	block0(x + 1, y);
	block0(x + 2, y);
	block0(x + 1, y + 1);
	return 7;
}
int block8(unsigned char x, unsigned char y)
{
	block0(x, y);
	block0(x, y + 1);
	block0(x, y + 2);
	block0(x + 1, y + 1);
	return 8;
}
int block9(unsigned char x, unsigned char y)
{
	block0(x + 1, y);
	block0(x + 1, y + 1);
	block0(x + 1, y + 2);
	block0(x, y + 2);
	return 0;
}
int block10(unsigned char x, unsigned char y)
{
	block0(x, y);
	block0(x + 1, y);
	block0(x + 2, y);
	block0(x + 2, y + 1);
	return 10;
}
int block11(unsigned char x, unsigned char y)
{
	block0(x, y);
	block0(x + 1, y);
	block0(x, y + 1);
	block0(x, y + 2);
	return 11;
}
int block12(unsigned char x, unsigned char y)
{
	block0(x, y);
	block0(x, y + 1);
	block0(x + 1, y + 1);
	block0(x + 1, y + 2);
	return 12;
}
int block13(unsigned char x, unsigned char y)
{
	block0(x, y + 1);
	block0(x + 1, y);
	block0(x + 1, y + 1);
	block0(x + 2, y);
	return 13;
}
int block14(unsigned char x, unsigned char y)
{
	block0(x, y);
	block0(x, y + 1);
	block0(x + 1, y + 1);
	block0(x + 1, y + 2);
	return 14;
}
int block15(unsigned char x, unsigned char y)
{
	block0(x, y);
	block0(x + 1, y);
	block0(x + 2, y);
	block0(x + 3, y);
	return 15;
}
void blockadd(unsigned char x, unsigned char y, unsigned char num)//block이 바닥에 도착하거나 다른 블럭 위에 도착했을 때 쌓인 블럭을 저장하는 함수
{
		switch(num)
		{
			case 1:
				block[x][y] = 1;
				block[x][y+1] = 1;
				block[x+1][y] = 1;
				block[x+1][y+1] = 1;
				break;
			case 2:
				block[x + 1][y] = 1;
				block[x][y + 1] = 1;
				block[x + 1][y + 1] = 1;
				block[x + 2][y + 1] = 1;
				break;
			case 3:
				block[x][y] = 1;
				block[x][y + 1] = 1;
				block[x + 1][y + 1] = 1;
				block[x + 2][y + 1] = 1;
				break;
			case 4:
				block[x + 1][y] = 1;
				block[x + 2][y] = 1;
				block[x][y + 1] = 1;
				block[x + 1][y + 1] = 1;
				break;
			case 5:
				block[x][y] = 1;
				block[x][y + 1] = 1;
				block[x][y + 2] = 1;
				block[x][y + 3] = 1;
				break;
			case 6:
				block[x + 1][y] = 1;
				block[x + 1][y + 1] = 1;
				block[x + 1][y + 2] = 1;
				block[x][y + 1] = 1;
				break;
			case 7:
				block[x][y] = 1;
				block[x + 1][y] = 1;
				block[x + 2][y] = 1;
				block[x + 1][y + 1] = 1;
				break;
			case 8:
				block[x][y] = 1;
				block[x][y + 1] = 1;
				block[x][y + 2] = 1;
				block[x + 1][y + 1] = 1;
				break;
			case 9:
				block[x + 1][y] = 1;
				block[x + 1][y + 1] = 1;
				block[x + 1][y + 2] = 1;
				block[x][y + 2] = 1;
				break;
			case 10:
				block[x][y] = 1;
				block[x + 1][y] = 1;
				block[x + 2][y] = 1;
				block[x + 2][y + 1] = 1;
				break;
			case 11:
				block[x][y] = 1;
				block[x + 1][y] = 1;
				block[x][y + 1] = 1;
				block[x][y + 2] = 1;
				break;
			case 12:
				block[x][y] = 1;
				block[x][y + 1] = 1;
				block[x + 1][y + 1] = 1;
				block[x + 1][y + 2] = 1;
				break;
			case 13:
				block[x][y + 1] = 1;
				block[x + 1][y] = 1;
				block[x + 1][y + 1] = 1;
				block[x + 2][y] = 1;
				break;
			case 14:
				block[x][y] = 1;
				block[x][y + 1] = 1;
				block[x + 1][y + 1] = 1;
				block[x + 1][y + 2] = 1;
				break;
			case 15:
				block[x][y] = 1;
				block[x + 1][y] = 1;
				block[x + 2][y] = 1;
				block[x + 3][y] = 1;
				break;
		}
}
int draw_block()//떨어지는 블럭을 화면에 그려주는 함수
{
	switch(blocktype)
	{
		case 1:
			if(block[main_x][main_y + 1] == 0 && block[main_x + 1][main_y + 1] == 0 && main_x < 8 && main_y < 14)
				block1(main_x, main_y);
			else if(block[main_x][main_y + 1] == 1 || block[main_x + 1][main_y + 1] == 1)
			{
				blockadd(main_x, main_y-1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 2:
			if(block[main_x][main_y + 1] == 0 && block[main_x + 1][main_y + 1] == 0 && block[main_x + 2][main_y + 1] == 0 && main_x < 7 && main_y < 14)
				block2(main_x, main_y);
			else if(block[main_x][main_y + 1] == 1 || block[main_x + 1][main_y + 1] == 1 || block[main_x+2][main_y+1] == 1)
			{
				blockadd(main_x, main_y-1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 3:
			if(block[main_x][main_y + 1] == 0 && block[main_x + 1][main_y + 1] == 0 && block[main_x + 2][main_y + 1] == 0 && main_x < 7 && main_y < 14)
				block3(main_x, main_y);
			else if(block[main_x][main_y + 1] == 1 || block[main_x + 1][main_y + 1] == 1 || block[main_x+2][main_y+1] == 1)
			{
				blockadd(main_x, main_y-1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 4:
			if(block[main_x][main_y + 1] == 0 && block[main_x + 1][main_y + 1] == 0 && block[main_x + 2][main_y] == 0 && main_x < 7 && main_y < 14)
				block4(main_x, main_y);
			else if(block[main_x][main_y + 1] == 1 || block[main_x + 1][main_y + 1] == 1 || block[main_x+2][main_y] == 1)
			{
				blockadd(main_x, main_y-1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 5:
			if(block[main_x][main_y + 3] == 0 && block[main_x][main_y + 2] == 0 && block[main_x][main_y + 1] == 0 && x_left < 8 && main_y < 12)
				block5(main_x, main_y);
			else if(block[main_x][main_y + 3] == 1 || block[main_x][main_y + 2] == 1 || block[main_x][main_y] == 1)
			{
				blockadd(main_x, main_y-1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 6:
			if(block[main_x + 1][main_y + 2] == 0 && block[main_x][main_y + 1] == 0 && main_x < 7 && main_y < 13)
				block6(main_x, main_y);
			else if(block[main_x + 1][main_y + 2] == 1 || block[main_x][main_y + 1] == 1)
			{
				blockadd(main_x, main_y - 1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 7:
			if(block[main_x + 2][main_y] == 0 && block[main_x + 1][main_y + 1] == 0 && block[main_x][main_y] == 0 && main_x < 6 && main_y < 14)
				block7(main_x, main_y);
			else if(block[main_x + 2][main_y] == 1 || block[main_x + 1][main_y + 1] == 1 || block[main_x][main_y] == 1)
			{
				blockadd(main_x, main_y - 1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 8:
			if(block[main_x + 1][main_y + 1] == 0 && block[main_x][main_y + 2] == 0 && main_x < 7 && main_y < 13)
				block8(main_x, main_y);
			else if(block[main_x + 1][main_y + 1] == 1 || block[main_x][main_y + 2] == 1)
			{
				blockadd(main_x, main_y - 1, blocktype);
				return blocktype;
			}
			else
			{	
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 9:
			if(block[main_x + 1][main_y + 2] == 0 && block[main_x][main_y + 2] == 0 && main_x < 7 && main_y < 13)
				block9(main_x, main_y);
			else if(block[main_x + 1][main_y + 2] == 1 || block[main_x][main_y + 2] == 1)
			{
				blockadd(main_x, main_y - 1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 10:
			if(block[main_x + 2][main_y + 1] == 0 && block[main_x + 1][main_y] == 0 && block[main_x][main_y] == 0 && main_x < 6 && main_y < 14)
				block10(main_x, main_y);
			else if(block[main_x + 2][main_y + 1] == 1 || block[main_x + 1][main_y] == 1 || block[main_x][main_y] == 1)
			{
				blockadd(main_x, main_y - 1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 11:
			if(block[main_x + 1][main_y] == 0 && block[main_x][main_y + 2] == 0 && main_x < 7 && main_y < 13)
				block11(main_x, main_y);
			else if(block[main_x + 1][main_y] == 1 || block[main_x][main_y + 2] == 1)
			{
				blockadd(main_x, main_y - 1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 12:
			if(block[main_x + 1][main_y + 2] == 0 && block[main_x][main_y + 1] == 0 && main_x < 7 && main_y < 13)
				block12(main_x, main_y);
			else if(block[main_x + 1][main_y + 2] == 1 || block[main_x][main_y + 1] == 1)
			{
				blockadd(main_x, main_y - 1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 13:
			if(block[main_x + 2][main_y] == 0 && block[main_x + 1][main_y + 1] == 0 && block[main_x][main_y + 1] == 0 && main_x < 6 && main_y < 14)
				block13(main_x, main_y);
			else if(block[main_x + 2][main_y] == 1 || block[main_x + 1][main_y + 1] == 1 || block[main_x][main_y + 1] == 1)
			{
				blockadd(main_x, main_y - 1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 14:
			if(block[main_x + 1][main_y + 2] == 0 && block[main_x][main_y + 1] == 0 && main_x < 7 && main_y < 14)
				block14(main_x, main_y);
			else if(block[main_x + 1][main_y + 2] == 1 || block[main_x][main_y + 1] == 1)
			{
				blockadd(main_x, main_y - 1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
		case 15:
			if(block[main_x + 3][main_y] == 0 && block[main_x + 2][main_y] == 0 && block[main_x + 1][main_y] == 0 && block[main_x][main_y] == 0 && main_x < 5 && main_y < 15)
				block15(main_x, main_y);
			else if(block[main_x + 3][main_y] == 1 || block[main_x + 2][main_y] == 1 || block[main_x + 1][main_y] == 1 || block[main_x][main_y] == 1)
			{
				blockadd(main_x, main_y - 1, blocktype);
				return blocktype;
			}
			else
			{
				blockadd(main_x, main_y, blocktype);
				return blocktype;
			}
			break;
	}
	
	for(int i = 8; i >= 0; i--)
		for(int j = 15; j >= 0; j--)
			if(block[i][j] == 1)
				block0(i, j);
	return 0;
}
void gameStart()//게임 시작 시 실행되는 함수
{
	lcd_xy(0,0);
	int x_max = 8, y_max = 15;//x : 0 ~ 8, y : 0 ~ 15
	int clear;
	int levelup = 5;
	int key = 0;
	int cnt = 0;
	int temp;
	int gameover = 0;
	srand(key);
	main_x = 3;
	main_y = 0;
	
	score = 0;
	blocktype = rand() % 5 + 1;
	if(blocktype == 1)
		x_left = main_x + 1;
	else if(blocktype == 2 || blocktype == 3 || blocktype == 4)
		x_left = main_x + 2;
	else if(blocktype == 5)
		x_left = main_x;
		
	while(1)
	{		
			lcd_clear();
			ScreenBuffer_clear();
			//lcd_xy(15, 0);
			lcd_char_rotate(levelup);
			//lcd_xy(0,0);
			if(draw_block() != 0)
			{
				S_S3();
				cnt++;
				main_x = 3;
				main_y = -1;
				
				srand(key);
				blocktype = rand() % 6 + 1;
				if(blocktype == 6)
					blocktype--;
					
				if(blocktype == 1)
				x_left = main_x + 1;
				else if(blocktype == 2 || blocktype == 3 || blocktype == 4)
				x_left = main_x + 2;
				else if(blocktype == 5)
				x_left = main_x;
				
				key = 0;
				//한 줄이 채워졌는지 확인
				for(int i = 15; i > 0; i--)
				{
					clear = 0;
					for(int j = 0; j <= x_max; j++)
						if(block[j][i] == 1)
							key += j + i*7;
					
					for(int j = 0; j <= x_max; j++)
						if(block[j][i] == 1)
						{
							
							clear++;
						}
						else
							break;
					
					//한줄완성		
					if(clear == 9)
					{
						S_S1();
						score++;
						for(int j = i; j > 0; j--)
							for(int k = 0; k < x_max; k++)
								block[k][j] = block[k][j-1];
						i++;
						levelup--;
						if(levelup == 0)
						{
							PORTB = PORTB >> 1;
							levelup = 5;
							level++;
						}
					}
				}
				if(cnt == 4)//4번째 블럭마다 쌓인 블럭들을 랜덤하게 이동함(왼쪽 shift, 오른쪽 shift, 좌우 반전)
				{
					if(key % 3 == 0)
					{
						for(int i = 15; i > 0; i--)
						{
							temp = block[x_max][i];
							for(int k = x_max; k > 0; k--)
							block[k][i] = block[k - 1][i];
							block[0][i] = temp;
						}
						
					}
					else if(key % 3 == 1)
					{
						for(int i = 15; i > 0; i--)
						{
							temp = block[0][i];
							for(int k = 0; k < x_max; k++)
							block[k][i] = block[k + 1][i];
							block[x_max][i] = temp;
						}
					}
					else if(key % 3 == 2)
					{
						for(int i = 15; i > 0; i--)
						{
							for(int k = 0; k < x_max / 2; k++)
							{
								temp = block[k][i];
								block[k][i] = block[x_max - k][i];
								block[x_max - k][i] = temp;
							}
						}
					}
					cnt = 0;
				}
			}
			
			//_delay_ms(500);
			joystick_control();
			main_y++;//block moves down
			//x++;//block mvoes left
		
		for(int i = 0; i <= x_max; i++)
			if(block[i][0] == 1)
				gameover = 1;
		if(gameover)
			break;
}
}
void gameEnd()//게임이 끝난 후 점수를 출력하는 함수
{
	lcd_clear();
	ScreenBuffer_clear();
	lcd_xy(6, 8);
	lcd_char_rotate(17);//S
	lcd_xy(5, 8);
	lcd_char_rotate(12);//C
	lcd_xy(4, 8);
	lcd_char_rotate(20);//O
	lcd_xy(3, 8);
	lcd_char_rotate(15);//R
	lcd_xy(2, 8);
	lcd_char_rotate(16);//E
	lcd_xy(5, 10);
	lcd_char_rotate(score / 100);
	lcd_xy(4, 10);
	lcd_char_rotate((score / 10) % 10);
	lcd_xy(3, 10);
	lcd_char_rotate(score % 10);
	while(1);
}
void gameTitle()//게임 시작 시에 나오는 게임 타이틀
{
	glcd_draw_bitmap(tetris, 0, 20, 64, 24);
	lcd_xy(6, 13);
	lcd_char_rotate(14);//P
	lcd_xy(5, 13);
	lcd_char_rotate(15);//R
	lcd_xy(4, 13);
	lcd_char_rotate(16);//E
	lcd_xy(3, 13);
	lcd_char_rotate(17);//S
	lcd_xy(2, 13);
	lcd_char_rotate(17);//S
	lcd_xy(5, 15);
	lcd_char_rotate(11);//B
	lcd_xy(4, 15);
	lcd_char_rotate(18);//T
	lcd_xy(3, 15);
	lcd_char_rotate(19);//N
	lcd_xy(2, 15);
	lcd_char_rotate(0);//0
	while(PIND & (0x01) != 0)
	{
		S_S2();
	}
}
int main(void)
{
	init_devices();
	DDRB = 0xff;
	PORTB = 0x7f;
	
	//start 화면
	for(int i = 0; i < 9; i++)
	{
		blank[i] = 14;
		for(int j = 0; j < 16; j++)
		{
			block[i][j] = 0;
		}
	}

	lcd_clear();
	ScreenBuffer_clear();
	//GLCD_Line(0, 15, 63, 15);
	//lcd_xy(7, 0);
	//lcd_string_rotate(1, 0, "ABCD");
	
	//_delay_ms(10000);
	/* Replace with your application code */
	//lcd_xy(5, 5);
	gameTitle();
	//lcd_clear();
	//ScreenBuffer_clear();
	gameStart();
	gameEnd();
	while (1)
	{
		
	}
	return 0;
}