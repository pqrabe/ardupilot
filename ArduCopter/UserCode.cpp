/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Copter.h"
#include <AP_HAL/AP_HAL.h>
#include <AP_SerialManager/AP_SerialManager.h>
AP_HAL::UARTDriver *uart = nullptr;
extern const AP_HAL::HAL& hal;
//const AP_HAL::HAL& hal = AP_HAL::get_HAL();
char c;
volatile uint32_t forward;
volatile uint32_t right;
volatile uint32_t left;
volatile uint32_t back;

volatile uint32_t DataFromCont;
volatile float Roll;
volatile float Pitch;

int ftime;
int rtime;
int ltime;
int btime;

void Printfunc();
char Valid(char in);


#ifdef USERHOOK_INIT
void Copter::userhook_init()
{
    c = 0;
    forward = 9999;
    right = 9999;
    left = 9999;
    back = 9999;

    ftime = 0;
    rtime = 0;
    ltime = 0;
    btime = 0;

    DataFromCont = 99;
    Roll = -1;
    Pitch = -1;
    // put your initialisation code here
    // this will be called once at start-up
    //uart = serial_manager.find_serial(AP_SerialManager::SerialProtocol_GPS2, 0);
    //if (uart != nullptr) {
    //    uart->begin(serial_manager.find_baudrate(AP_SerialManager::SerialProtocol_GPS2, 0));
    //}
}
#endif

#ifdef USERHOOK_FASTLOOP
void Copter::userhook_FastLoop()
{
    // put your 100Hz code here
}
#endif

#ifdef USERHOOK_50HZLOOP
void Copter::userhook_50Hz()
{
    // put your 50Hz code here
    Printfunc();
}
#endif

#ifdef USERHOOK_MEDIUMLOOP
void Copter::userhook_MediumLoop()
{
    // put your 10Hz code here
}
#endif

#ifdef USERHOOK_SLOWLOOP
void Copter::userhook_SlowLoop()
{
    // put your 3.3Hz code here
    //Printfunc();
}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{
    // put your 1Hz code here
    
}
#endif

char valid(char in){
    if(
        (in == '0') ||
        (in == '1') ||
        (in == '2') ||
        (in == '3') ||
        (in == '4') ||
        (in == '5') ||
        (in == '6') ||
        (in == '7') ||
        (in == '8') ||
        (in == '9')){
        return in;
    }else{
        return '\0';
    }

}

void Printfunc(){
    uint32_t data;
    char buff[5];
    memset(buff,'\0',sizeof(buff));
    c = 0;

    c = hal.uartE->read();
    if ((c == 'f')||(c == 'b')||(c == 'l')||(c == 'r')){ //falure to read!//nothing inbuffer
        buff[0] = valid(hal.uartE->read());
        buff[1] = valid(hal.uartE->read());
        buff[2] = valid(hal.uartE->read());
        buff[3] = valid(hal.uartE->read());
        buff[4] = '\0';

        if(buff[0] != '\0'){
            data = atoi(buff);
        }else{
            data = 9999;
        }
        
        switch(c){
            case 'f':
                forward = data;
                ftime = 0;
                break;
            case 'b':
                back = data;
                btime = 0;
                break;
            case 'l':
                left = data;
                ltime = 0;
                break;
            case 'r':
                right = data;
                rtime = 0;
                break;
            default:

                break;
        }
    }
    ftime++;
    rtime++;
    ltime++;
    btime++;
    if (ftime > 20){
        forward = 9999;
    }
    if (rtime > 20){
        right = 9999;
    }
    if (ltime > 20){
        left = 9999;
    }
    if (btime > 20){
        back = 9999;
    }

    

    //c = hal.uartE->read();

    //c = hal.uartE->read();
    //if (c == (255)){ //falure to read!, use old value
    //    c = temp;
    //}
    


    //c;//send data

    char outarr[100];
    memset(outarr,'\0',sizeof(outarr));
    sprintf(outarr,"Hello! f:%d Roll:%5f Pitch:%5f \n\r",forward,Roll,Pitch);
    hal.uartE->write((uint8_t*)outarr, sizeof(outarr));


    //if (uart != nullptr) {
    //    int16_t nbytes = uart->available();
    //    while (nbytes) {
    //        char c = uart->read();
    //        uart->write(c);
    //    }
    //}
    
    //hal.console->print("hello world! ");
    //if (c != 0){
   //    hal.console->print(c); 
   //}else{
   // hal.console->print("Nothing");
   //}
    

   // hal.console->print("\n");


}