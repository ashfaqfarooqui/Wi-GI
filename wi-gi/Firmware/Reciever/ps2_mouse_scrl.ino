/* 
  Reciever for mouse(Wi-Gi)
  
  A simple device that behaves as a mouse.It uses the PS/2 protocol.
  it behaves as the Intellimouse by Microsoft, supporting movement,
  2 buttons and scroll functionality.
  
  Circuit:
  * pin 2- data and pin 3 - clock connected to the PS/2 port or to the PS/2 to USB converter 
  ic
  * A RF reciever connected to RX pin, or a direct wire from the transmitter.
  
  Modified 12 May 2012
  by Ashfaq h Farooqui 
  E-mail: ashfaq.farooqui@gmail.com
  
  PS/2dev library taken from http://www.arduino.cc/playground/ComponentLib/Ps2mouse
  and modified as per needs.


*/
#include "ps2dev.h"

PS2dev mouse(3,2); // 2 data 3clock

char buttons[3] = {0,0,0};
unsigned char x,y,z,z_s,flag,s,a,sign;
int delta_x = 0;
int delta_y = 0;
int delta_z = 0;
//we start off not enabled
int enabled =0;

//ack a host command
void ack() {
  while(mouse.write(0xFA));
}

void write_packet() {
  char overflowx =0;
  char overflowy =0;
  char data[4];
  int x,y;
  
  if (delta_x > 255) {
    overflowx =1;
    x=255;
  }
  if (delta_x < -255) {
    overflowx = 1;
    x=-255;
  }  
  if (delta_y > 255) {
    overflowy =1;
    y=255;
  }
  if (delta_y < -255) {
    overflowy = 1;
    y=-255;
  }
  
  data[0] = ((overflowy & 1) << 7) |
    ( (overflowx & 1) << 6) |
    ( (((sign &0x02)>>1) & 1) << 5) |             
    ( ( ((sign &0x01))& 1) << 4) |                
    ( ( 1) << 3) |
    ( ( ((0)) & 1) << 2) |
    ( ( ((sign &0x04)>>2) & 1) << 1) |
    ( ( ((sign &0x08)>>3) & 1) << 0) ;
    
    
  data[1] = delta_x & 0xff;
  data[2] = delta_y & 0xff;
  data[3] = delta_z & 0xff;
  mouse.write(data[0]);     //sign bits and button press
  mouse.write(data[1]);     //x byte
  mouse.write(data[2]);     //y byte
  mouse.write(data[3]);    //z byte

  delta_x = 0;
  delta_y = 0;
  delta_z = 0;
}

int mousecommand(int command) {
  unsigned char val;

  //This implements enough mouse commands to get by, most of them are
  //just acked without really doing anything
  Serial.println(command,HEX);
  switch (command) {
  case 0xFF: //reset
    ack();
    //the while loop lets us wait for the host to be ready
    while(mouse.write(0xAA)!=0);  
    while(mouse.write(0x00)!=0);
  
    break;
  case 0xFE: //resend
    ack();
    break;
  case 0xF6: //set defaults 
    //enter stream mode   
    ack();
    break;
  case 0xF5:  //disable data reporting
    //FM
    ack();
    break;
  case 0xF4: //enable data reporting
    //FM
    enabled = 1;
    ack();
    break;
  case 0xF3: //set sample rate
    ack();
    mouse.read(&val); // for now drop the new rate on the floor
    //      Serial.println(val,HEX);
    ack();
    break;
  case 0xF2: //get device id
    ack();
    mouse.write(03);
    break;
  case 0xF0: //set remote mode 
    ack();  
    break;
  case 0xEE: //set wrap mode
    ack();
    break;
  case 0xEC: //reset wrap mode
    ack();
    break;
  case 0xEB: //read data
    ack();
    write_packet();
    break;
  case 0xEA: //set stream mode
    ack();
    break;
  case 0xE9: //status request
    ack();
    //      send_status();
    break;
  case 0xE8: //set resolution
    ack();
    mouse.read(&val);
    //    Serial.println(val,HEX);
    ack();
    break;
  case 0xE7: //set scaling 2:1
    ack();
    break;
  case 0xE6: //set scaling 1:1
    ack();
    break;

  } 
  
}

int xcenter ;
int ycenter;

int xsum = 0;
int ysum = 0;

void setup() {
  unsigned char val;
 Serial.begin(2400);
  // send the mouse start up
  
  while(mouse.write(0xAA)!=0);  
  while(mouse.write(0x00)!=0);


}




void loop() {
  unsigned char  c;
  if( (digitalRead(3)==LOW) || (digitalRead(2) == LOW)) {
    while(mouse.read(&c)) ;
    mousecommand(c);
  } 
  for(int i=0;i<3;i++)
  {
  if((Serial.available()>0))
  {
    a=Serial.read();
    
    if((a&0xc0)==0x80)
    {
      x=a&0x0c;
    }
    else if((a&0xc0)==0x40)
    {
      s=a&0x1f;
    }
    else if((a&0xc0)==0)
    {
      y=a&0x0c;
    }
    else if((a&0xc0)==0xc0)
    {
      z=a&0x07;
    }
  }
  }
  Serial.flush();
    
  z_s=s&0x03;
  switch(z_s)
  {
    case 1:x=-x;
    break;
    case 2:y=-y;
    break;
    case 3:x=-x;y=-y;
    break;
    
  }
  if((s&0x10)==0x10)
  {
    z=-z;
  }
  /*Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.print(z);
    Serial.print('\t');
    Serial.print(s,BIN);
    
    Serial.println();*/
  if (enabled) {
    delta_x = x;
    delta_y = y;
    delta_z=z;
    sign=s;
    
    
    write_packet()  ;
    x=y=z=0;
  }
  

}
