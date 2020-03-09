/*
{ 0xFD,  | 0x09,| 0x01,| 0x04,       | 0x00,| 0x09, | 0x00, |0x00, | 0x00,| 0x00,| 0x00 | };
{  translation mode    |modifier_mask|  NA  | ASCII | NA    |NA    |NA    | NA   |NA    | }                        
          
modifier_mask:                                             | combo    | mask Hex value
  7   |   6   |   5   |   4   |  3   |   2  |   1  |   0   | left alt | 04
------------------------------------------------------------ left gui | 08
Right | Right | Right | Right | Left | Left | Left | Left  | left shift+left alt | 06     
GUI   |  Alt  | Shift | Cntrl | GUI  |  Alt | Shift| Cntrl | nothing/blank | 00

ASCII Hex Value:
data rcvd--> | a | b  | c | d  | e | f  | g  |  h  | i  | j  | k  | l  | m  | n  | o  | p  | q  | r  | s | t  | u | v  | u | x  | y | z |
hex by uC--> | 04| 05 | 06| 07 | 08| 09 | 0A |  0B | 0C | 0D | 0E | 0F | 10 | 11 | 12 | 13 | 14 | 15 | 16| 17 | 18| 19 | 1A| 1B | 1C| 1D|

data rcvd--> | 1 |  2 |  3| 4 | 5 | 6 | 7 | 8 | 9 | 0 | Enter |Escape| Backspace | TAB | Space | -  | = | [  |                                                        
hex by uC--> | 1E| 1F | 20| 21| 22| 23| 24| 25| 26| 27|   28  |  29  |    2A     | 2B  |  2C   | 2D | 2E| 2F |                                                     

data rcvd--> |  ] |  \ |  \ |  ; |  '  |  ` |  , |  . | / | Delete | Right arrow | Left arrow | Down arrow | UP arrow |
hex by uC--> | 30 | 31 | 32 | 33 | 34  | 35 | 36 | 37 |38 |   4C   |     4F      |    50      |     51     |     52   |

Null
hex by uC--> 2A,39,
*/

#include <Keypad.h>
#include <SoftwareSerial.h>
//SoftwareSerial hid_Serial(10,11); // RX, TX
SoftwareSerial hid_Serial(A2,A3); // RX, TX

#define batt_pin A0

char vari=0x39;
uint8_t rpt_count=5;
uint8_t test_val[11]={0xFD,0x09,0x01,0x00,0x00,0x4C,0x00,0x00,0x00,0x00,0x00};
uint8_t right_arw_key[11]={0xFD,0x09,0x01,0x00,0x00,0x4F,0x00,0x00,0x00,0x00,0x00};//right arrow
uint8_t left_arw_key[11]={0xFD,0x09,0x01,0x00,0x00,0x50,0x00,0x00,0x00,0x00,0x00};//left arrow
uint8_t down_arw_key[11]={0xFD,0x09,0x01,0x00,0x00,0x51,0x00,0x00,0x00,0x00,0x00};//down arrow
uint8_t up_arw_key[11]={0xFD,0x09,0x01,0x00,0x00,0x52,0x00,0x00,0x00,0x00,0x00};//up arrow
uint8_t enter_app_key[11]={0xFD,0x09,0x01,0x00,0x00,0x28,0x00,0x00,0x00,0x00,0x00};//enter_in_the_app_key
uint8_t tab_key[11]={0xFD,0x09,0x01,0x00,0x00,0x2B,0x00,0x00,0x00,0x00,0x00};//TAB
uint8_t main_menu[11]={0xFD,0x09,0x01,0x01,0x00,0x29,0x00,0x00,0x00,0x00,0x00}; //LefT_Ctrl+Esc //main_menu without additional app
uint8_t back_key[11]={0xFD,0x09,0x01,0x00,0x00,0x29,0x00,0x00,0x00,0x00,0x00}; //back//escape

uint8_t contacts[11]={0xFD,0x09,0x01,0x08,0x00,0x06,0x00,0x00,0x00,0x00,0x00}; //windows+c //without additional app
const uint8_t app_contacts[11]={0xFD,0x09,0x01,0x04,0x00,0x06,0x00,0x00,0x00,0x00,0x00}; //Left_Alt(A0)+c
const uint8_t app_msgs[11]={0xFD,0x09,0x01,0x04,0x00,0x10,0x00,0x00,0x00,0x00,0x00}; //Left_Alt(A1)+m
const uint8_t app_phone[11]={0xFD,0x09,0x01,0x04,0x00,0x15,0x00,0x00,0x00,0x00,0x00}; //Left_Alt(A2)+r
const uint8_t app_spkng_clck[11]={0xFD,0x09,0x01,0x04,0x00,0x14,0x00,0x00,0x00,0x00,0x00}; //Left_Alt(A3)+t// Now alt + q
const uint8_t app_radio[11]={0xFD,0x09,0x01,0x04,0x00,0x16,0x00,0x00,0x00,0x00,0x00}; //Left_Alt(A4)+f // Now alt + s
const uint8_t release_key[11]={0xFD,0x09,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //Key Release 

const uint8_t numbr_one[11]={0xFD,0x09,0x01,0x00,0x00,0x1E,0x00,0x00,0x00,0x00,0x00}; //Numeric_1
const uint8_t numbr_two[11]={0xFD,0x09,0x01,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00}; //Numeric_2
const uint8_t numbr_three[11]={0xFD,0x09,0x01,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00}; //Numeric_3
const uint8_t numbr_four[11]={0xFD,0x09,0x01,0x00,0x00,0x21,0x00,0x00,0x00,0x00,0x00}; //Numeric_4
const uint8_t numbr_five[11]={0xFD,0x09,0x01,0x00,0x00,0x22,0x00,0x00,0x00,0x00,0x00}; //Numeric_5
const uint8_t numbr_six[11]={0xFD,0x09,0x01,0x00,0x00,0x23,0x00,0x00,0x00,0x00,0x00}; //Numeric_6
const uint8_t numbr_seven[11]={0xFD,0x09,0x01,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00}; //Numeric_7
const uint8_t numbr_eight[11]={0xFD,0x09,0x01,0x00,0x00,0x25,0x00,0x00,0x00,0x00,0x00}; //Numeric_8
const uint8_t numbr_nine[11]={0xFD,0x09,0x01,0x00,0x00,0x26,0x00,0x00,0x00,0x00,0x00}; //Numeric_9
const uint8_t numbr_zero[11]={0xFD,0x09,0x01,0x00,0x00,0x27,0x00,0x00,0x00,0x00,0x00}; //Numeric_0 

const uint8_t alpha_a[11]={0xFD,0x09,0x01,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00}; //Alphabet_a
const uint8_t alpha_b[11]={0xFD,0x09,0x01,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00}; //Alphabet_b
const uint8_t alpha_c[11]={0xFD,0x09,0x01,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00}; //Alphabet_c
const uint8_t alpha_d[11]={0xFD,0x09,0x01,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00}; //Alphabet_d
const uint8_t alpha_e[11]={0xFD,0x09,0x01,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00}; //Alphabet_e
const uint8_t alpha_f[11]={0xFD,0x09,0x01,0x00,0x00,0x09,0x00,0x00,0x00,0x00,0x00}; //Alphabet_f
const uint8_t alpha_g[11]={0xFD,0x09,0x01,0x00,0x00,0x0A,0x00,0x00,0x00,0x00,0x00}; //Alphabet_g
const uint8_t alpha_h[11]={0xFD,0x09,0x01,0x00,0x00,0x0B,0x00,0x00,0x00,0x00,0x00}; //Alphabet_h
const uint8_t alpha_i[11]={0xFD,0x09,0x01,0x00,0x00,0x0C,0x00,0x00,0x00,0x00,0x00}; //Alphabet_i
const uint8_t alpha_j[11]={0xFD,0x09,0x01,0x00,0x00,0x0D,0x00,0x00,0x00,0x00,0x00}; //Alphabet_j
const uint8_t alpha_k[11]={0xFD,0x09,0x01,0x00,0x00,0x0E,0x00,0x00,0x00,0x00,0x00}; //Alphabet_k
const uint8_t alpha_l[11]={0xFD,0x09,0x01,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00}; //Alphabet_l
const uint8_t alpha_m[11]={0xFD,0x09,0x01,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00}; //Alphabet_m
const uint8_t alpha_n[11]={0xFD,0x09,0x01,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00}; //Alphabet_n
const uint8_t alpha_o[11]={0xFD,0x09,0x01,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00}; //Alphabet_o
const uint8_t alpha_p[11]={0xFD,0x09,0x01,0x00,0x00,0x13,0x00,0x00,0x00,0x00,0x00}; //Alphabet_p
const uint8_t alpha_q[11]={0xFD,0x09,0x01,0x00,0x00,0x14,0x00,0x00,0x00,0x00,0x00}; //Alphabet_q
const uint8_t alpha_r[11]={0xFD,0x09,0x01,0x00,0x00,0x15,0x00,0x00,0x00,0x00,0x00}; //Alphabet_r
const uint8_t alpha_s[11]={0xFD,0x09,0x01,0x00,0x00,0x16,0x00,0x00,0x00,0x00,0x00}; //Alphabet_s
const uint8_t alpha_t[11]={0xFD,0x09,0x01,0x00,0x00,0x17,0x00,0x00,0x00,0x00,0x00}; //Alphabet_t
const uint8_t alpha_u[11]={0xFD,0x09,0x01,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00}; //Alphabet_u
const uint8_t alpha_v[11]={0xFD,0x09,0x01,0x00,0x00,0x19,0x00,0x00,0x00,0x00,0x00}; //Alphabet_v
const uint8_t alpha_w[11]={0xFD,0x09,0x01,0x00,0x00,0x1A,0x00,0x00,0x00,0x00,0x00}; //Alphabet_w
const uint8_t alpha_x[11]={0xFD,0x09,0x01,0x00,0x00,0x1B,0x00,0x00,0x00,0x00,0x00}; //Alphabet_x
const uint8_t alpha_y[11]={0xFD,0x09,0x01,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00}; //Alphabet_y
const uint8_t alpha_z[11]={0xFD,0x09,0x01,0x00,0x00,0x1D,0x00,0x00,0x00,0x00,0x00}; //Alphabet_z
const uint8_t back_space[11]={0xFD,0x09,0x01,0x00,0x00,0x2A,0x00,0x00,0x00,0x00,0x00}; //back_space on 9 key
const uint8_t space_bar[11]={0xFD,0x09,0x01,0x00,0x00,0x2C,0x00,0x00,0x00,0x00,0x00}; //space_key on 0 key

char prv_customKey=0;
uint8_t oprtn_state=0;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
unsigned long nxt_swtch_time=0;
uint8_t switch_state=0;
int batt_val=0;
String volts="";

char hexaKeys[ROWS][COLS] = 
{
  {'D','#','0','*'},
  {'C','9','8','7'},
  {'B','6','5','4'},
  {'A','3','2','1'}
};
//byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {2, 3, 4, 5}; //connect to the column pinouts of the keypad
byte rowPins[ROWS] = {9, 10, 11, 12}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4, 8}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup()
{
  hid_Serial.begin(115200);
  Serial.begin(115200);
  Serial.println("KFX");
}

void loop() 
{
  /*batt_val=analogRead(batt_pin);
  Serial.print(batt_val);Serial.print(" Bat_MAP ");
  batt_val=map(batt_val, 0, 1023, 0, 255); 
  Serial.println(batt_val);
  if(batt_val>=15)
  {
   volts="3.8V"; //kid tracker mode
   Serial.print(" BAT ");Serial.println(batt_val);
  }
  else if(batt_val<15){Serial.print(" Chng_Bat ");Serial.println(batt_val);volts="Low!";}*/
  
  char customKey = customKeypad.getKey();
  if(customKey)
  {
    nxt_swtch_time=millis();
       
    if(prv_customKey=='*')//
    {
      //customKey = customKeypad.getKey();
      Serial.print(customKey);Serial.println(" HEY ");
      if(customKey)
      {
        if((customKey=='1')&&(prv_customKey=='*'))
        {
          for(uint8_t index=0;index<=10;index++)
          {
            hid_Serial.write(app_contacts[index]);delay(3);
          }
          for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
          Serial.print(" OP_1 ");Serial.println(customKey);  
        }
        if((customKey=='2')&&(prv_customKey=='*'))
        {
          for(uint8_t index=0;index<=10;index++)
          {
            hid_Serial.write(app_msgs[index]);delay(3);
          }
          for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
          Serial.print(" OP_2 ");Serial.println(customKey);  
        }
        if((customKey=='3')&&(prv_customKey=='*'))
        {
          for(uint8_t index=0;index<=10;index++)
          {
            hid_Serial.write(app_phone[index]);delay(3);
          }
          for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
          Serial.print(" OP_3 ");Serial.println(customKey);  
        }
        if((customKey=='4')&&(prv_customKey=='*'))
        {
          for(uint8_t index=0;index<=10;index++)
          {
            hid_Serial.write(app_spkng_clck[index]);delay(3);
          }
          for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
          Serial.print(" OP_4 ");Serial.println(customKey);  
        }
        if((customKey=='5')&&(prv_customKey=='*'))
        {
          for(uint8_t index=0;index<=10;index++)
          {
            hid_Serial.write(app_radio[index]);delay(3);
          }
          for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
          Serial.print(" OP_5 ");Serial.println(customKey);  
        }
        if((customKey=='*')&&(prv_customKey=='*'))
        {
          for(uint8_t index=0;index<=10;index++)
          {
            hid_Serial.write(back_key[index]);delay(3);
          }
          for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
          Serial.println(customKey);
        }
      }    
    }
    if((customKey=='A')&&(prv_customKey!='*'))//left_arw_key
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(left_arw_key[index]);//delay(3);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
      Serial.print("L_arw " );Serial.println(customKey);
    }
    if((customKey=='C')&&(prv_customKey!='*'))//right_arw_key
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(right_arw_key[index]);//delay(3);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
      Serial.print("R_arw " );Serial.println(customKey);
    }
    if((customKey=='B')&&(prv_customKey!='*'))//up_arw_key
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(up_arw_key[index]);//delay(3);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
      Serial.print("UP_arw " );Serial.println(customKey);
    }
    if((customKey=='D')&&(prv_customKey!='*'))//down_arw_key
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(down_arw_key[index]);//delay(3);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
      Serial.print("Dn_arw " );Serial.println(customKey);
    }

    if((customKey=='#')&&(prv_customKey!='*'))//enter_the_app+
    {
      for(uint8_t index=0;index<=10;index++)
     {
        hid_Serial.write(enter_app_key[index]);//delay(3);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(20);
      Serial.println(customKey);
    }

///////////////////////////////////////////////////////////////Numeric_KeyPad with Alphabets//////////////////////////////////////////////////////////////////////////////////////////////

    if((customKey=='1')&&(prv_customKey!='*'))//numeric_1
    {switch_state++;}
    
    if((customKey=='2')&&(prv_customKey!='*'))//numeric_2
    {switch_state++;}
    
    if((customKey=='3')&&(prv_customKey!='*'))//numeric_3
    {switch_state++;}
    
    if((customKey=='4')&&(prv_customKey!='*'))//numeric_four
    {switch_state++;}
    
    if((customKey=='5')&&(prv_customKey!='*'))//numeric_5
    {switch_state++;}
    
    if((customKey=='6')&&(prv_customKey!='*'))//numeric_6
    {switch_state++;}
    
    if((customKey=='7')&&(prv_customKey!='*'))//numeric_7
    {switch_state++;}
    
    if((customKey=='8')&&(prv_customKey!='*'))//numeric_8
    {switch_state++;}
    
    if((customKey=='9')&&(prv_customKey!='*'))//numeric_9
    {switch_state++;}
    
    if((customKey=='0')&&(prv_customKey!='*'))//numeric_9
    {switch_state++;}
    
    if(switch_state>5){switch_state=1;}
    //Serial.print(" SW_State ");Serial.println(switch_state);
    prv_customKey=customKey;  
  }

  
  if(((millis()-nxt_swtch_time)>1000))
  {
////////////////////////////////////////////////// combination at Numeric 1 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
   if(prv_customKey=='1')
   {
    if(switch_state==1)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(numbr_one[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("1"); 
    }
    if(switch_state==2)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_a[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("a"); 
    }
    if(switch_state==3)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_b[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("b");
    }
    if(switch_state==4)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_c[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("c");
    }
    if(switch_state==5)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_d[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("d");
    }
   }

////////////////////////////////////////////////// combination at Numeric 2 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(prv_customKey=='2')
   {
    if(switch_state==1)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(numbr_two[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("2"); 
    }
    if(switch_state==2)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_e[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("e"); 
    }
    if(switch_state==3)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_f[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("f");
    }
    if(switch_state==4)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_g[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("g");
    }
    if(switch_state==5)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_h[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("h");
    }
   } 

////////////////////////////////////////////////// combination at Numeric 3 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(prv_customKey=='3')
   {
    if(switch_state==1)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(numbr_three[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("3"); 
    }
    if(switch_state==2)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_i[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("i"); 
    }
    if(switch_state==3)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_j[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("j");
    }
    if(switch_state==4)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_k[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("k");
    }
    if(switch_state==5)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_l[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("l");
    }
   }
   
////////////////////////////////////////////////// combination at Numeric 4 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(prv_customKey=='4')
   {
    if(switch_state==1)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(numbr_four[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("4"); 
    }
    if(switch_state==2)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_m[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("m"); 
    }
    if(switch_state==3)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_n[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("n");
    }
    if(switch_state==4)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_o[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("o");
    }
    if(switch_state==5)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_p[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("p");
    }
   }

////////////////////////////////////////////////// combination at Numeric 5 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(prv_customKey=='5')
   {
    if(switch_state==1)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(numbr_five[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("5"); 
    }
    if(switch_state==2)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_q[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("q"); 
    }
    if(switch_state==3)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_r[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("r");
    }
    if(switch_state==4)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_s[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("s");
    }
    if(switch_state==5)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_t[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("t");
    }
   }      

////////////////////////////////////////////////// combination at Numeric 6 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(prv_customKey=='6')
   {
    if(switch_state==1)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(numbr_six[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("6"); 
    }
    if(switch_state==2)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_u[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("u"); 
    }
    if(switch_state==3)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_v[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("v");
    }
    if(switch_state==4)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_w[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("w");
    }
    if(switch_state==5)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_x[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("x");
    }
   }

////////////////////////////////////////////////// combination at Numeric 7 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(prv_customKey=='7')
   {
    if(switch_state==1)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(numbr_seven[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("7"); 
    }
    if(switch_state==2)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_y[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("y"); 
    }
    if(switch_state==3)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(alpha_z[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("z");
    }
   }

////////////////////////////////////////////////// combination at Numeric 8 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(prv_customKey=='8')
   {
    if(switch_state==1)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(numbr_eight[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("8"); 
    }
   }

////////////////////////////////////////////////// combination at Numeric 9 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(prv_customKey=='9')
   {
    if(switch_state==1)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(numbr_nine[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("9"); 
    }
    if(switch_state==2)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(back_space[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("back_space"); 
    }    
   }   

////////////////////////////////////////////////// combination at Numeric 0 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(prv_customKey=='0')
   {
    if(switch_state==1)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(numbr_zero[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("0"); 
    }
    if(switch_state==2)
    {
      for(uint8_t index=0;index<=10;index++)
      {
        hid_Serial.write(space_bar[index]);
      }
      for(uint8_t index=0;index<=10;index++){hid_Serial.write(release_key[index]);}delay(5);
      Serial.print(switch_state);Serial.print(" Sw ");Serial.println("space"); 
    }    
   }   
   switch_state=0;
   nxt_swtch_time=millis();
  }
 }
