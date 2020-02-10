#include <EEPROM.h>
#include <ss_oled.h>
#include <SoftwareSerial.h>
struct {
   byte Unknowndata0 = 2;
   byte Unknowndata1 = 15;
    byte PhaseCurrent = 106;//0 … 255 V = I * 0.53
    byte BatteryCurrent = 126;//0 … 255 V = I * 1.70
    byte LVC = 116;         //0 … 255 V = U * 3.285
    byte Tolerance = 4;    //0 … 255 V = U * 3.285
   byte SpeedLimit = 92;//1 … 96 V = SL * 0.96
   byte SpeedMode = 0; //0: Switch 3spd X1 X2; 1: Cycle 3spd X1; 2: High Switch 3spd X1; 3: Cycle 4spd X1
    byte Speed1 = 104; //0 … 104 V = SL * 0.8
    byte Speed2 = 104; //0 … 104 V = SL * 0.8
    byte Speed3 = 104; //0 … 104 V = SL * 0.8
   byte BlockTime = 5; //10 … 35 V = T * 10
   byte AutoCruiseTime = 150; //V = T * 10
   byte SlipChargeMode = 1; //0: Enable; 1: Disable
   byte IndicateMode = 1;  //0: Common VCC;1: Common GND
    byte RegenForce = 60; //0..200 V = L
   byte ReverseSpeed = 0; //0 … 128 V = SL * 1.28
   byte RegenVoltageLimit = 177; // 0 … 255 V = U * 3.285
   byte GuardLevel = 0; //0: Low; 1: High
   byte DriveSaving = 1; //0: Disabled; 1: Enabled
   byte IpIChoice = 21; //0 .. 21
   byte PASStartPulse = 1; //1 .. 15
   byte DefaultSpeed = 1; //0 … 3 0 - Speed1 … 3 - Speed4
   byte Speed4 = 24; //24 … 95 V = SL * 0.8
   byte ControllerType = 5; //1..6 KH618 5
   byte LimitCruise = 0; //0: No; 1: Yes
   byte IpIMaxSpeed = 0; //0 … 128 V = SL * 1.28
   byte FluxWeaken = 27; //0 .. 191
   byte FluxFineTune = 0; //0 .. 255
   byte WeakPosition = 0; //0 .. 40
   byte WeakTurnPoint = 0; //0 .. 255
    byte Flags = 0b11000000; //192  //Bit-Description;
                                    //7-If 1, motor Hall sensors at 120°, otherwise 60°
                                    //6-Enable regenerative braking if 1
                                    //5-Enable soft start if 1
                                    //4-Disable SoftLVC if 1
   byte unused = 0;
   byte SoftLVCSpeed1 = 80;//0 … 104 V = SL * 0.8
   byte SoftLVCSpeed2 = 80;//0 … 104 V = SL * 0.8
   byte SoftLVCSpeed3 = 80;//0 … 104 V = SL * 0.8
   byte SoftLVCSpeed4 = 80;//0 … 104 V = SL * 0.8
    byte s1Current = 64; //0 .. 141 V = C * 1.28
    byte s2Current = 102; //0 .. 141 V = C * 1.28
    byte s3Current = 128; //0 .. 141 V = C * 1.28
   byte s4Current = 0; //0 .. 141 V = C * 1.28
   byte BattCurLimitLVC = 132; //0 .. 255 V = I * 13.18
   byte SoftStartTime = 0; //0 .. 10 in seconds
   byte SlowSpeed = 0; //0 .. 80 V = S * 0.8
   byte RecoverSpeed  = 0; //0 .. 128 V = S * 1.28
   byte CurrentCompensation = 85; //0 .. 100 V = C
   byte unused0 = 0;
   byte unused1 = 0;
   byte unused2 = 0;
   byte unused3 = 0;
   byte unused4 = 0;
   byte unused5 = 0;
   byte unused6 = 0;
   byte unused7 = 0;
   byte unused8 = 0;
   byte unused9 = 0;
   byte unused10 = 0;
   byte unused11 = 0;
   byte unused12 = 0;
   byte unused13 = 0;
   byte unused14 = 0;
   byte unused15 = 0; 
   byte unused16 = 0;
   byte XOR = 230; //230  //63bit
} dat;

#define BTN_INT 3
#define BTN_PLUS 6
#define BTN_MINUS 5
#define BTN_MPLS 8
#define BTN_MMNS 7

int menu = 13;
byte param = 0;
byte eep_page = 1;
bool plus,minus,ok = 0;
SoftwareSerial mySerial(11, 12);



byte xor_crc_summ(){
  byte xor_crc;
  byte dat_ar[sizeof(dat)];
  memcpy(dat_ar, &dat, sizeof(dat));
  xor_crc = 0;
  for (int i=0; i<=(sizeof(dat)-2); i++){xor_crc=xor_crc ^ dat_ar[i];}
  return xor_crc;
}

void dslay_print(char line1[],String line2,int l1=25,int l2=50){
   char line2_1[25];
   line2.toCharArray(line2_1,sizeof(line2_1));
   sei();
   oledFill(0, 1);
   oledWriteString(0, l1, 0,line1, FONT_SMALL, 0, 1);
   oledWriteString(0, l2, 2,line2_1, FONT_SMALL, 0, 1);
   
}
void eep_write(byte page){
  byte dat_ar[sizeof(dat)];
  dat.XOR=xor_crc_summ();
  memcpy(dat_ar, &dat, sizeof(dat));
  for (int i=0; i<sizeof(dat); i++){
    EEPROM.write((i+(sizeof(dat)*page)), dat_ar[i]);
  }
      dslay_print(" ","WRITE DONE");
      delay(1000);
      init_menu();
}

void eep_read(byte page){
  byte dat_ar[sizeof(dat)];
  for (int i=0; i<sizeof(dat); i++){
    dat_ar[i]=EEPROM.read((i+(sizeof(dat)*page)));
    memcpy(&dat, dat_ar, sizeof(dat));
  }
  
  if (xor_crc_summ()==dat.XOR){
      dslay_print(" ","READ DONE");
      delay(1000);
      init_menu();
   }
   else {
      dslay_print(" ","READ ERROR");
      delay(1000);
      init_menu(); 
   }
}

void programm(){
  dat.XOR=xor_crc_summ();
  
  byte nul[sizeof(dat)];
  Serial.readBytes(nul,sizeof(dat));
  
  Serial.write('8');
  delay(10);
  if ( Serial.read()=='U'){ Serial.write((uint8_t *) &dat, sizeof(dat)); Serial.flush(); }
  delay(10);
  if (Serial.available()==2 && Serial.readString()=="QR"){
      dslay_print(" ","WRITE DONE");
      delay(1000);
      init_menu();
      }
      else {
      dslay_print(" ","WRITE ERROR");
      delay(1000);
      init_menu(); 
      }
}

void reading(){
    byte dat_ar[sizeof(dat)];
    
    byte nul[sizeof(dat)];
    Serial.readBytes(nul,sizeof(dat));
    
    Serial.write('U');
    delay(10);
    Serial.readBytes(dat_ar,sizeof(dat));//}
    delay(10);
    //mySerial.write(dat_ar, sizeof(dat));
    memcpy(&dat, dat_ar, sizeof(dat));
    if (xor_crc_summ()==dat.XOR) {
      dslay_print(" ","READ DONE");
      delay(1000);
      init_menu();
      }
      else {
      dslay_print(" ","READ ERROR");
      delay(1000);
      init_menu(); 
      }
}


void init_menu(){
  bool err=1;
  switch(menu){
    case 1:
      param = dat.PhaseCurrent; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,255);
      dat.PhaseCurrent = param;
      dslay_print("1.PhaseCurrent:  ",String(param / 0.53));
    break; 
    case 2:
      param = dat.BatteryCurrent; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,255);
      dat.BatteryCurrent = param;
      dslay_print("2.BatteryCurrent:",String(param / 1.70));
    break; 
    case 3:
      param = dat.Speed1; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,104);
      dat.Speed1 = param;
      dslay_print("3.Speed1:        ",String(param / 0.8));
    break;
    case 4:
      param = dat.Speed2; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,141);
      dat.Speed2 = param;
      dslay_print("4.S1 Current:    ",String(param / 1.28));
    break;
    case 5:
      param = dat.Speed2; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,104);
      dat.Speed2 = param;
      dslay_print("5.Speed2:        ",String(param / 0.8));
    break;
    case 6:
      param = dat.s2Current; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,141);
      dat.s2Current = param;
     dslay_print("6.S2 Current:     ",String(param / 1.28));
    break;
    case 7:
      param = dat.Speed3; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,104);
      dat.Speed3 = param;
     dslay_print("7.Speed3:        ",String(param / 0.8));
    break;
    case 8:
      param = dat.s3Current; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,141);
      dat.BatteryCurrent = param;
      dslay_print("8.S3 Current:   ",String(param / 1.28));
    break;
    case 9:
      param = dat.LVC; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,255);
      dat.LVC = param;
      dslay_print("9.LVC:           ",String(param / 3.285));
    break;
    case 10:
      param = dat.Tolerance; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,255);
      dat.Tolerance = param;
      dslay_print("10.Tolerance:     ",String(param / 3.285));
    break;  
    case 11:
      param = dat.Flags; 
      if (plus){bitSet(param,6); plus=0;}
      if (minus){bitClear(param,6); minus=0;}
      dat.Flags = param;
      dslay_print("11.REGEN_ON:      ",String(bitRead(param,6)));
    break;
    case 12:
      param = dat.RegenForce; 
      if (plus){param++; plus=0;}
      if (minus){param--; minus=0;}
      param=constrain(param,0,200);
      dat.RegenForce = param;
      dslay_print("12.RegenForce:   ",String(param));
    break;
    case 13:
      if (plus){plus=0; programm();}
      if (minus){minus=0; reading(); }
      dslay_print("13.Controller_RW:","+:W ; -:R");
    break;
    case 14:
      if (plus){eep_page++;plus=0;}
      if (minus){eep_page--;minus=0;}
      eep_page=constrain(eep_page,1,8);
      dslay_print("14.EEPROM  page: ",String(eep_page));
    break;
    case 15:
      if (plus){plus=0;eep_write(0);}
      if (minus){minus=0;eep_read(0);}
      dslay_print("15.EEPROM_RW:    ","+:W ; -:R");
    break;


  }
}

void setup(){ 
  Serial.begin(38400,SERIAL_8N1);
  mySerial.begin(9600);
  
  pinMode(BTN_PLUS, INPUT);
  pinMode(BTN_MINUS, INPUT);
  pinMode(BTN_MPLS, INPUT);
  pinMode(BTN_MMNS, INPUT);
  pinMode(BTN_INT, INPUT_PULLUP);
  digitalWrite(BTN_PLUS,HIGH);
  digitalWrite(BTN_MINUS,HIGH);
  digitalWrite(BTN_MPLS,HIGH);
  digitalWrite(BTN_MMNS,HIGH);
  eep_read(eep_page);
  oledInit(OLED_128x32, 0, 0, 9, 10, 400000);
  oledFill(0, 1);
  dslay_print("Infenion 4 Progger","by Felan",15,35);
  delay(2000);
  init_menu();

}

void loop() {
  
    if (!digitalRead(BTN_PLUS)){plus = 1;init_menu();}
    else if (!digitalRead(BTN_MINUS)){minus = 1;init_menu();}
    else if (!digitalRead(BTN_MPLS)){menu++;menu=constrain(menu,1,15);init_menu();}
    else if (!digitalRead(BTN_MMNS)){menu--;menu=constrain(menu,1,15);init_menu();}
    else {plus = 0;minus = 0;}
  
  delay(200);  
  
}
