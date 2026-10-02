#include <pt.h>
#include <Wire.h>
#include <U8x8lib.h>

//defining the led, button and potentiometer pins 
#define LED_PIN 4           // D4 port 
#define BUTTON_PIN 6        // D6 port
#define POTENTIOMETER_PIN A0

static struct pt pt_button, pt_potentiometer, pt_led, pt_display;
bool ledState = false;  
int potValue = 0;   

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA);

//initializing the timestamps (start and end time & cycle time)
unsigned long startTime, endTime;
unsigned long cycleStartTime;
unsigned long lastDebounceTime = 0;

const unsigned long debounceDelay = 0;  //debounce time in ms
bool lastLedState = false;
int lastPotValue = -1;

void setup(){
  //LED_PIN acts as an output meaning is used to send signals
    pinMode(LED_PIN, OUTPUT); 
  
  //BUTTON_PIN acts as in input meaning is used to receive signals 
    pinMode(BUTTON_PIN, INPUT);
    Serial.begin(38400);

    u8x8.begin();
    u8x8.setFont(u8x8_font_chroma48medium8_r);

  //initializing the protothreads that we are using 
    PT_INIT(&pt_button);
    PT_INIT(&pt_potentiometer);
    PT_INIT(&pt_led);
    PT_INIT(&pt_display);
}

void loop() {

    cycleStartTime = micros();  //Recording the start time of the cycle

//macro that manages the execution of a protothread
    PT_SCHEDULE(protothread_button(&pt_button));
    PT_SCHEDULE(protothread_potentiometer(&pt_potentiometer));
    PT_SCHEDULE(protothread_led(&pt_led));
    PT_SCHEDULE(protothread_display(&pt_display));

    //Calculating and printing the elapsed time for the cycle
    unsigned long cycleEndTime = micros();   //ending the cycle's time recording 
    Serial.print("Full Cycle Time: ");
    Serial.println(cycleEndTime - cycleStartTime);   //calculating and printing the full cycle's elapsee time
}

//protothread function about the button. Handles button inputs 
static int protothread_button(struct pt *pt) {

    PT_BEGIN(pt);   //beginning of the protothread
    static int buttonState = 0;   //declaring the button state
    
    while (1) {

        startTime = micros();   //starting recording the time
        int currentState = digitalRead(BUTTON_PIN);
        if (currentState != buttonState) {
            lastDebounceTime = millis();
        }
        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (currentState == HIGH) {
                ledState = !ledState;
            }
        }
        buttonState = currentState;
        endTime = micros();
        Serial.print("Button Protothread Time: ");
        Serial.println(endTime - startTime);

        PT_YIELD(pt);  //allows other protothreads to run
    }

    PT_END(pt);   //end of the protothread 
}


//protothread function about the potentiometer. Handles potentiometer readings
static int protothread_potentiometer(struct pt *pt) {
    PT_BEGIN(pt);  //beginning of the protothread

    while (1) {
        startTime = micros();   //starting recording the time
        potValue = analogRead(POTENTIOMETER_PIN);
        endTime = micros();   //ending recording the time

        Serial.print("Potentiometer Protothread Time: ");
        Serial.println(endTime - startTime);  //printing the caclulating potentiometer protothread execution time

        PT_YIELD(pt);  //allows other protothreads to run
    }

    PT_END(pt);  //end of the protothread 
}

//protothread function about the led. Manages LED state changes 
static int protothread_led(struct pt *pt) {
    PT_BEGIN(pt);   //beginning of the protothread 

    while (1) {
        startTime = micros();   //starting recording the time
        analogWrite(LED_PIN, ledState ? map(potValue, 0, 1023, 0, 255) : 0);
        endTime = micros(); //ending recording the time

        Serial.print("LED Protothread Time: ");
        Serial.println(endTime - startTime);  //printing the execution time regarding the led's protothread time

        PT_YIELD(pt);  //allows other protothreads to run
    }
    PT_END(pt);  //end of the protothread
}

//protothread function about the oled display. Manages updates to the display
static int protothread_display(struct pt *pt) {
    PT_BEGIN(pt);   //beginning of the protothread

    while (1) {
        startTime = micros();  //starting recording the time 
        if (ledState != lastLedState || potValue != lastPotValue) {
            // Only update the parts of the display that have changed
            if (ledState != lastLedState) {
                u8x8.setCursor(0, 0);
                u8x8.print("LED State: ");
                u8x8.print(ledState ? "ON " : "OFF");
                lastLedState = ledState;
            }

            if (potValue != lastPotValue) {
                u8x8.setCursor(0, 1);
                u8x8.print("Pot Value: ");
                u8x8.print(potValue);
                lastPotValue = potValue;
            }
        }
        endTime = micros();   //ending recording the time
        Serial.print("Display Protothread Time: ");
        Serial.println(endTime - startTime);   //printing the execution time regarding the oled's protothread time
        
        PT_YIELD(pt);  //allows other protothreads to run
    }

    PT_END(pt);   //end of the protothread 
}
