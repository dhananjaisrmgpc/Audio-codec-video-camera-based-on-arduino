/*================================================================================================================================================================================================================================================================
Follow this and datasheet:
https://www.aliexpress.com/item/NEW-RS232-TTL-JPEG-Digital-Serial-Port-CCTV-Camera-Module-SCB-1-with-video-out-Support/1975852463.html?spm=2114.01010208.3.2.iagS5N&ws_ab_test=searchweb0_0,searchweb201602_2_10152_10065_10151_10068_10084_10083_10080_10082_10081_10110_10136_10137_519_10175_10111_10060_10112_10113_10155_10062_10114_437_10154_10056_10055_10054_10182_10059_303_100031_10099_10078_10079_10103_10073_10102_10096_5030019_10123_10189_10052_10053_10142_10107_142_10050_10051,searchweb201603_1,ppcSwitch_3&btsid=47ee5c0e-95cd-478d-9d75-51ad27288959&algo_expid=eb491db4-10b2-41e7-9bc0-f3671ba9e803-0&algo_pvid=eb491db4-10b2-41e7-9bc0-f3671ba9e803

The Command Set for VC0706 with Responses in HEX

COMMAND         Send To Camera                      Response Received from Camera

Reset           56002600                            760026000056433037303320312e30300d0a
Get Version     56001100                            760011000b56433037303320312e3030
Take PIC        5600360100                          7600360000 //STOP Current frame
Image Len       5600340100                          760034000400003394
Image Data      5600320C000A000000000000            5600320c000a0000000000003394000a  CHANGES EVERY TIME
Resume          5600360102 //try 03 also            76003600 //Resume Current frame
Set Photo size  56 00 31 05 04 01 00 19 00     re     where (00 is 640x480)||(11 is 320x240)||(22 is 160x120) expected Response from camera is 76 00 31 00 00

                                     |<----->|
56 00 32 0C 00 0A 00 00 00 00 00 00 | 32 98 | 00 0A

56 00 24 03 01 0D A6


                camm.write((byte)0x56);
                camm.write((byte)0x0);
                camm.write((byte)0x32);
                camm.write((byte)0x0C);
                camm.write((byte)0x0);
                camm.write((byte)0x0A);
                camm.write((byte)0x0);
                camm.write((byte)0x0);
                camm.write((byte)0x0);
                camm.write((byte)0x0);
                camm.write((byte)0x0);
                camm.write((byte)0x0);

=================================================================================================================================================================================================================================================================*/
#include "SoftwareSerial.h"
#include <SPI.h>
#include <SD.h>
File myFile;
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

void Reset(void);
void getVersion(void);
void set_img_size(void);
void take_pic(void);
void pic_length(void);
void pic_data(void);
void resume_pic(void);
void readResponse1(uint8_t numbytes, uint8_t timeout);

uint8_t  received;
unsigned long img_length=0;
uint8_t last_two_byte=0;
uint8_t fbuf_msb=0x00;
uint8_t fbuf_lsb=0x00;
uint8_t second_last=0x00;
uint8_t last=0x00;

unsigned long addrs_val=0x00;
uint8_t addrs_val_h=0x00;
uint8_t addrs_val_l=0x00;
uint8_t addrs_val_lsb=0x00;
uint8_t addrs_val_msb=0x00;
uint8_t end_prgrm=0;
uint8_t rmv_header=0;

uint8_t valid_file=0;
char arr[7];
uint8_t pos=0;
uint8_t led_ucnt, led_dcnt, r1_ucnt, r1_dcnt, r2_ucnt, r2_dcnt=0;
char led_high[]={':','a','t','L','D','H','z'}; //:atLDHz
char led_low[]={':','a','t','L','D','L','z'}; //:atLDLz

SoftwareSerial camm(2, 3); //Camera TX t0 D2 and Camera RX to D3

void Reset(void) //Very Important_dont forget to do this..so that new snaps are taken every time
{
  last_two_byte=0; fbuf_msb=0x00; fbuf_lsb=0x00; second_last=0x00; last=0x00; 
  addrs_val_msb=0x00; addrs_val_h=0x00; addrs_val_lsb=0x00; addrs_val_l=0x00;
  end_prgrm=0; rmv_header=0; //activates on every new frame 
  img_length=0;
  
  Serial.println(" Reset ");
  camm.write((byte)0x56);
  camm.write((byte)0x00);
  camm.write((byte)0x26);
  camm.write((byte)0x00);
  delay(500);
  while(camm.available()>0)
  {
    char ver=camm.read();
    Serial.print(ver);
  }
  Serial.println();
}

void reduce_baud(void)
{
  Serial.println(" BAUD ");
  camm.write((byte)0x56);
  camm.write((byte)0x00);
  camm.write((byte)0x24);
  camm.write((byte)0x03);
  camm.write((byte)0x01);
  camm.write((byte)0x2A); //0x0D..0xA6 for 115200
  camm.write((byte)0xF2);
  delay(1000);
  camm.begin(38400);
  delay(200);
  while(camm.available()>0)
  {
    char ver=camm.read();
    Serial.print(ver);
  }
  Serial.println();
}
void getVersion(void)
{
  Serial.println("Get_V");
  camm.write((byte)0x56);
  camm.write((byte)0x00);
  camm.write((byte)0x11);
  camm.write((byte)0x00);
  delay(500);
  while(camm.available()>0)
  {
    char ver=camm.read();
    Serial.print(ver);
  }
  Serial.println();
}
void set_img_size(void)
{
  //56 00 31 05 04 01 00 19 00          
  camm.write((byte)0x56);
  camm.write((byte)0x00);
  camm.write((byte)0x31);
  camm.write((byte)0x05);
  camm.write((byte)0x04);
  camm.write((byte)0x01);
  camm.write((byte)0x00);
  camm.write((byte)0x19);
  camm.write((byte)0x22); //where (00 is 640x480)||(11 is 320x240)||(22 is 160x120) expected Response from camera is 76 00 31 00 00
  delay(500);
  while(camm.available()>0)
  {
    char ver=camm.read();
    Serial.print(ver);
  }
  Serial.println("38400 now");
}
void take_pic(void)
{
  camm.write((byte)0x56);
  camm.write((byte)0x00);
  camm.write((byte)0x36);
  camm.write((byte)0x01);
  camm.write((byte)0x00);
  delay(500);
  while(camm.available()>0)
  {
    char ver=camm.read();
    Serial.print(ver);
  }
  Serial.println();
}
void pic_length(void)
{
  camm.write((byte)0x56);
  camm.write((byte)0x00);
  camm.write((byte)0x34);
  camm.write((byte)0x01);
  camm.write((byte)0x00);
  delay(500);  
  readResponse1(0, 0);
}
void readResponse1(uint8_t numbytes, uint8_t timeout)
{
  last_two_byte=0; fbuf_msb=0x00; fbuf_lsb=0x00; second_last=0x00; last=0x00; 
  uint8_t counter = 0;
  uint8_t loop1 = 0;
  img_length=0;
  
  while (camm.available() > 0)
  {
    received = camm.read();
    last_two_byte++;
    if(last_two_byte==6)
    {
      fbuf_msb=received;
      Serial.print(fbuf_msb,HEX);Serial.print(" The fbuf_msb is ");Serial.println(fbuf_msb); 
    }
    if(last_two_byte==7)
    {
      fbuf_lsb=received;
      Serial.print(fbuf_lsb,HEX);Serial.print(" The fbuf_lsb is ");Serial.println(fbuf_lsb); 
    }
    if(last_two_byte==8)
    {
      second_last=received;
      Serial.print(second_last,HEX);Serial.print(" The second last is ");Serial.println(second_last); 
      //return(second_last);
    }
    if(last_two_byte==9)
    {
      last=received; 
      Serial.print(last,HEX);Serial.print(" The last is ");Serial.println(last);
      last_two_byte=0;
    }                     
  }
  
  img_length=( ( (fbuf_msb<<32)|(fbuf_lsb<<16)|(second_last<<8) )|(last));
    
  if(fbuf_msb>0x00){img_length=((img_length)&(0x00FFFFFFFF));Serial.println("FM");}
  if((fbuf_lsb>0x00)&&(fbuf_msb==0x00)){img_length=((img_length)&(0x00FFFFFF));Serial.println("FL");}
  if((second_last>0x00)&&(fbuf_msb==0x00)&&(fbuf_lsb==0x00)){img_length=((img_length)&(0x0000FFFF));Serial.println("M");}
  if((last>0x00)&&(fbuf_msb==0x00)&&(fbuf_lsb==0x00)&&(second_last==0x00)){img_length=((img_length)&(0x000000FF));Serial.println("L");}
 
  Serial.print(img_length,HEX);Serial.print(" IMG_LEN ");Serial.println(img_length,DEC);
}
void pic_data(void)
{
  int32_t dat_count=0;  
  uint8_t A = 0x56;
  uint8_t chunk_size=0x20; //Dont change me I am 32 in Decimal
  addrs_val_msb=0x00; addrs_val_h=0x00; addrs_val_lsb=0x00; addrs_val_l=0x00;addrs_val=0x00;
  end_prgrm=0; rmv_header=0; //activates on every new frame 
  dat_count=0;
  
  while(end_prgrm<=1)
  {
    rmv_header=0; //activates on every new frame  
    addrs_val_msb=((((addrs_val)>>32)&(0x000000FF)));
    addrs_val_h=(((addrs_val)>>8) &(0x000000FF));
    addrs_val_lsb=(((addrs_val)>>16)&(0x000000FF));
    addrs_val_l=((addrs_val)&0x000000FF);
    //                       ||             ||       |<----->|
    // {//56 00 32 0C 00 0A || 00 00 00 00 || 00 00 | 32 98 | 00 0A
    //                                               |-------|
    //Serial.print("==="); Serial.print(second_last,HEX);Serial.print("==="); Serial.println(last,HEX);Serial.print("===");  
    camm.write((byte)A);
    camm.write((byte)0x0); //smart way to confirm that it takes 0x00 and 0x0
    camm.write((byte)0x32);
    camm.write((byte)0x0C);
    camm.write((byte)0x0);
    camm.write((byte)0x0A);
    camm.write((byte)addrs_val_msb);//
    camm.write((byte)addrs_val_lsb);//
    camm.write((byte)addrs_val_h);//addr_msb
    camm.write((byte)addrs_val_l);//addr_lsb
    camm.write((byte)0x00);//packet_byte4
    camm.write((byte)0x00);//packet_byte3
    //camm.write((byte)second_last);//packet_byte2  
    //camm.write((byte)last);//packet_byte1//keep this size fixed
    camm.write((byte)0x00);//packet_byte2
    camm.write((byte)chunk_size);//packet_byte1//keep this size fixed   
    camm.write((byte)0x00);
    camm.write((byte)0xFF);
    delay(500);//Serial3.println("Command Given");
    Serial.println("");
    //Serial.print(chunk_size);Serial.print(" Chunk ");Serial.print(addrs_val);Serial.print(" Original_Add ");Serial.print(addrs_val_h);Serial.print(" MSB_LSB ");Serial.println(addrs_val_l);
    while(camm.available()>0)
    {
      char ver=camm.read();
      dat_count++;
      //Serial.print(ver);
      //if((rmv_header>10)&&(rmv_header<43)){myFile.print(ver);}
      myFile.print(ver);     
    }
    addrs_val=addrs_val+chunk_size;
    //if(img_length<0) //since img_length is unsigned long thus it wont be 0 //Check this Logic 
    if(img_length<=chunk_size)
    {
      chunk_size=(img_length+32);//Dont use chunk_size, it is in HEX//32
      end_prgrm=5;
      Serial.print(" HERE ");Serial.print(end_prgrm);Serial.println(" STOP ");
    }
    if(end_prgrm<=1){img_length=img_length-chunk_size;}
    //Serial.print(end_prgrm);Serial.print(" Len_left ");
    Serial.println(img_length);//Serial.print(" Chunk ");Serial.println(chunk_size);
  } //Phot has been saved now...Pulse the camera
  
  delay(500);
  digitalWrite(A0,HIGH);//Turn OFF supply for Camera via PMOS by keeping A0 HIGH
  myFile.close();
  Serial.print("::::::ENDDDD");
  camm.end();
  delay(500);
  myFile = SD.open("nmg.txt", FILE_READ);
  delay(200);
  while (myFile.available()) 
  {
    Serial.write(myFile.read());
  } 
  delay(100);
  myFile.close();
  Serial.print(dat_count);Serial.println(" Count ");
  SD.remove("nmg.txt");
  delay(100);
}
void resume_pic(void)
{
  camm.write((byte)0x56);
  camm.write((byte)0x00);
  camm.write((byte)0x36);
  camm.write((byte)0x01);
  camm.write((byte)0x03);
  delay(10);
  while(camm.available()>0)
  {
    char ver=camm.read();
    Serial.print(ver);
  }
  Serial.println();  
}


void setup()
{
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);//Turn ON supply for Camera via PMOS by keeping A0 LOW
  camm.begin(115200);
  Serial.begin(38400);
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) 
  {
    Serial.println("initialization failed!");
    SD.remove("nmg.txt");
    return;
  }
  Serial.println("initialization done.");
  SD.remove("nmg.txt");
}

void loop()
{
  while(Serial.available()>0)
  {
    char code=Serial.read();   
    Serial.println(code);
    if(code==':'){pos=0;} //when it detects : then its the start of the code
    arr[pos]=code;
    pos++;
    if(pos==7)
    {
      led_ucnt=0;led_dcnt=0;
      for(uint8_t p=0;p<=6;p++)
      { 
        if(arr[p]==led_high[p]){led_ucnt++;}//:atLDHz
        if(arr[p]==led_low[p]){led_dcnt++;}//:atLDLz
      }
      if((led_ucnt==7))
      {
        myFile = SD.open("nmg.txt", FILE_WRITE);
        delay(500);
        if (myFile) 
        {
          Serial.print("Writing to test.txt...");
        }
        else 
        {
          Serial.println("error opening test.txt");
        }
        
        delay(500);
        Serial.println("Start_operations");
        {resume_pic();}delay(1000); //7 Not needed I guess, Last number should be 03 not (02)
        {Reset();}delay(1000);//
        reduce_baud();
        //{getVersion();}delay(1000); //2
        {set_img_size();}delay(1000); //3
        {take_pic();}delay(1000); //4
        {pic_length();}delay(1000); //5
        {pic_data();}delay(1000); //6
        {resume_pic();}delay(1000); //7 Not needed I guess, Last number should be 03 not (02)
        {Reset();}delay(1000);//reset again not really needed
        camm.begin(115200); //after Reset baud goes back to default
        delay(500);
        Serial.println("::::::PGMEND");
        delay(100);
      }
      for(uint8_t n=0;n<=6;n++){arr[n]=0;}//empty the input array
      pos=0;
    }
  }
}  


