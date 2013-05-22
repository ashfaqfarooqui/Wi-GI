/*
*This code was originally written by Ashfaq Farooqui(ashfaq.farooqui@gmail.com) and is licenced under GPLv3.
*in case of modifications and improvements add your Name and EmailID bellow.
*
*	Author			EmailId						Modification
*	Ashfaq			ashfaq.farooqui@gmail.com			Original Code
*
*
*
*
*
*
*
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
unsigned char x=0,y=0,z=0;  //ADC variables
signed char s_x,s_y,x_n,z_z,s_z;

/* s_x = final X BYTE.
s_y = final Y byte
z_z = final z byte
s_z = intermediate Z value on moving pot
x_n = switch states and sign bits
*/
void send_ser(char);


int main(void)
{
    char r_x=0,r_y=0,r_z=0;
    DDRD=0;
    //configure timer


    //Serial Starts
    UCSRB= (1<<TXEN);
    UCSRC=0x01;//enable USART, 1 start 2 stop bit,2400 bauds
    UCSRC= (1<<UCSZ1)|(1<<UCSZ0)|(1<<URSEL)|(1<<USBS);
    UBRRL=0xa0;
    //Serial ends

    //ADC starts_here
    ADMUX=0x60;
    ADCSRA|=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADIE);
    ADMUX|=(1<<ADLAR);
    ADCSRA|=(1<<ADSC);
    //Adc ends

    //takeing reference starts
    while((ADCSRA&(1<<ADIF))==0) ;
    r_x=ADCH; //reference
    ADMUX|=(1<<MUX0);
    ADCSRA|=(1<<ADSC);
    while((ADCSRA&(1<<ADIF))==0) ;
    r_y=ADCH; //reference
    ADMUX++;
    ADCSRA|=(1<<ADSC);
    while((ADCSRA&(1<<ADIF))==0) ;
    r_z=ADCH; //reference
    ADMUX=0x60;

    sei();

    //Taking reference ends

    ADCSRA|=(1<<ADSC); //start ADC

    while(1)
    {



        s_x=(x-(r_x));
        s_y=(y-(r_y));
        s_z=((r_z)-z);
        //set sign bits
        if(s_x<0)
        {
            s_x=-s_x;
            x_n=1;
        }

        if(s_y<-2)
        {
            s_y=-s_y;
            x_n|=(1<<PIN1);
        }
        //scroll
        if(s_z>2)
        {
            z_z=0x05;
            x_n|=(1<<PIN4);
        }
        else if(s_z<-2)
        {
            z_z=0x05;
        }
        else
        {
            z_z=0;
        }
        //end scroll

        //make reference on button press.
        if((PIND&0x80)==0)
        {
            cli();
            r_x=x;
            r_y=y;
            r_z=z;
            sei();
        }

        //take switch states
        if((PIND&0x08)==0)         //sw1
        {
            x_n|=(1<<PIN3);

        }

        if((PIND&0x04)==0)           //sw2
        {
            x_n|=(1<<PIN2);
        }

        //packet data in terms of 2 MSB bits for 4 combinations. when subtracting reference with present values assumed that MSB are clear.
       s_x=s_x&0x3f;
        x_n=x_n&0x3f;
       z_z=z_z&0x3f;
        s_x|=(1<<PIN7);
        x_n|=(1<<PIN6);
        z_z|=(1<<PIN7)|(1<<PIN6);



        send_ser(s_x);
        send_ser(s_y);
        send_ser(x_n);
        send_ser(z_z);
        x_n=s_x=s_y=0;


    }


    return 0;
}
ISR(ADC_vect)
{

    switch(ADMUX)
    {
        case 0x60:x=ADCH;
        break;
        case 0x61:y=ADCH;
        break;
        case 0x62:z=ADCH;
        break;
    }
    ADMUX++;
    if(ADMUX>0x62)
    {
        ADMUX=0x60;
    }
    ADCSRA|=(1<<ADSC);

}

void send_ser(char data)
{
    while(!(UCSRA&(1<<UDRE)));
    UDR=data;
}
