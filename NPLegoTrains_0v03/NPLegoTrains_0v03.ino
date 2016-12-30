//#define DEBUG

#define NPLT_VER_0.1

// Display libraries and variables
#include <Wire.h>
#include <U8glib.h>

// menu system includes and definition
#include <menu.h>//menu macros and objects
#include <menuFields.h>
#include <menuU8G.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
menuU8G gfx(u8g,1,0,1,1,1,0,6,10);
bool runMenu=true;
bool test=false;
int param;

// NPLT library definition
#define TRACKSWITCH_A       1
#define TRACKSWITCH_B       2

#define BUTTON_TS_A         11
#define BUTTON_TS_B         13

#define NPLT_TS_DISTANCE_FW    1050
#define NPLT_TS_DISTANCE_BK    -1050

static enum {MOVING_A_CW, MOVING_A_CCW} stateA;
static enum {MOVING_B_CW, MOVING_B_CCW} stateB;

// Stepper libraries and variables
#include <AccelStepper.h>
#define HALFSTEP 8

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
// Motor pin definitions
#define NPLT_MA_PIN1  2     // IN1 on the ULN2003 driver 1
#define NPLT_MA_PIN2  3     // IN2 on the ULN2003 driver 1
#define NPLT_MA_PIN3  4     // IN3 on the ULN2003 driver 1
#define NPLT_MA_PIN4  5     // IN4 on the ULN2003 driver 1
#define NPLT_MB_PIN1  6     // IN1 on the ULN2003 driver 2
#define NPLT_MB_PIN2  7     // IN2 on the ULN2003 driver 2
#define NPLT_MB_PIN3  8     // IN3 on the ULN2003 driver 2
#define NPLT_MB_PIN4  9     // IN4 on the ULN2003 driver 2

AccelStepper stepper1(HALFSTEP, NPLT_MA_PIN1, NPLT_MA_PIN3, NPLT_MA_PIN2, NPLT_MA_PIN4);
AccelStepper stepper2(HALFSTEP, NPLT_MB_PIN1, NPLT_MB_PIN3, NPLT_MB_PIN2, NPLT_MB_PIN4);



#define NPLT_MSG_MAX_COUNT  7
#define NPLT_MSG_MAX_LENGTH 25

// messages text content definition
#define NPLT_MSG_DISPLAYLOGOTEXT "NPLegoTrains"
#define NPLT_MSG_TS_AB_INIT "Initialization"
#define NPLT_MSG_TS_AB_READY "TS A/B READY"
#define NPLT_MSG_TS_A_LEFT "TS A LEFT"
#define NPLT_MSG_TS_A_RIGHT "TS A RIGHT"
#define NPLT_MSG_TS_B_LEFT "TS B LEFT"
#define NPLT_MSG_TS_B_RIGHT "TS B RIGHT"

// messages index definition
#define NPLT_MSGI_DISPLAYLOGOTEXT 0
#define NPLT_MSGI_TS_INIT         1
#define NPLT_MSGI_TS_AB_READY     2
#define NPLT_MSGI_TS_A_LEFT       3
#define NPLT_MSGI_TS_A_RIGHT      4
#define NPLT_MSGI_TS_B_LEFT       5
#define NPLT_MSGI_TS_B_RIGHT      6

const char *displaymessages[NPLT_MSG_MAX_COUNT] = { NPLT_MSG_DISPLAYLOGOTEXT,
                                                    NPLT_MSG_TS_AB_INIT ,
                                                    NPLT_MSG_TS_AB_READY, 
                                                    NPLT_MSG_TS_A_LEFT, 
                                                    NPLT_MSG_TS_A_RIGHT, 
                                                    NPLT_MSG_TS_B_LEFT, 
                                                    NPLT_MSG_TS_B_RIGHT};;



// setup tlacitka

int buttonpin = 12; // define the key switch sensor interface

void calibrateMotorA() {

  Serial.print("Calibrating motor A in progress. PUSH button A\n");

  while(digitalRead(BUTTON_TS_A) == HIGH) {} ;
  
  Serial.print("Reaching LEFT position\n");          
  stepper1.moveTo(NPLT_TS_DISTANCE_BK);
  
  while(stepper1.distanceToGo() != 0) {
    stepper1.run();
  }  
  Serial.print("Position LEFT of motor B reached.\n");
}

void calibrateMotorB() {

  Serial.print("Calibrating motor B in progress. PUSH button B\n");

  while(digitalRead(BUTTON_TS_B) == HIGH) { Serial.println(digitalRead(BUTTON_TS_B));} ;
  
  Serial.print("Reaching LEFT position\n");          
  stepper2.moveTo(NPLT_TS_DISTANCE_FW); // value for testing purpose only

  Serial.println(digitalRead(BUTTON_TS_B));
  while(stepper2.distanceToGo() != 0) {
    stepper2.run();
  }  
  Serial.print("Position LEFT of motor B reached.\n");
}

void setup() {

  pinMode (BUTTON_TS_A, INPUT); // button for motor A pin assign
  pinMode (BUTTON_TS_B, INPUT); // button for motor B pin assign
  
  Serial.begin(115200);
   while(!Serial);
  Serial.println("menu system test");
  u8g.setFont(u8g_font_unifont);
  menu::wrapMenus=true;
  
}//--(end setup )---

promptFeedback pauseMenu(prompt &p, menuOut &o, Stream &i) {
  runMenu=false;
  o.clear();
  return false;
}

promptFeedback quit() {
  Serial.println("Quiting after action call");
  return true;
}

promptFeedback trackswitchAon() {

  switch (stateA) {
    case MOVING_A_CW:              
      stepper1.moveTo(NPLT_TS_DISTANCE_FW);  
      while(stepper1.distanceToGo() != 0)  
        {stepper1.run();}      
      stateA = MOVING_A_CCW;
      break;
    case MOVING_A_CCW:       
      stepper1.moveTo(-75); 
      while(stepper1.distanceToGo() != 0) 
        {stepper1.run();}
      stateA = MOVING_A_CW;
      break;
  Serial.println("TrackSwitch A");
  return stateA;
  }
}

promptFeedback trackswitchBon() {
  Serial.println("TrackSwitch B");
  return true;
}


TOGGLE(test,onoff_tog,"Track: ",
  VALUE("(A)",HIGH),
  VALUE("(B)",LOW)
);

int selTest=0;
SELECT(selTest,selMenu,"TrackSwitch",
  VALUE("(A)",0),
  VALUE("(B)",1)
  
);

bool ts=false;
TOGGLE(ts,track2,"led: ",
  VALUE("On",HIGH),

  VALUE("Off",LOW)
);

MENU(subMenu1,"TrackSwitch A/B"
  ,OP("1.Trackswitch (A)",trackswitchAon)
  ,OP("2.Trackswitch (B)",trackswitchBon)
  ,OP("3.Exit",quit)
);

MENU(subMenu2,"Settings"
  ,OP("1.Calibrate (A)",quit)
  ,OP("2.Calibrate (B)",quit)
  ,OP("3.Exit",quit)
);

MENU(mainMenu,"Main menu"
  ,SUBMENU(subMenu1)
  ,SUBMENU(subMenu2)
  //,FIELD(param,"Name","%",0,100,10,1)
  //,SUBMENU(sample_clock)
  ,SUBMENU(onoff_tog)
  //,OP("Nothing",prompt::nothing)
  ,OP("Exit",pauseMenu)
);

void npltMenu(void)
{
 if (runMenu) mainMenu.poll(gfx,Serial);
  else {
    u8g.setColorIndex(1);
    u8g.drawStr(0,15,"r-site.net");
    if (Serial.read()==menu::enterCode) runMenu=true;
  }  

  

   // while(stepper1.distanceToGo() != 0) {}
  
    //Serial.print(digitalRead(buttonpin));
/*
  if (digitalRead(BUTTON_TS_A) == LOW) {             
            switch (state1) {
            case MOVING_A_CW:
                if (digitalRead(BUTTON_TS_A) == LOW) {  // hit switch
                    
                    stepper1.moveTo(NPLT_TS_DISTANCE_FW);
                    draw(3, TRACKSWITCH_A);
                    while(stepper1.distanceToGo() != 0)  
                      {stepper1.run();}
                    state1 = MOVING_A_CCW;
                }
                break;
            case MOVING_A_CCW:
                if (digitalRead(BUTTON_TS_A) == LOW) {  // hit switch
                  
                  stepper1.moveTo(-75);
                  draw(4, TRACKSWITCH_A);
                  while(stepper1.distanceToGo() != 0) 
                    {stepper1.run();}
                  state1 = MOVING_A_CW;
                }
                break;
    }  ;
        }
    
  if (digitalRead(BUTTON_TS_B) == LOW) {             
            switch (state2) {
            case MOVING_B_CW:
                if (digitalRead(BUTTON_TS_B) == LOW) {  // hit switch
                    
                    stepper2.moveTo(NPLT_TS_DISTANCE_FW);
                    draw(3, TRACKSWITCH_A);
                    while(stepper2.distanceToGo() != 0)  
                      {stepper2.run();}
                    state2 = MOVING_B_CCW;
                }
                break;
            case MOVING_B_CCW:
                if (digitalRead(BUTTON_TS_B) == LOW) {  // hit switch
                  
                  stepper2.moveTo(-75);
                  draw(4, TRACKSWITCH_A);
                  while(stepper2.distanceToGo() != 0) 
                    {stepper2.run();}
                  state2 = MOVING_B_CW;
                }
                break;
            }  ;
        }   
*/  
}


void loop() {
u8g.firstPage();
  do {
    npltMenu();
  } while( u8g.nextPage() );
  
}
