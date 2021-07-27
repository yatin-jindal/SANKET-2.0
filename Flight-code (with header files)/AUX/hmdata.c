#include "hmdata.h"
#include "adc.h"
#include "uart.h"
#include "avr_compiler.h"

extern uint8_t hmData[18];
extern uint16_t timeCounter1;
extern uint8_t opMode;
extern uint8_t Uplink_Rx;
extern uint8_t CC_status;

void HM_data_check_send(void){
	hmData[0] = (timeCounter1 & 0x00FF);
	hmData[1] = (timeCounter1>>8);
	hmData[2] = (Uplink_Rx<<7) | (CC_status<<4) | opMode;   // the different HM data are collected and stored at different places 
	hmData[3] |= (PINA & 0x0F);                             // here the EN and OC of the current limiters are connected in the 4 LSB of Port A
	adcMeasure(4,4);
	adcMeasure(5,5);
	adcMeasure(6,6);
  
	for(int j=0;j<18;j++){
		uart_transmit(hmData[j]);
  }
}