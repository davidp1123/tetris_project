/*
 * _main.h
 *
 * Created: 2016-12-18
 * Author : 12121561정헌휘 12123953윤태훈
 */ 

#include <avr/io.h>
#define F_CPU 14745600UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#define nop()  __asm__ __volatile__ ("nop" ::)
#define NOP()  __asm__ __volatile__ ("nop" ::)
#define _NOP()  __asm__ __volatile__ ("nop" ::)

#define CLI() cli()
#define SEI() sei()
#define BIT(x)	(1 << (x))

typedef unsigned char		byte;
typedef unsigned char		u8;
typedef signed int		s16;
typedef unsigned int		u16;
typedef signed long		s32;
typedef unsigned long		u32;
typedef signed long long	s64;
typedef unsigned long long 	u64;

#define sbi(port, bit) 	(port |= (1<<bit))
#define cbi(port, bit) 	(port &= (~(1<<bit)))
#define inp(port, bit) 		(port & (1<<bit))
// bit 매크로
#define SetBit(x,y)  (x|=(1<<y))
#define ClrBit(x,y)  (x&=~(1<<y))
#define ToggleBit(x,y) (x^=(1<<y))
#define FlipBit(x,y) (x^=(1<<y)) // Same as ToggleBit.
#define TestBit(x,y) (x&(1<<y))

// bit 매크로
#define SETBIT(x,y)  (x|=(1<<y))
#define CLEARBIT(x,y)  (x&=~(1<<y))
#define TOGGLEBIT(x,y) (x^=(1<<y))
#define FLIPBIT(x,y) (x^=(1<<y)) // Same as ToggleBit.
#define TESTBIT(x,y) (x&(1<<y))

extern unsigned int INT0_DataView;
extern unsigned int INT1_DataView;
extern unsigned int Uart1_DataView;


void joystick_move_menu(void); //조이스틱의 방향을 리턴해주는 함수
void clear(unsigned ); //각 스테이지 성공시 호출
void fail(void); //각 스테이지 실패시 호출
void tutorials(void); //튜토리얼 모드 실행시 호출
void setup_level(void); //게임 난이도 설정
void start_game(); //게임 시작시 호출 (이미지 출력)
void game_left_frame(int); //게임화면에서 left frame 부분의 캐릭터 춤동작
void game_right_frame(int); //게임화면에서 right frame 부분의 캐릭터 춤동작
void game_frame(); //게임시 frame출력 함수
int game_start_level1(); //game level 1 start
int game_start_level2();  //game level 2 start
int game_start_level3(); //game level 3 start
int game_start_level4();  //game level 4 start
int game_start_level5();   //game level 5 start
void menu(void); //메뉴화면