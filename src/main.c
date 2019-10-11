#include<stdint.h>
#include"tm4c123gh6pm.h"

int increase=1,cur=0;
int digit_hex[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

void delayMs(int);
void DisableInterrupts(void);
void EnableInterrupts(void);
void key_interrupt_Handler(void);
void initInterrupt_keys();
void init_7_segment();
void init_keypad();
void initInterrupt_keys();
void set_digits(int*);

void delayMs(int n)
{
int i, j;
for(i = 0 ; i < n; i++)
for(j = 0; j < 3180; j++) {}
}

void set_digits(int digits[])
{
    int i,j;
    for(j=0;j<60;j++){
        for (i=0;i<4;i++){
            GPIO_PORTB_DATA_R =digit_hex[digits[i]];
            GPIO_PORTA_DATA_R =1<<(i+4);
                delayMs(2);
        }
    }
}

void display_time(int digits[],int cur)
{
    int seconds=cur%60,minutes=cur/60;
    digits[0]=seconds%10;digits[1]=seconds/10;
    digits[2]=minutes%10;digits[3]=minutes/10;
    set_digits(digits);
}

void init_keypad()
{
        SYSCTL_RCGC2_R |= 0x00000014; // GPIO clock on PORT C and PORT E
        GPIO_PORTE_DIR_R |= 0x0F; // set PORT E (E0-E3) as output pins (by setting them as 1)
        GPIO_PORTE_ODR_R |= 0x0F; // set PORT E (E0-E3) as open drain output
        GPIO_PORTE_DEN_R |= 0x0F; // digital enable set for PORT E(E0-E3)
        GPIO_PORTC_DIR_R &= 0x0F; // set PORT C (C4-C7) as input pins (by setting them as 0)
        GPIO_PORTC_PUR_R |= 0xF0; // set PORT C (C4-C7) as pull-up
        GPIO_PORTC_DEN_R |= 0xF0; // // digital enable set for PORT C(C4-C7)
}

void init_7_segment()
{
    SYSCTL_RCGC2_R |= 0x00000003;
    GPIO_PORTB_DIR_R |= 0xFF;
    GPIO_PORTB_DEN_R |= 0xFF;
    GPIO_PORTA_DIR_R |= 0xF0;
    GPIO_PORTA_DEN_R |= 0xF0;
}

void DisableInterrupts(void)
{
    __asm ("CPSID  I\n");
}

void EnableInterrupts(void)
{
    __asm  ("CPSIE  I\n");
}

void initInterrupt_keys()
{
    SYSCTL_RCGC2_R |= 0x00000014; // GPIO clock on PORT C and PORT E
    GPIO_PORTE_DIR_R |= 0x0F; // set PORT E (E0-E3) as output pins (by setting them as 1)
    GPIO_PORTE_ODR_R |= 0x0F; // set PORT E (E0-E3) as open drain output
    GPIO_PORTE_DEN_R |= 0x0F; // digital enable set for PORT E(E0-E3)
    GPIO_PORTC_DIR_R &= 0x0F; // set PORT C (C4-C7) as input pins (by setting them as 0)
    GPIO_PORTC_PUR_R |= 0xF0; // set PORT C (C4-C7) as pull-up
    GPIO_PORTC_DEN_R |= 0xF0; // // digital enable set for PORT C(C4-C7)

    GPIO_PORTC_IS_R &= 0x8F; // Edge sensitive
    GPIO_PORTC_IBE_R &= 0x8F; // Not both edges
    GPIO_PORTC_IEV_R &= 0x8F; // Falling edge (HIGH to LOW)
    GPIO_PORTC_ICR_R = 0x70; // Clear interrupt flag
    GPIO_PORTC_IM_R |= 0x70; //  unmask interrupt

    NVIC_PRI0_R = (NVIC_PRI0_R  & 0xFF1FFFFF) | 0x00A00000;
    NVIC_EN0_R = 1<<2; // Enable interrupt 2
    EnableInterrupts();
}

void key_interrupt_Handler(void)
{
    volatile int readback;

    GPIO_PORTC_ICR_R = 0x70;
    switch(GPIO_PORTC_DATA_R>>4)
      {
        case 0xE:increase=0;break;
        case 0xD:increase=1;break;
        case 0xB:cur=0;break;
      }
    readback = GPIO_PORTC_ICR_R;    /* a read to force clearing of interrupt flag */
}

int main(void)
{
    int digits[4];
    init_7_segment();
    initInterrupt_keys();
    GPIO_PORTE_DATA_R = ~1;
    while(1) {
            display_time(digits,cur);
            cur=cur+increase;
    }
    return 0;
}
