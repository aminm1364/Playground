#include <Arduino.h>

/*
  ESP8266MOD 12-F
  Push button to control built-in LED status

  Wiring:
    Connect the button between GPIO D1 and GND.
    Add a 10kΩ pull-down resistor between GPIO D1 and GND (instead of using the internal pull-up resistor)
      or use this code : pinMode(BUTTON_PIN, INPUT_PULLUP); // Configure button pin with internal pull-up resistor

*/

#define BUTTON_PIN D1       // GPIO pin where the button is connected
#define LED_PIN LED_BUILTIN // Onboard LED pin

bool ledState = false;       // Variable to store LED state
bool lastButtonState = HIGH; // Variable to store last button state

void setup()
{
  Serial.begin(9600);          // Start Serial communication at 9600 baud rate
  pinMode(BUTTON_PIN, INPUT);  // Configure button pin as input (without pull-up)
  pinMode(LED_PIN, OUTPUT);    // Configure LED pin as output
  digitalWrite(LED_PIN, HIGH); // Ensure LED is off initially (active-low)
}

void loop()
{
  // Read the current state of the button
  bool currentButtonState = digitalRead(BUTTON_PIN);

  // Process the button press
  if (currentButtonState == LOW && lastButtonState != currentButtonState)
  {
    lastButtonState = currentButtonState;
    ledState = !ledState; // Toggle LED state
    digitalWrite(LED_PIN, ledState ? LOW : HIGH);
    Serial.println(ledState ? "Button pressed: LED ON" : "Button pressed: LED OFF");
    delay(50);
  }

  if (currentButtonState == HIGH && lastButtonState == LOW)
  {
    lastButtonState = HIGH;
  }
}
