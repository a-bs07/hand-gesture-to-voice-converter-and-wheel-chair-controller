#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h> //the library which helps us to control the radio modem (nRF24L)
#include <RF24_config.h>
#include <SPI.h>      //the communication interface with the modem     

/*
//Motor A
const int RightMotorForward = 3;    // IN1
const int RightMotorBackward = 2;   // IN2
//Motor B
const int LeftMotorForward = 5;     // IN3
const int LeftMotorBackward = 4;    // IN4
*/

//Motor A
const int RightMotorForward = 5;    // IN1
const int RightMotorBackward = 6;   // IN2
//Motor B
const int LeftMotorForward = 7;     // IN3
const int LeftMotorBackward = 8;    // IN4

const int led = 2; 

RF24 radio(10,9);     /*This object represents a modem connected to the Arduino. 
                      Arguments 10 and 9 are a digital pin numbers to which signals 
                      CE and CSN are connected.*/

const uint64_t pipes = 0xE8E8F0F0E1LL; //the address of the modem,that will receive data from the Arduino.
uint16_t datas[4];

//int num;

void setup(){
  Serial.begin(9600);
  radio.begin();                    //it activates the modem.
  radio.openReadingPipe(1, pipes);   
  /*
   * nrf module has upto 6 pipes for receiving data .each pipe can be set to receive to diff address.
   * pipe number = 0 to 5.
   * the first operand i.e 1 represent the second pipe  that is configured to receive data from the specified address
   * pipes represent the address from which data will be received 
   * 
   */
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
  
  pinMode(RightMotorForward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT); 
   pinMode(led, OUTPUT); 
     digitalWrite(led,LOW);
          //enable receiving data via modem
  }

void loop(){
  if(radio.available()){

  radio.read(&datas, sizeof(datas));
  digitalWrite(led,HIGH);
  Serial.print(datas[0]);
  Serial.print(" ");
  Serial.print(datas[1]);
  Serial.print(" ");
  Serial.print(datas[2]);
  Serial.print(" ");
  Serial.println(datas[3]);
 
      if(datas[1] <=900){
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    Serial.println("FORWARD");
    }

    if(datas[2]<=700){
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, HIGH);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, HIGH);
    Serial.println("BACKWARD");
    }
    
    if(datas[0] <= 700 ){
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, HIGH);
    Serial.println("TURN LEFT");
    }

  if(datas[3] <= 700 ){
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, HIGH);
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    Serial.println("TURN RIGHT");
    }

    if(datas[0] > 700 && datas[1] >900 && datas[2] > 700 && datas[3] > 700 ){
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, LOW);
    Serial.println("STOP");
    }

  }
  else
  {
      digitalWrite(led,LOW);
  }
}
