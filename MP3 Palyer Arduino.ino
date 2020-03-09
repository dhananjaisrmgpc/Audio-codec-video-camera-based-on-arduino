
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
SoftwareSerial mySerial(6,7); // RX, TX
char c;

void setup () {
	Serial.begin (9600);
	mySerial.begin (9600);
	mp3_set_serial (mySerial);	//set softwareSerial for DFPlayer-mini mp3 module 
	delay(1);  //wait 1ms for mp3 module to set volume
	mp3_set_volume (22);
  mp3_stop ();
  Serial.println("K8");
}
void loop ()
{
  while(Serial.available()>0)
  {
    c=Serial.read();
    Serial.println(c);
  }
  if(c=='P')
  { 
    Serial.println("St");       
    mp3_stop ();delay(30);//
    mp3_play (1);
    digitalWrite(6,LOW);
    digitalWrite(7,LOW);
    delay (10000);
    mp3_stop ();delay(30);
    Serial.println("Spp");
  }
}

/*
   mp3_play ();		//start play
   mp3_play (5);	//play "mp3/0005.mp3"
   mp3_next ();		//play next 
   mp3_prev ();		//play previous
   mp3_set_volume (uint16_t volume);	//0~30
   mp3_set_EQ ();	//0~5
   mp3_pause ();
   mp3_stop ();
   void mp3_get_state (); 	//send get state command
   void mp3_get_volume (); 
   void mp3_get_u_sum (); 
   void mp3_get_tf_sum (); 
   void mp3_get_flash_sum (); 
   void mp3_get_tf_current (); 
   void mp3_get_u_current (); 
   void mp3_get_flash_current (); 
   void mp3_single_loop (boolean state);	//set single loop 
   void mp3_DAC (boolean state); 
   void mp3_random_play (); 
 */

