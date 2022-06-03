#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"
#include "utils.h"

extern char FAN;
extern char HEATER;
extern char duty_cycle;
extern char heater_set_temp;
extern signed int DS1621_tempF;

int get_duty_cycle(signed int temp, int set_temp)
{
    int diff;
    if (temp >= set_temp)
    {
        duty_cycle = 0;
    }
    else if (temp <= set_temp)
    {
        diff = set_temp - temp;
        if (diff > 50)
        {
            duty_cycle = 100;
        }
        else if (diff >=25 && diff <50)
        {
            duty_cycle = diff*2;
        }
        else if (diff >=10 && diff <25)
        {
            duty_cycle = diff*3/2;
        }
        else if (diff >=0 && diff <10)
        {
            duty_cycle = diff;
        }
              
    }
    return duty_cycle;
}

void Monitor_Heater()
{
    duty_cycle = get_duty_cycle(DS1621_tempF, heater_set_temp);
    do_update_pwm(duty_cycle);
    
    if (HEATER == 1) 
    {
        if(DS1621_tempF < heater_set_temp) Turn_On_Fan();
    }
    else Turn_Off_Fan();

}

void Toggle_Heater()
{
   if (HEATER == 0){
        Turn_On_Fan();
    }
    else {
        Turn_Off_Fan();
    }
}

int get_RPM()
{
    int RPS = TMR3L/2;
    
    TMR3L = 0;
    return(RPS * 60);
}

void Toggle_Fan()
{
    if (FAN == 0){
        Turn_On_Fan();
    }
    else {
        Turn_Off_Fan();
    }
}


void Turn_Off_Fan()
{
    HEATER =0;
    FAN = 0;
        do_update_pwm(duty_cycle);
    FAN_EN = 0;
    //FAN_LED = 0;
}

void Turn_On_Fan()
{
    HEATER = 1;
    FAN = 1;
    do_update_pwm(duty_cycle);
    FAN_EN = 1;
   // FAN_LED = 1;
}

void Increase_Speed()
{
    if (duty_cycle == 100)
    {
        Do_Beep();
        Do_Beep();
        FAN_PWM = 1;
        do_update_pwm(duty_cycle);
    }
    else
    {
        duty_cycle = duty_cycle + 5;
        do_update_pwm(duty_cycle);
    
    }
    
}

void Decrease_Speed()
{
    if(duty_cycle == 0)
    {
        Do_Beep();
        Do_Beep();
        FAN_PWM = 1;
        do_update_pwm(duty_cycle);
        // reprogram duty cycle, D cycle must remain at 0
    }
    else
    {
        duty_cycle = duty_cycle - 5;
        do_update_pwm(duty_cycle);
    }
        
}


void Set_DC_RGB(int duty_cycle)
{
    int d1color_I;
    unsigned char d1color_C = 0x00;
    d1color_I = duty_cycle / 10;
    if (d1color_I >7) d1color_I =7;
    
    //d1color_C = (char) d1color_I;
    d1color_C = (char) d1color_I << 1;
    PORTA = d1color_C;
}

void Set_RPM_RGB(int rpm)
{
   int d2color_I;
    int D2color[8] = {0x08, 0x18, 0x10, 0x20, 0x28, 0x30, 0x38, 0x00};
    char MASKD;
    if (rpm == 0)   d2color_I = 8;
    else
    {
        d2color_I = rpm / 500;
        if (d2color_I >7) d2color_I =7;
    }

    MASKD = PORTD & 0xC7;
    PORTD = D2color[d2color_I] | MASKD;
 
}

