#include <DFRobotDFPlayerMini.h>
#include<SoftwareSerial.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>


#define SENSOR_PIN26 26
#define SENSOR_PIN27 27
#define SENSOR_PIN35 35
#define SENSOR_PIN12 12
#define SENSOR_PIN13 13
//****************************** FOR DF MINI PLAYER**************************
DFRobotDFPlayerMini player;// object of the class DFRobotDFPlayerMini

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 25; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 33; // Connects to module's TX
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);


//****************************** FOR NRF TRANSRECEIVER**************************
RF24 radio(4, 5);     /*creates an  object name radio that  represents a modem connected to the Arduino.
                      Arguments 4, 5 are a digital pin numbers to which signals
                      CE and CSN are connected.*/
uint64_t pipes = 0xE8E8F0F0E1LL; //the 64 bit or 8 byte address of the module,that will receive data.

uint16_t datas[4]; // array for storing the flex values  of the 4 flex sensor that will be transmitted to receiver



uint16_t thumb;
uint16_t index_finger;
uint16_t middle_finger;
uint16_t ring_finger;
uint16_t pinky_finger;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //****************************** FOR DF MINI PLAYER**************************
  softwareSerial.begin(9600);
  
  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial))
  {
    Serial.println("OK");
    // Set volume to maximum (0 to 30).
    player.volume(30);
  }
  else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
  //****************FOR NRF TRANSRECEIVER**************************
  radio.begin();
  /*it activates the module
    1.initialize the SPI comm between module and micro controller
    2.sets default module configuration like data rate , channel frequency and other configuration
    3.aftder configuriong it checks the statjus of the module that whether its working or not
  */
  radio.openWritingPipe(pipes);
  /* nrf module has multiple pipes(channels) which serves as comm channels . Each pipe can be assigned an unique address.this func is used to setup
    one of these pipes as a primary channel for data transmission betwn two modules*/
  radio.setPALevel(RF24_PA_LOW);
  /*
    sets the power amplification level which can help optimizing the range and performance of comm based on application
    1.helps to adjust the transmission power of module
    2.by settion   RF24_PA_LOW the module transmits data by usig minimum power and hence due to this the range is decreased. this mode is used when power consumption need 
    to be minimized and the transerceiver will not be far apart . RANGE = 20 to 50 METERS. IN CASE OF PA HIGH RANGE= 100  to 200 meters
  */

  radio.stopListening();
  //***************************************************************
  pinMode(SENSOR_PIN26, INPUT);
  pinMode(SENSOR_PIN27, INPUT);
  pinMode(SENSOR_PIN35, INPUT);
  pinMode(SENSOR_PIN12, INPUT);
  pinMode(SENSOR_PIN13, INPUT);
  pinMode(14, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
start:
  thumb      = analogRead(SENSOR_PIN35);
  index_finger  = analogRead(SENSOR_PIN26);
  middle_finger  = analogRead(SENSOR_PIN27);
  ring_finger   = analogRead(SENSOR_PIN12);
  pinky_finger  = analogRead(SENSOR_PIN13);

  Serial.print(" thumb : ");
  Serial.print(thumb);

  Serial.print(" index : ");
  Serial.print(index_finger);

  Serial.print(" midle : ");
  Serial.print(middle_finger);

  Serial.print(" ring: ");
  Serial.print(ring_finger);

  Serial.print(" pinky : ");
  Serial.println(pinky_finger);


  // PUT THE CONDITION FOR THE MODE SELECTION
  if (thumb > 700 && index_finger <= 1000 && middle_finger <= 800 && ring_finger <= 800 && pinky_finger > 1000) // MUTTHI without pinky
  {
    Serial.println("Entered to  first state of car mode");
    delay(1000);//wait for 3 seconds
    //Again check after 3 seconds
    thumb         = analogRead(SENSOR_PIN35);
    index_finger  = analogRead(SENSOR_PIN26);
    middle_finger  = analogRead(SENSOR_PIN27);
    ring_finger   = analogRead(SENSOR_PIN12);
    pinky_finger  = analogRead(SENSOR_PIN13);

    if (thumb >700 && index_finger <= 1000 && middle_finger <= 800 && ring_finger <= 800 && pinky_finger > 1000) // MUTTHI without pinky  // if true that means car mode is selected
    {
      Serial.println(" car mode  activated");
      player.play(1); //Car mode activated
      digitalWrite(14, HIGH);
      delay(3000);
      while (1)
      {
        thumb        = analogRead(SENSOR_PIN35);
        index_finger  = analogRead(SENSOR_PIN26);
        middle_finger = analogRead(SENSOR_PIN27);
        ring_finger   = analogRead(SENSOR_PIN12);
        pinky_finger  = analogRead(SENSOR_PIN13);


        if (thumb > 700 && index_finger > 900 && middle_finger > 700 && ring_finger > 700 && pinky_finger <= 1000) // condition to de activate car mode
        {
          // first stop the whel chair=========
          datas[0] =900;
          datas[1] =1000;
          datas[2]=800;
          datas[3] = 800;
          radio.write(&datas, sizeof(datas));
          //====================================
          // get out of this the car mode
          Serial.println("De activated car mode");
          player.play(7); //Car mode  deactivated
          digitalWrite(14, LOW);
          delay(500);
          goto start;
        }
        else
        {
          datas[0] = thumb;
          datas[1] = index_finger;
          datas[2] = middle_finger;
          datas[3] = ring_finger;
          radio.write(&datas, sizeof(datas)); // sending the array

          Serial.print(" thumb : ");
          Serial.print(datas[0]);

          Serial.print(" index : ");
          Serial.print(datas[1]);

          Serial.print(" midle : ");
          Serial.print(datas[2]);

          Serial.print(" ring: ");
          Serial.println(datas[3]);

        }
      }
    }
    else goto start;
  }
  //   PUT THE CONDITION FOR MODE SELECTION

  else if (thumb > 700 && index_finger <= 900 && middle_finger <= 700 && ring_finger <= 700 && pinky_finger <= 1000)
  {
    Serial.println("I am okay ");
   player.play(5);
    delay(1000);
  }

  else if (thumb <= 700)
  {
    Serial.println("I need some water");
    player.play(4);
    delay(1000);
  }
  else if (index_finger <= 900)
  {
    Serial.println("Give me food");
    player.play(2);
    delay(1000);
  }
  else if (middle_finger <= 700)
  {
    Serial.println("Iam not feeling well");
    player.play(3);
    delay(1000);
  }
  else if (ring_finger <= 700)
  {
    Serial.println("Take me to bathroom");
    player.play(6);
    delay(1000);
  }


  delay(1000);
}
