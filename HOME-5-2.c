#include <stdio.h>
#include "tm4c123gh6pm.h"
#define SWITCH (*((volatile uint32_t *)0x4002400C))
#define LIGHT (*((volatile uint32_t *)0x4000501C))
	
#define Idle &FSM[0]
#define Ready &FSM[1]
#define StandBy &FSM[2]
// PE0 and PE1 used for SWITCHES. when 
//PE1=0, PE0=0 input 00
//PE1=0, PE0=1 input 01
//PE1=1, PE0=0 input 10
//PE1=1, PE0=1 input 11
// PORT B from 0 to 2pins has been used for output 

void SysTick_Init(void);
void SysTick_Wait(uint32_t delay);
void PORTE_init(void);
void PORTB_init(void);
void delayMs( unsigned int dTime );

struct State {
uint32_t Out;
uint32_t Time;
const struct State *Next[4];};
typedef const struct State STyp;


STyp FSM[3]={
{0x01,16777216,{Idle,Idle,Ready,Idle}},
{0x04,16777216,{Ready,Idle,Ready,StandBy}},
{0x02,16777216,{Ready,Idle,Ready,StandBy}}
};
int main(void){
	STyp *Pt; // state pointer
	PORTB_init();
	PORTE_init();
  SysTick_Init();
  uint32_t Input;
	Pt = Ready;
	while(1){
		LIGHT = Pt ->Out;
		SysTick_Wait(Pt->Time);
		Input = SWITCH;
		Pt=Pt->Next[Input];
	}
}


void SysTick_Init(void){
NVIC_ST_CTRL_R = 0; // 1) disable SysTick during setup
NVIC_ST_RELOAD_R = 0x00FFFFFF; // 2) maximum reload value
NVIC_ST_CURRENT_R = 0; // 3) any write to current clears it
NVIC_ST_CTRL_R = 0x00000005; // 4) enable SysTick with core clock
}
void SysTick_Wait(uint32_t delay){
NVIC_ST_RELOAD_R = delay-1; // number of counts to wait
NVIC_ST_CURRENT_R = 0; // any value written to CURRENT clears
while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
}
}


void PORTE_init(){
	SYSCTL_RCGCGPIO_R |= 0x10;
	GPIO_PORTE_DIR_R = ~0x03;
	GPIO_PORTE_DEN_R = 0x03;
}
void PORTB_init(){
	SYSCTL_RCGCGPIO_R |= 0x02;
	GPIO_PORTB_AMSEL_R = ~0x07;
	GPIO_PORTB_PCTL_R =~0x00000FFF;
	GPIO_PORTB_DIR_R = 0x07;
	GPIO_PORTB_AFSEL_R = ~0x07;
	GPIO_PORTB_DEN_R = 0x07;
}

