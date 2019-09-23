/*

//----------------------------------------------------------------------------
// C main line
// 
// authored by Jim Sughrue.  Habitat for humanity chair project
// senses someone sitting on chair and makes chair alive.
// one big cap sense sensor underneath a chair.  Modified for pepper
// roulette wheel in 2016.  Port to Arduino Uno in 2019.
//
//----------------------------------------------------------------------------

 The circuit:
 * Audio Out - pin 9
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 
 * pepper roulette sensor 1 on pin 2
 * pepper roulette sensor 2 on pin 3
 * pepper roulette sensor 3 on pin 5
 * pepper roulette sensor 4 on pin 6
 * pepper roulette sensor 5 on pin 7

 */
 
#include "SD.h" //Lib to read SD card
#include "TMRpcm.h" //Lib to play auido
#include "SPI.h" //SPI lib for SD card

#define SD_ChipSelectPin 4 //Chip select is pin number 4
TMRpcm audio;              // declare audio object

#define AJILEMONPTRMAX 2        // number of tracks for pepper
#define NUMEXSUAVEPTRMAX 1        // number of tracks for pepper
#define TRINIDADPERFUMEPTRMAX 2 // number of tracks for pepper
#define TRICKORTREATPTRMAX 1     // number of tracks for pepper
#define AJIDULCEPTRMAX 1        // number of tracks for pepper
#define BUZZERPTRMAX  2         // number of tracks for buzzer
#define ANNOUNCEPTRMAX  10      // number of tracks for announcement
#define ANNOUNCETIMEPTRMAX  6     // number of time intervals for announcement

static char ajiLemonTrack[AJILEMONPTRMAX][4] = {"AL0","AL1"};     // these tracks announce the type of pepper
static char numexSuaveTrack[NUMEXSUAVEPTRMAX][4] = {"NS0"};          // these tracks announce the type of pepper
static char trinidadPerfumeTrack[TRINIDADPERFUMEPTRMAX][4] = {"TP0","TP1"}; // these tracks announce the type of pepper
static char trickOrTreatTrack[TRICKORTREATPTRMAX][4] = {"TT0"};      // these tracks announce the type of pepper
static char ajiDulceTrack[AJIDULCEPTRMAX][4] = {"AD0"};            // these tracks announce the type of pepper
static char buzzerTrack[BUZZERPTRMAX][4] = {"BZ0","BZ1"};         // these tracks announce failures
static char announceTrack[ANNOUNCEPTRMAX][4] = {"AN0","AN1","AN2","AN3","AN4","AN5","AN6","AN7","AN8","AN9"};         // these tracks send advertisements for players
static char test[1][5]={"test"};
//static unsigned int announce_time_interval[ANNOUNCETIMEPTRMAX] = {5000,1000,1000,6000,7000,8000};  // announce time in ms.  Make entries here unequal to entries in announce tracks so it's irregular
static unsigned int announce_time_interval[ANNOUNCETIMEPTRMAX] = {65000,3000,3000,65000,50000,65000};  // announce time in ms.  Make entries here unequal to entries in announce tracks so it's irregular

unsigned long announce_time;
  
enum stateMachine {look_for_spin_0,look_for_spin_1,look_for_spin_2,look_for_spin_3,look_for_spin_4,look_for_stop,buzzer,ajiLemon,numexSuave,trinidadPerfume,trickOrTreat,ajiDulce,announce};

int position( void );

void setup(){
  
  audio.speakerPin = 9; //Auido out on pin 9
  Serial.begin(115200); //Serial Com for debugging   
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD fail");
    return;
  }
    
  audio.setVolume(4);    //   0 to 7. Set volume level
  audio.quality(1);        //  Set 1 for 2x oversampling Set 0 for normal

  Serial.println("begin test audio.play");  
  audio.play(test[0]);     //play  test file on startup
  Serial.println("end test audio.play");  

 while(audio.isPlaying()){
      Serial.println("test isPlaying=true");    
 }
  Serial.println("test isPlaying=false");  

  announce_time = millis();  // number of ms since program started  
}

void loop()
{
//  // test all 5 roulette wheel sensors
//  if (digitalRead(2)==LOW)
//  {
//    Serial.println("2 low");
//  }
//  if (digitalRead(3)==LOW)
//  {
//    Serial.println("3 low");
//  }
//  if (digitalRead(5)==LOW)
//  {
//    Serial.println("5 low");
//  }
//  if (digitalRead(6)==LOW)
//  {
//    Serial.println("6 low");
//  }
//  if (digitalRead(7)==LOW)
//  {
//    Serial.println("7 low");
//  }

//audio.play(ajiLemonTrack[0]);     // play track
//
//while(audio.isPlaying()){
//    Serial.println("ajiLemon isPlaying=true");    
//}
//Serial.println("ajiLemon isPlaying=false");  
    
//while(1){}

  int ajiLemonPtr = 0;
  int numexSuavePtr = 0;
  int trinidadPerfumePtr = 0;
  int trickOrTreatPtr = 0;
  int ajiDulcePtr = 0;
  int buzzerPtr = 0;
  int announcePtr = 0;
  int announceTimePtr = 0;
  int state, timeout, spinState0, spinState1, spinState2, spinState3, stopState0, stopState1, stopState2;
  unsigned int positions;     // have all positions been registered for wheel rotating

while(1){
  if (state == look_for_spin_0){
     spinState0 = position();  // get position of roulette wheel.  1-5 or 0 if not discernible

     if (spinState0 != 0){
        state = look_for_spin_1;    // captured a position, lets see if it is moving in next state
     }

     if ((millis()-announce_time) > announce_time_interval[announceTimePtr]){
            state = announce;

            announceTimePtr++;                
            if (announceTimePtr >= ANNOUNCETIMEPTRMAX){
                announceTimePtr = 0;
            } 
     }   
  }else if (state == look_for_spin_1){
//     Serial.println("state=look_for_spin_1"); 
     spinState1 = position();  // get position of roulette wheel.  1-5 or 0 if not discernible
           
     timeout = 0;
     while ((spinState1 == spinState0) && (timeout < 5 )){
//        Serial.println("state=look_for_spin_1.retry"); 
        spinState1 = position();  // get position of roulette wheel.  1-5 or 0 if not discernible
        
        delay( 100 );  
  
        timeout = timeout + 1;
     }
     
     if (timeout >= 5){
        state = look_for_spin_0;   // not moving enough
     }else {
        state = look_for_spin_2;   // detected movement
     }        
  }else if (state == look_for_spin_2){
     Serial.println("state=look_for_spin_2"); 
     // moving and all 360 degrees
     positions = 0;
     timeout = 0;
     while((positions != 0x3E)&&(timeout<100)){
        Serial.println("state=look_for_spin_2.retry"); 
        positions |= (1 << position());
  
        delay( 50 );
        
        timeout = timeout + 1;            
     }
     
     if (timeout >= 100){
        Serial.println("state=look_for_spin_2.timeout");       
        state = buzzer;   // not moving enough, heckle user
     }else {
//        state = look_for_spin_3;     // determined moving over full 360 degrees, now look for stop
        state = look_for_stop;     // determined moving over full 360 degrees, now look for stop
     }        
  }else if (state == look_for_spin_3){
      Serial.println("state=look_for_spin_3"); 
     // moving and all 360 degrees
     positions = 0;
     timeout = 0;
     while((positions != 0x3E)&&(timeout<100)){
        Serial.println("state=look_for_spin_3.retry");       
        positions |= (1 << position());
  
        delay( 50 );  
        
        timeout = timeout + 1;            
     }
     
     if (timeout >= 100){
        Serial.println("state=look_for_spin_3.timeout");       
        state = buzzer;   // not moving enough, heckle user
     }else {
        state = look_for_stop;     // determined moving over full 360 degrees, now look for stop
     }    
  }else if (state == look_for_stop){
    Serial.println("state=look_for_stop");      
    stopState0 = position();  // get position of roulette wheel.  1-5 or 0 if not discernible
    delay( 500 );
    stopState1 = position();  // get position of roulette wheel.  1-5 or 0 if not discernible         
    delay( 500 );
    stopState2 = position();  // get position of roulette wheel.  1-5 or 0 if not discernible
    delay( 500 );
  
  // wait for positions to be the same
     while ((stopState0 != stopState1)||(stopState0 != stopState2)||(stopState2 != stopState1)){
    Serial.println("state=look_for_stop.retry");      
        stopState0 = position();  // get position of roulette wheel.  1-5 or 0 if not discernible
         delay( 500 );
         stopState1 = position();  // get position of roulette wheel.  1-5 or 0 if not discernible         
         delay( 500 );
        stopState2 = position();  // get position of roulette wheel.  1-5 or 0 if not discernible
         delay( 400 );
     }
     
     // select pepper as fcn of position
     if (stopState2 == 0){
        Serial.println("state=look_for_stop.buzzer");
        state = buzzer;              // not discernible - try again
     }else if (stopState2 == 1){
        Serial.println("state=look_for_stop.ajiLemon");
        state = ajiLemon; 
     }else if (stopState2 == 2){
        Serial.println("state=look_for_stop.numexSuave");
        state = numexSuave; 
     }else if (stopState2 == 3){
        Serial.println("state=look_for_stop.trinidadPerfume");
        state = trinidadPerfume; 
     }else if (stopState2 == 4){
        Serial.println("state=look_for_stop.trickOrTreat");
        state = trickOrTreat; 
     }else if (stopState2 == 5){
        Serial.println("state=look_for_stop.ajiDulce");
        state = ajiDulce; 
     }else {
        Serial.println("state=look_for_stop.fail");              
        state = look_for_spin_0;    // something wrong, start over
     }
     
  }else if (state == ajiLemon){
    audio.play(ajiLemonTrack[ajiLemonPtr]);     // play track
  
    // increase pointer and check bounds
    ajiLemonPtr++;                
    if (ajiLemonPtr >= AJILEMONPTRMAX){
        ajiLemonPtr = 0;
    }         
    
    while(audio.isPlaying()){           // wait for track to finish
//      Serial.println("isPlaying=true");  
    }
    Serial.println("isPlaying=false");         
    
    state = look_for_spin_0;    // done, start over
     
  }else if (state == numexSuave){
    audio.play(numexSuaveTrack[numexSuavePtr]);     // play track
    
    // increase pointer and check bounds
    numexSuavePtr++;                
    if (numexSuavePtr >= NUMEXSUAVEPTRMAX){
        numexSuavePtr = 0;
    }         
       
    while(audio.isPlaying()){           // wait for track to finish
//      Serial.println("isPlaying=true");  
    }
    Serial.println("isPlaying=false");    
    
    state = look_for_spin_0;    // done, start over
  }else if (state == trinidadPerfume){
    audio.play(trinidadPerfumeTrack[trinidadPerfumePtr]);     // play track
    
    // increase pointer and check bounds
    trinidadPerfumePtr++;               
    if (trinidadPerfumePtr >= TRINIDADPERFUMEPTRMAX){
        trinidadPerfumePtr = 0;
    }         
       
    while(audio.isPlaying()){           // wait for track to finish
//      Serial.println("isPlaying=true");  
    }
    Serial.println("isPlaying=false");    
     
    state = look_for_spin_0;    // done, start over
  }else if (state == trickOrTreat){
    audio.play(trickOrTreatTrack[trickOrTreatPtr]);     // play track
    
    // increase pointer and check bounds
    trickOrTreatPtr++;                
    if (trickOrTreatPtr >= TRICKORTREATPTRMAX){
       trickOrTreatPtr = 0;
    }         
       
    while(audio.isPlaying()){           // wait for track to finish
//      Serial.println("isPlaying=true");  
    }
    Serial.println("isPlaying=false");    
     
    state = look_for_spin_0;    // done, start over
  }else if (state == ajiDulce){
    audio.play(ajiDulceTrack[ajiDulcePtr]);     // play track
    
    // increase pointer and check bounds
    ajiDulcePtr++;                
    if (ajiDulcePtr >= AJIDULCEPTRMAX){
       ajiDulcePtr = 0;
    }         
       
    while(audio.isPlaying()){           // wait for track to finish
//      Serial.println("isPlaying=true");  
    }
    Serial.println("isPlaying=false");    
     
    state = look_for_spin_0;    // done, start over
  
  }else if (state == buzzer){
    Serial.println("state = buzzer");   
    audio.play(buzzerTrack[buzzerPtr]);     // play track
    
    // increase pointer and check bounds
    buzzerPtr++;                
    if (buzzerPtr >= BUZZERPTRMAX){
        buzzerPtr = 0;
    }         
       
    while(audio.isPlaying()){           // wait for track to finish
//      Serial.println("isPlaying=true");  
    }
    Serial.println("isPlaying=false");    
    
    state = look_for_spin_0;    // done, start over
  }else if (state == announce){
    Serial.println("state = announce");   
    audio.play(announceTrack[announcePtr]);     // play track
    
    // increase pointer and check bounds
    announcePtr++;                
    if (announcePtr >= ANNOUNCEPTRMAX){
        announcePtr = 0;
    }         
       
    while(audio.isPlaying()){           // wait for track to finish
//      Serial.println("isPlaying=true");  
    }
    Serial.println("isPlaying=false");

    announce_time = millis();   // reset timer
    
    state = look_for_spin_0;    // done, start over
  }else {
     state = look_for_spin_0;    // something wrong, start over
  }   
}
}



// optical sensors read low when wheel is in their position.  Negative logic.
int position( void ){

  if (digitalRead(2)==LOW)
  {
//    Serial.println("2 low");
    return 1;
  }else if (digitalRead(3)==LOW){
//    Serial.println("3 low");
    return 2;
  }else if (digitalRead(5)==LOW){
//    Serial.println("5 low");
    return 3;
  }else if (digitalRead(6)==LOW){
//    Serial.println("6 low");
    return 4;
  }else if (digitalRead(7)==LOW){
//    Serial.println("7 low");
    return 5;
  }else {
    return 0;      
  }
}
