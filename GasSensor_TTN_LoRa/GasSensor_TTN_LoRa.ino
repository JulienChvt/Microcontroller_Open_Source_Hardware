// ------------------------------------------------------
// ------------------ GAS SENSOR MQ-9 -------------------
// ------------------------------------------------------
// http://wiki.seeedstudio.com/Grove-Gas_Sensor-MQ9/
//
// Gas : Propane, Butane, Monoxyde de carbone, Methane
//
// Arduino  <-->  MQ-9
//   5V     <-->  VCC
//  GND     <-->  GND
//   NC     <-->  NC
//   A0     <-->  SIG
//
// ------------------------------------------------------
// ----------------- LoRa Module RN2483 -----------------
// ------------------------------------------------------
// Arduino  <-->  RN2483
//   10     <-->    TX
//   11     <-->    RX
//   12      <-->   RST
//   3V3    <-->    3V3
//   GND    <-->    GND
//
// ------------------------------------------------------
// ----------------------- Buzzer -----------------------
// ------------------------------------------------------
// Arduino  <-->  Buzzer
//    9     <-->    +
//   GND    <-->    -
//
// ------------------------------------------------------
// -------------- Trigger de Schmitt --------------------
// ------------------------------------------------------
//
//R1 = 4.7k
//R2 = 100k
//AOP = LM741
//Vref = 2V
//
//
#include <TheThingsNetwork.h>
#include <SoftwareSerial.h>
#include <MsTimer2.h>
#define interruptPin 3
#define buzzerPin 9
#define RX 10
#define TX 11
#define pinRST 12
#define debugSerial Serial
#define freqPlan TTN_FP_EU868


//Global variables
volatile bool stateAlarm = false;
volatile bool alert = false;
float calibrationGazSensor = 0;
int i = 0;


//TTN device infos
const char *devAddr = "260113D3";
const char *nwkSKey = "6D74F323D978A615F5A1484556C6E30A";
const char *appSKey = "60D6025A351BD9D6E55E2FCBC710F8F5";


SoftwareSerial loraSerial(RX, TX);
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup() {
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  pinMode(interruptPin, INPUT);

  //Initialize the buzzer and the timer for the alarm
  pinMode(buzzerPin, OUTPUT);
  MsTimer2::set(1000, AlarmOn);


  //Reset RN2483 for init
  pinMode(pinRST, OUTPUT);
  digitalWrite(pinRST, LOW);
  delay(500);
  digitalWrite(pinRST, HIGH);
  delay(2000);

  //TTN Activation Method : ABP
  ttn.personalize(devAddr, nwkSKey, appSKey);

  //Set callback for incoming messages
  ttn.onMessage(calibration);

  //Monitor interruptPin and execute GasAlert function when the interruptPin goes from low to high or from high to low
  attachInterrupt(digitalPinToInterrupt(interruptPin), GasAlert, CHANGE);

}

void loop() {

  //Use of the poll function to look if there is an incoming downlnk message from the NodeRED dashboard
  while (i < 3) {
    ttn.poll();
    i++;
    delay(3000);
  }
  //Check if there is an alert
  //If yes, start the alarm & send an alert message (byte[0]=1)
  //We need to do this in the loop cause it's impossible to send message on the interrupt function
  //because both the interruption and the ttn.send(t) use the serial
  if (alert) {
    SendAlertMessage();
    //Start Alarm
    MsTimer2::start();
  }
  else {
    //Send every 5 seconds the value of the gas sensor
    byte data[3];
    data[0] = 00;
    data[1] = 00;
    data[2] = 61;

    //Payload(0, data);

    debugSerial.print("Sensor volt : ");
    debugSerial.print(data[1] + data[2] / 100.0);
    debugSerial.println(" V");

    //Send data to TTN
    ttn.sendBytes(data, sizeof(data));

    data[0] = 01;
    data[1] = 01;
    data[2] = 58;
    ttn.sendBytes(data, sizeof(data));
  }

  i = 0;
}



//This function calculates an overage over 100 values sent by the gaz sensor,
//converts it in voltage and return this value (float)
float GasSensorAverageVolt() {
  float tmp_sensor_value = 0, sensor_volt, sensor_mean;

  for (int i = 0 ; i < 100 ; i++) {
    tmp_sensor_value += analogRead(A0);
  }

  sensor_mean = tmp_sensor_value / 100.0;
  sensor_volt = sensor_mean / 1024 * 5.0;

  return sensor_volt;
}



//Define the payload to send to TTN
//
//Arguments :
//    - alert (int) => 0 : no gas alert ; 1 : gas alert
//    - payload[] (byte) => data to send
//          Byte 0 = alert
//          Byte 1 = Integer part of the float value return by the GasSensorAverageVolt function
//          Byte 2 = Decimal part of the float value return by the GasSensorAverageVolt function
void Payload(int alert, byte payload[]) {

  float tmp_sensor_volt;
  int intPart, decPart;

  //----------- Byte 0 ------------
  payload[0] = alert;

  //--------- Bytes 1 & 2 ---------
  //Get the float value of the gas sensor
  //Split the integer part and the decimal part
  //Put respectively the integer part and the decimal part into payload[1] and payload[2]
  tmp_sensor_volt = GasSensorAverageVolt() - calibrationGazSensor;

  if (tmp_sensor_volt < 0) {
    tmp_sensor_volt = 0;
  }

  intPart = (int)tmp_sensor_volt;
  decPart = (tmp_sensor_volt - intPart) * 100;

  payload[1] = intPart;
  payload[2] = decPart;
}




//Function used to display, on the serial monitor, the incoming TTN downlink messages
void calibration(const uint8_t *payload, size_t size, port_t port)
{
  debugSerial.println("-- DOWNLINK MESSAGE --");
  debugSerial.print("Received " + String(size) + " bytes on port " + String(port) + ":");

  for (int i = 0; i < size; i++)
  {
    debugSerial.print(" " + String(payload[i]));
  }

  debugSerial.println();

  //If the calibration button is press on the Node Red Dashboard (=> TTN downlink message = 05)
  //Then define the calibration offset of the gas sensor
  if (payload[0] == 5 && payload[1] == 0 && size == 1)
  {
    calibrationGazSensor = GasSensorAverageVolt();
    debugSerial.print("Calibration OK ! Offset = ");
    debugSerial.println(calibrationGazSensor);
  }
}




//Function call by attachInterrupt to set the global var alert to true if interruptPin is set to HIGH
//or set alert to false and stop the alarm if interruptPin is set to LOW
//WARNING : This function must have no parameter and return nothing
//          delay() doesn't work on interrup function
void GasAlert() {
  if (digitalRead(interruptPin)) {
    debugSerial.println("----- GAS ALERT -----");
    alert = true;
  }
  else {
    alert = false;
    GasAlertFinish();
  }
}


void SendAlertMessage() {
  byte data[3];
  Payload(1, data);

  //Send data to TTN
  ttn.sendBytes(data, sizeof(data));
  debugSerial.print("Alert sent !");
}



//Start the alarm and switch the buzzer state
void AlarmOn()
{
  if (stateAlarm)
  {
    analogWrite(buzzerPin, 5);
    stateAlarm = false;
  }
  else
  {
    analogWrite(buzzerPin, 0);
    stateAlarm = true;
  }
}



//Function to stop the alarm
void GasAlertFinish() {
  debugSerial.println("----- GAS ALERT FINISHED -----");
  MsTimer2::stop();
  analogWrite(buzzerPin, 0);
}

