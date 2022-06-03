#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ST7735_TFT.h"
#include "I2C_Support.h"
#include "Setup_Time.h"
#include "Main_Screen.h"
#include "Main.h"
#include "utils.h"


extern unsigned char second, minute, hour, dow, day, month, year;
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;

extern char setup_time[];
extern char setup_date[];

extern char buffer[22];
extern char *nbr;
extern char *txt;
extern char tempC[];
extern char tempF[];
extern char time[];
extern char date[];
extern char found;
extern short nec_ok;

char In_Time_Setup;
char Select_Field;  

void Do_Setup_Time(void)
{

    Select_Field = 0;                           // start at field 0

    DS3231_Read_Time();                         // Read actual time
    setup_second = bcd_2_dec(second);           // Set setup time as current time
    setup_minute = bcd_2_dec(minute); // add code here          // convert all the bcd to real integer
    setup_hour = bcd_2_dec(hour); // add code here            //
    setup_day = bcd_2_dec(day); // add code here 
    setup_month = bcd_2_dec(month); // add code here 
    setup_year = bcd_2_dec(year); // add code here 
    
    Initialize_Setup_Time_Screen();             // Initialize setup screen
    Update_Setup_Time_Screen();                 // Update screen with latest information
    In_Time_Setup = 1;                          // set variable to stay in setup mode
    
    while (In_Time_Setup == 1)
    {   
         if (check_for_button_input() == 1)
         {

             if (found >= Prev && found <= EQ)
             {
                Do_Beep_Good(); 
                if (found == Prev) Go_Prev_Field();
// add code here to check if 'Next' then use Go_Next_Field(); 
                if (found == Next) Go_Next_Field();
// add code here to check if 'Play/Pause' then use Do_Save_New_Time(); 
                if (found == Play_Pause) Do_Save_New_Time();
// add code here to check if 'Minus' then use Decrease_Time();
                if (found == Minus) Decrease_Time();
// add code here to check if 'Plus' then use Increase_Time();
                if (found == Plus) Increase_Time();
// add code here to check if 'EQ' Exit_Time_Setup();
                if (found == EQ) Exit_Time_Setup();
                found = 0xff;
             }
             else
             {
                 Do_Beep_Bad();
             }
        }                   
    }
}

void Increase_Time()
{
            switch (Select_Field)               // Increment the proper field
            {
                case 0:
                    setup_hour++;
                    if (setup_hour == 24) setup_hour = 0; 
                    break;

                case 1:
                    setup_minute++;
                    if (setup_hour == 60) setup_minute = 0;
                    break; // add code here to handle minute increment

                case 2:
                      setup_second++;
                    if (setup_second == 60) setup_second = 0; 
                    break; // add code here to handle second increment

                case 3:
                      setup_month++;
                    if (setup_month == 12) setup_month = 0; 
                    break;
                    // add code here to handle month increment

                case 4:
                      setup_day++;
                    if (setup_day == 31) setup_day = 0; 
                    break;
                    // add code here to handle day increment   

                case 5:
                      setup_year++;
                    
                    break;
                    // add code here to handle year increment

                default:
                    break;
            }    
            Update_Setup_Time_Screen();         // Update screen with latest info
}

void Decrease_Time()
{
            switch (Select_Field)               // Decrement the proper field
            {
                case 0:
                    if (setup_hour == 0) setup_hour = 23;
                    else --setup_hour;
                    break;
                    
                case 1:
                    if (setup_minute == 0) setup_minute = 61;
                    else --setup_minute;
                    break;
                    
                case 2:
                    if (setup_second == 0) setup_second = 61;
                    else --setup_second;
                    break;
                    
                case 3:
                    if (setup_month == 0) setup_hour = 13;
                    else --setup_month;
                    break;
                    
                case 4: 
                    if (setup_day == 0) setup_day = 32;
                    else --setup_day;
                    break;
                    
                case 5:
                     --setup_year;
                    break;


                default:
                    break;
            }                
            Update_Setup_Time_Screen();                 // Update screen with latest info
        } 

void Go_Next_Field()
{
        
    Select_Field++;                                     // Move to next field
    if (Select_Field == 6) Select_Field = 0;            // Wrap around if necessary
    Update_Setup_Screen_Cursor_Forward(Select_Field);           // Update cursor

}  

void Go_Prev_Field()
{
    if (Select_Field == 0) Select_Field = 5;
    else Select_Field--;                                // Move to next field
    Update_Setup_Screen_Cursor_Backward(Select_Field);  // Update cursor
} 

void Do_Save_New_Time()
{
    DS3231_Write_Time();                                // Write time
    DS3231_Read_Time();                                 // Read time
    Initialize_Screen();                                // Initialize the screen before returning  
    In_Time_Setup = 0;
}
    
void Exit_Time_Setup()
{
    DS3231_Read_Time();                                 // Read time
    Initialize_Screen();                                // Initialize the screen before returning  
    In_Time_Setup = 0;
}
void Initialize_Setup_Time_Screen(void) 
{ 
    fillScreen(ST7735_BLACK);                           // Fills background of screen with color passed to it
 
    strcpy(txt, "ECE3301L Sp'22 Sx\0");               // Text displayed 
    drawtext(start_x , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);
                                                        // X and Y coordinates of where the text is to be displayed

    strcpy(txt, "Time Setup\0");                        // Text displayed 
    drawtext(start_x+3 , start_y+15, txt, ST7735_MAGENTA, ST7735_BLACK, TS_2); 
       
    strcpy(txt, "Time");
    drawtext(time_x  , time_y , txt, ST7735_BLUE   , ST7735_BLACK, TS_1);
    
    fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
    strcpy(txt, "Date");
    drawtext(date_x  , date_y , txt, ST7735_RED    , ST7735_BLACK, TS_1);
}

void Update_Setup_Time_Screen(void)
{
    setup_time[0]  = (setup_hour/10)  + '0';
    setup_time[1]  = (setup_hour % 10)  + '0';                    // add code here  
 
    setup_time[3]  = (setup_minute/10)  + '0';                     // add code here 
    setup_time[4]  = (setup_minute % 10)  + '0';                    // add code here 

    setup_time[6]  = (setup_second/10)  + '0';                     // add code here 
    setup_time[7]  = (setup_second % 10)  + '0';                     // add code here
 
    setup_date[0]  = (setup_month/10)  + '0';                     // add code here 
    setup_date[1]  = (setup_month % 10)  + '0';                    // add code here 

    setup_date[3]  = (setup_day/10)  + '0';                     // add code here 
    setup_date[4]  = (setup_day % 10)  + '0';                    // add code here 

    setup_date[6]  = (setup_year/10)  + '0';                     // add code here 
    setup_date[7]  = (setup_year % 10)  + '0';			    // add code here

    drawtext(data_time_x, data_time_y, setup_time, ST7735_CYAN, ST7735_BLACK, TS_2);
    drawtext(data_date_x, data_date_y, setup_date, ST7735_GREEN, ST7735_BLACK, TS_2);
}

void Update_Setup_Screen_Cursor_Forward(char field)
{
    char xinc = 36;
    char yinc = 30;   
    switch (field)
    {
        case 0:
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
            break;
            
        case 1:
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break; 
            
        case 2:
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break;  
            
        case 3:
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;   
            
       case 4:
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;  

       case 5:
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;                           
    }
}

void Update_Setup_Screen_Cursor_Backward(char field)
{
    char xinc = 36;
    char yinc = 30;   
    switch (field)
    {
        case 0:
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
            break;
            
        case 1:
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break; 
            
        case 2:
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break;  
            
        case 3:
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;   
            
       case 4:
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;  

       case 5:
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_BLACK);           
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;              
    }
}


