#include<dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

// TEMPERATURE AND HUMIDITY SENSOR
static const int DHT_SENSOR_PIN = 2;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

// Fan
int Fan = 4;

// LEDS
int greenLight = 10;
int yellowLight = 9;
int redLight = 8;

// C02 Buttons
int button_C02 = 5;

void setup() {
  // We define digital LED parameters
  pinMode(greenLight, OUTPUT);
  pinMode(yellowLight, OUTPUT);
  pinMode(redLight, OUTPUT);
  
  // Buttons initialization
  pinMode(button_C02, INPUT_PULLUP);

  // Fan
  pinMode(Fan, OUTPUT);

  // Initializate the serial port
  Serial.begin(9600);
}

// Temperature and humidity mesearumente every 4 seconds
static bool measure_environment( float *temperature, float *humidity ) {
  static unsigned long measurement_timestamp = millis();

  // Measire once every four seconds. 
  if( millis() - measurement_timestamp > 3000ul ) {
    if(dht_sensor.measure( temperature, humidity ) == true ) {
      measurement_timestamp = millis();
      return (true);  
    }
  }

  return (false);
}

void loop() {
  float temperature;
  float humidity;

  if( measure_environment( &temperature, &humidity) == true ) {
    Serial.print("T = ");
    Serial.print(temperature, 1);
    Serial.print("deg. C, H = ");
    Serial.print(humidity, 1);
    Serial.print("%\n");

    // In temperature the efectiveness zone to avoid
    // deseases like covid is > 30º
    // That's why we only stablish a human comfort zone
    // control because that condition is not real.
    unsigned int temperature_state;
    if(temperature < 21 || temperature > 25) {
      temperature_state = 1;
    } else {
      temperature_state = 0;
    }

    // For the humidity state whe have set as 
    // condition:
    // 40% <= x <= 60% green
    // x < 40% yellow
    // x < 20% red
    unsigned int humidity_state;
    if(humidity < 20) {
      humidity_state = 2; // red
    } else if(humidity < 40) {
      humidity_state = 1; // yellow
    } else {
      humidity_state = 0; // green
    }

    // For C02 whe set a 'suposition' button
    // that changes the state, real conditions:
    // x < 500ppm green
    // 500ppm <= x < 800ppm yellow
    // 800 <= x red
    unsigned int C02_state = 0;
    if(digitalRead(button_C02) == LOW) {
      C02_state = 2;
    }

    // There are parameters whe don't have contemplated in 
    // this prototype. And normally it would be based on the
		// average measurement of each value, but we don't have 
		// implemented this simple code this way for a real time 
		// demonstration.

    // For the real device implentation whe would use 
    // a score parameter, where each parameter have a
    // percentage relevance which conditionate the 
    // general state of the room.
    // General_score = C02_score_percentaje*0.7 + HR_score_percentaje*0.5 + T_score_percentaje*03
    // (Max percentaje 1.5)
    // State 0 <= 0.60 < State 1 <= 1.0 <= State 2 
    if(humidity_state == 2 || C02_state == 2) {
      digitalWrite(greenLight, LOW);
      digitalWrite(yellowLight, LOW);
      digitalWrite(redLight, HIGH);
      
      // Emergy automatization process
      digitalWrite(Fan, HIGH);
      
    } else if (humidity_state == 1 || C02_state == 1 || temperature_state == 1) {
      digitalWrite(greenLight, LOW);
      digitalWrite(yellowLight, HIGH);
      digitalWrite(redLight, LOW);
      digitalWrite(Fan, LOW);

      if(humidity_state == 1) {
        // Humidity reduction process or 
        // recomendation 
      }

      if(C02_state == 1) {
        // C02 reduction process or 
        // recomendation
      }

      if(temperature_state == 1) {
        // Temperature reduction process or
        // recomendation
      }
    } else {
      digitalWrite(greenLight, HIGH);
      digitalWrite(yellowLight, LOW);
      digitalWrite(redLight, LOW);
      digitalWrite(Fan, LOW);
    } 

    Serial.print(humidity_state, 1);
    Serial.print(" ");
    Serial.print(C02_state, 1);
    Serial.print(" ");
    Serial.print(temperature_state, 1);
    Serial.print("\n");

    // Here whe would transmit the data and state 
    // from the arduino to the raspberry pi.
    // The raspberry is connected to the local wifi and
    // upload all the information (register) to the database.
  }
}
