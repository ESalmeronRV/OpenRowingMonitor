#include <TimerOne.h>

/* OpenRowingMonitor
 * (c) Esteban Salmeron 2021
 */

//Fonts to use:
//Text: u8g2_font_4x6_tf
//Data: u8g2_font_6x13_tf // u8g2_font_10x20_tf
//-----------------------------------------------

#define INPIN_ANALOG_REV A0        //Wheel on Analog 0
#define INPIN_ANALOG_STROKE A1     //Seat on Analog 1
#define DEBOUNCE_STROKES 300       //Debounce for strokes (See doc)
#define DEBOUNCE_REV 35 //35       //Debounce for revolutions
#define TIMER_FREQ 2000            //Interrupt timer for measuring analog inputs
#define CLICKS_TO_METERS 0.220477  //Pulses of the wheel = meters (See doc)
#define WATTS_TO_KCALH 0.859845
#define MAGIC_FACTOR 2.164         //For calories and rowed distance calculation

#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0,4,5,U8X8_PIN_NONE);

 volatile byte half_strokes = 1; //Count the 
 unsigned int vueltas = 0, SPM = 0, totalPartialMin = 0, totalPartialSeg = 0, partialMin = 0, partialSeg = 0;
 uint32_t now_rev, now_stroke;
 float latestTime_rev, latestTime_stroke;
 float  elapsedTime_rev, elapsedTime_stroke;
 float totalDistance = 0, strokeDistanceOld = 0, strokeDistance = 0, RPM = 0, totalKcal=0,  pace = 0;
 boolean showInitScreen;

 //TIMER VARS
 uint32_t inicio, finalizado, Ttranscurrido, Tinicio;
 float h = 0, m = 0, s = 0, ms = 0;
 unsigned long terminado;

 //Debounce
 volatile uint32_t last_click_rev = 0;
 volatile uint32_t last_click_stroke = 0;
 
 void setup()
 {
   Timer1.initialize(TIMER_FREQ);  // 10 ms
   Timer1.attachInterrupt( analogReading ) ;
   //Serial.begin(115200); //Disabled by default
   u8g2_SetI2CAddress(u8g2.getU8g2(), 0x3C * 2);
   u8g2.begin();
   showInitScreen = true;
   totalKcal = 0;
 }

 void analogReading(){
  int sensorValue0 = !digitalRead(INPIN_ANALOG_REV);
  int sensorValue1 = !digitalRead(INPIN_ANALOG_STROKE);
  if(sensorValue0){
      rev_detect();
  }  
    if(sensorValue1){
      stroke_detect();  
    }
 }
 
 void printGrid(){
  //drawline from x1y1 to x2y2 (x1, y1, x2, y2)
  //vertical lines
  u8g2.drawLine(86,0,86,20);
  u8g2.drawLine(64,20,64,40);
  u8g2.drawLine(86,40,86,64);
  //horizontal lines
  u8g2.drawLine(0,20,128,20);
  u8g2.drawLine(0,40,128,40);
 }

 void printInitScreen(){
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.setCursor(10,12);
  u8g2.print("Open Rowing Monitor");
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.setCursor(3,35);
  u8g2.print("Start rowing to begin");
  u8g2.setFont(u8g2_font_4x6_tf);
  u8g2.setCursor(80,60);
  u8g2.print("By Esteban91");
  u8g2.setCursor(0,60);
  u8g2.print("Version 0.1");
 }
 
 void printScreenData(){
  
  printGrid();
  
  //Total distance
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.setCursor(20,15);
  u8g2.print(totalDistance);
  //u8g2.print("7345.23");
  u8g2.setFont(u8g2_font_4x6_tf);
  u8g2.setCursor(80,18);
  u8g2.print("m");
  
  //SPM
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.setCursor(94,15);
  u8g2.print(SPM);
  u8g2.setFont(u8g2_font_4x6_tf);
  u8g2.setCursor(115,18);
  u8g2.print("spm");
  
  //Training time
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.setCursor(10,35);
  if(int(h)<10){
    u8g2.print("0");
  }
  u8g2.print(int(h));
  u8g2.print(":");
  if(int(m)<10){
    u8g2.print("0");
  }
  u8g2.print(int(m));
  u8g2.print(":");
  if(int(s)<10){
    u8g2.print("0");
  }
  u8g2.print(int(s));
    
  // /500m total
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.setCursor(72,35);
  if(totalPartialMin<10){
    u8g2.print("0");
  }
  if(totalPartialMin < 99){
    u8g2.print(totalPartialMin);
  }
  else{
    u8g2.print("99");
  }
  u8g2.print(":");
  if(totalPartialSeg<10){
    u8g2.print("0");
  }
  u8g2.print(totalPartialSeg);
  u8g2.setFont(u8g2_font_4x6_tf);
  u8g2.setCursor(105,38);
  u8g2.print("/500m");
  
  // /500m current
  u8g2.setFont(u8g2_font_10x20_tf);
  u8g2.setCursor(10,60);
  u8g2.print(partialMin);
  u8g2.print(":");
  if(partialSeg<10){
    u8g2.print("0");
  }
  u8g2.print(partialSeg);
  u8g2.setFont(u8g2_font_4x6_tf);
  u8g2.setCursor(60,63);
  u8g2.print("/500m");
  
  // kcal
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.setCursor(92,60);
  u8g2.print(int(totalKcal));
  u8g2.setFont(u8g2_font_4x6_tf);
  u8g2.setCursor(115,63);
  u8g2.print("cal");

 }

 void debug_rev(){
  Serial.println("REV detected");
  Serial.println(RPM);
 }

 void debug_stroke(){
  Serial.print("Stroke detected");
  Serial.println(" ");
 }

 void calculateTotalPartial(unsigned long Ttranscurrido, float totalDistance){
  int seg500m = 0;
  seg500m = int(500 * (Ttranscurrido / (totalDistance * 1000))); //seg / 500m
  totalPartialMin = int(seg500m/60);
  totalPartialSeg = int(seg500m%60);
 }

 void calculatePartial(unsigned long elapsedTime_stroke, float strokeDistance){
  int seg500m = 0;
  seg500m = int(500 * (elapsedTime_stroke / (strokeDistance * 1000))); //seg / 500m
  partialMin = int(seg500m/60);
  partialSeg = int(seg500m%60);
  pace =  elapsedTime_stroke / (strokeDistance * 1000);
 }

 void calculateCaloriesLastStroke(float elapsedTime_stroke, float pace){
  float watts = MAGIC_FACTOR/pow(pace,3); //https://www.concept2.com/indoor-rowers/training/calculators/watts-calculator
  double wattsH = double(watts * (elapsedTime_stroke/360000));
  if (wattsH * WATTS_TO_KCALH < 1000){
    totalKcal = totalKcal + ( wattsH * WATTS_TO_KCALH);
  }
 }
 
 void loop()
 {
  u8g2.firstPage();
  do{
    if( showInitScreen ){
      printInitScreen();
      Tinicio = millis();
      
    }else{
      printScreenData();
      timer();
      calculateTotalPartial(Ttranscurrido, totalDistance);
      calculatePartial(elapsedTime_stroke, strokeDistance);
    }
  } while ( u8g2.nextPage() );
  delay(100);
 }

 void timer(){
    Ttranscurrido = millis() - Tinicio;
    h = int(Ttranscurrido / 3600000);
    terminado = Ttranscurrido % 3600000; 
    m = int(terminado / 60000);
    terminado = terminado % 60000;
    s = int(terminado / 1000);
    ms = terminado % 1000; 
 }

 
 void rev_detect()
 {
  uint32_t now = millis ();
    if (!last_click_rev || (now - last_click_rev > DEBOUNCE_REV)) {
      last_click_rev = millis ();
      //debug_rev(); //Disabled by default
      elapsedTime_rev = now - latestTime_rev;
      latestTime_rev = now;
      vueltas++;
      RPM = 1/(elapsedTime_rev/1000);
      totalDistance = vueltas * CLICKS_TO_METERS;
      showInitScreen = false;
   }
 }

 void stroke_detect()
 { 
    
    uint32_t now = millis ();
    if (!last_click_stroke || (now - last_click_stroke > DEBOUNCE_STROKES)) {
      last_click_stroke = millis ();
      half_strokes++;
      if(half_strokes == 2){
        //debug_stroke(); //Disabled by default
        now_stroke = millis();
        elapsedTime_stroke = now_stroke - latestTime_stroke;
        latestTime_stroke = now_stroke;
        half_strokes = 0;
        SPM = int(1/(elapsedTime_stroke/60000));
        strokeDistance = totalDistance - strokeDistanceOld;
        strokeDistanceOld = totalDistance;
        calculateCaloriesLastStroke( elapsedTime_stroke, pace);
    }
   }
 }
