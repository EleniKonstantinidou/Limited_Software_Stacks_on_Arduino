// Note: Evaluated as a stress-test on ATmega328P (2 KB SRAM).
// Encounters SRAM exhaustion when allocating 4 concurrent tasks + semaphores.

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Wire.h>
#include <U8x8lib.h>

// defining the led, button and potentiometer pins 
#define LED_PIN 4           // D4 port 
#define BUTTON_PIN 6        // D6 port
#define POTENTIOMETER_PIN A0

bool ledState = false;  
int potValue = 0;   

// initializing the OLED display
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA);

// initializing the timestamps (start and end time & cycle time)
unsigned long startTime, endTime;
unsigned long cycleStartTime;
unsigned long lastDebounceTime = 0;

const unsigned long debounceDelay = 0;  // debounce time in ms
bool lastLedState = false;
int lastPotValue = -1;

//Initiallizing Task handles
TaskHandle_t TaskButton;
TaskHandle_t TaskPotentiometer;
TaskHandle_t TaskLed;
TaskHandle_t TaskDisplay;

//Initiallizing Semaphore handles
SemaphoreHandle_t sem_button;
SemaphoreHandle_t sem_potentiometer;
SemaphoreHandle_t sem_led;
SemaphoreHandle_t sem_display;

void setup() {
    // LED_PIN acts as an output meaning is used to send signals
    pinMode(LED_PIN, OUTPUT); 

    // BUTTON_PIN acts as in input meaning is used to receive signals 
    pinMode(BUTTON_PIN, INPUT);
    Serial.begin(38400);   // baud

    u8x8.begin();
    u8x8.setFont(u8x8_font_chroma48medium8_r);

    // initializing the counting semaphores with max count 1
    sem_button = xSemaphoreCreateCounting(1, 1);
    sem_potentiometer = xSemaphoreCreateCounting(1, 1);
    sem_led = xSemaphoreCreateCounting(1, 1);
    sem_display = xSemaphoreCreateCounting(1, 1);

    //Creating tasks
    xTaskCreate(TaskButtonCode, "Button Task", 128, NULL, 1, NULL);
    xTaskCreate(TaskPotentiometerCode, "Potentiometer Task", 128, NULL, 1, NULL);
    xTaskCreate(TaskLedCode, "LED Task", 128, NULL, 1, NULL);
    xTaskCreate(TaskDisplayCode, "Display Task", 128, NULL, 1, NULL);

    xSemaphoreGive(sem_button);
}

void loop() {
    
}

// Task function for the button
void TaskButtonCode(void *pvParameters) {
    (void) pvParameters;

    int buttonState = 0;

    for (;;) {
        if (xSemaphoreTake(sem_button, portMAX_DELAY) == pdTRUE) {
            startTime = micros();
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
            Serial.print("Button Task Time: ");
            Serial.println(endTime - startTime);

            xSemaphoreGive(sem_potentiometer);
            vTaskDelay(1);  // Yield to other tasks
        }
    }
}

// Task function for the potentiometer
void TaskPotentiometerCode(void *pvParameters) {
    (void) pvParameters;

    for (;;) {
        if (xSemaphoreTake(sem_potentiometer, portMAX_DELAY) == pdTRUE) {
            startTime = micros();
            potValue = analogRead(POTENTIOMETER_PIN);
            endTime = micros();

            Serial.print("Potentiometer Task Time: ");
            Serial.println(endTime - startTime);

            xSemaphoreGive(sem_led);
            vTaskDelay(1);  // Yield to other tasks
        }
    }
}

// Task function for the LED
void TaskLedCode(void *pvParameters) {
    (void) pvParameters;

    for (;;) {
        if (xSemaphoreTake(sem_led, portMAX_DELAY) == pdTRUE) {
            startTime = micros();
            analogWrite(LED_PIN, ledState ? map(potValue, 0, 1023, 0, 255) : 0);
            endTime = micros();

            Serial.print("LED Task Time: ");
            Serial.println(endTime - startTime);

            xSemaphoreGive(sem_display);
            vTaskDelay(1);  // Yield to other tasks
        }
    }
}

// Task function for the display
void TaskDisplayCode(void *pvParameters) {
    (void) pvParameters;

    for (;;) {
        if (xSemaphoreTake(sem_display, portMAX_DELAY) == pdTRUE) {
            startTime = micros();

            if (ledState != lastLedState || potValue != lastPotValue) {
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

            endTime = micros();
            Serial.print("Display Task Time: ");
            Serial.println(endTime - startTime);

            xSemaphoreGive(sem_button);
            vTaskDelay(1);  // Yield to other tasks
        }
    }
}
