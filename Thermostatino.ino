/********************************************
                                                Thermostatino
                            Thermostat Arduino - Home autonomation open source
                                                Version: 1.0

   Author and developer: Newton Urbanetz
   Contact: new.urbanetz@gmail.com

   Created date: 10/2015

   Library used: Nextion arduino, SoftwareSerial and DHT11

   Nextion arduino : https://github.com/bborncr/nextion

   =======================================

   Material used:
   # 1 Arduino Uno compatible
   # 1 Nextion 2,4'
   # 1 Sensor DHT11
   # 1 Buzzer
   # 2 resistances 220 ohm
   # 1 relay
   # 1 power button, 1 reset button


   Nextion Display  | Relay         | DHT11         | Buzzer        | Button reset
                    |               |               |               |
   VCC  VCC         | VCC   VCC     | VCC   VCC     | VCC   6       | VCC   7
   GRND GRND        | GRND  GRND    | GRND  GRND    | GRND  GRND    | GRND  GRND(or 5v,check)
   TX   2           | PIN   5       | PIN   4       |               |
   RX   3           |               |               |               |


************************************************/

/*
    #
    #
    #   I include the essential libraries for operating both Nextion display
    #   and temperature and humidity DHT11 sensor
    #
    #########
*/

#include <SoftwareSerial.h>
#include <Nextion.h>
#include <dht11.h>

/*
    ################################################################
    #                                                              #
    #   I create the essential variables for operating the system  #
    #                                                              #
    ################################################################

    DHT         =>  is the variable that needs to communicate with the DHT11 sensor, declared with the
                    dht11 library. In this case, it's necessary to connect the arduino's board pin 4

    temp        =>  temperature management, reads the temperature's values
    humdt       =>  humidity management, reads the humidity's values

    buttonState =>  it's necessary to create a state when is pressed the power button on the Nextion display
    bright       =>  it creates a state to limit to 0 the display brightness

    checkTheme  =>  it is used to organise the display's theme. It creates an interval
                    to examine which theme display in according to the setted values.
                    In this case 255 (that corresponds to 4,25 minutes)

    timerPlus   =>  it will be the timer to visualize on the display, in seconds
    timerMinus  =>  it's the regressive timer, it needs to visualize the programmed
                    time and the time it remained activated for 6 seconds

    chk         =>  read the informations of the DHT11 sensor
*/

dht11 DHT;
#define DHT11_PIN 4

int temp, humdt; // see and decide if it is better float or int

boolean buttonState = false; // see and decide if it is better boolean or int
boolean bright = true; // see and decide if it is better boolean or int
int checkTheme = 0;
int timerPlus, timerMinus, chk;

/*
    Nextion TX to pin 2 and RX to pin 3 of Arduino
*/

SoftwareSerial nextion(2, 3);

/*
    create a Nextion object named myNextion using the nextion serial port @ 9600bps
*/

Nextion myNextion(nextion, 9600);

/*
    The function theme(), needs to change the images on the display in according to the setted values.
    The themes are:
        1 - Sun: when the temperature and the humidity are in the standard
        2 - Raindrops: when the humidity goes beyond the 65%
        3 - Winter: when the temperature is the same or inferior to 5°C
*/

void theme() {

  if (temp >= 23 && humdt <= 65) {

    /*
        setting parameters of Nextion elements
        in this case: "page0" image of the sun

        set up the different images (see the names on the file to load on the Nextionp
    */

    myNextion.sendCommand("page0.pic=1");
    myNextion.sendCommand("icons.pic=25");
    myNextion.sendCommand("icon_timer.pic=10");
    myNextion.sendCommand("temp.pic=17");
    myNextion.sendCommand("humdt.pic=20");

    /*
        comprehend when to change the button power on the display
    */

    if (buttonState == false) {

      myNextion.sendCommand("power.pic=7");
      myNextion.sendCommand("power.pic2=28");

    }

    myNextion.sendCommand("timerView.pic=14");
    myNextion.sendCommand("statusSensor.pic=23");

  } else if (temp <= 5) {

    /*
        set "page0" image to winter
    */

    myNextion.sendCommand("page0.pic=2");
    myNextion.sendCommand("icons.pic=26");
    myNextion.sendCommand("icon_timer.pic=11");
    myNextion.sendCommand("temp.pic=18");
    myNextion.sendCommand("humdt.pic=21");

    if (buttonState == false) {

      myNextion.sendCommand("power.pic=8");
      myNextion.sendCommand("power.pic2=29");

    }

    myNextion.sendCommand("timerView.pic=15");
    myNextion.sendCommand("statusSensor.pic=24");

  } else {

    /*
        set "page0" image to raindrops
    */

    myNextion.sendCommand("page0.pic=0");
    myNextion.sendCommand("icons.pic=12");
    myNextion.sendCommand("icon_timer.pic=9");
    myNextion.sendCommand("temp.pic=16");
    myNextion.sendCommand("humdt.pic=19");

    if (buttonState == false) {

      myNextion.sendCommand("power.pic=6");
      myNextion.sendCommand("power.pic2=27");

    }

    myNextion.sendCommand("timerView.pic=13");
    myNextion.sendCommand("statusSensor.pic=22");

  }

  return loop();

}

/*
    Set image of power button for different themes
*/

void button_Theme_On() {

  /*
      If the temperature is higher than or equal to 23°C
      and if the humidity is lesser than or equal to 65°C
      change theme to Sun
  */

  if (temp >= 23 && buttonState == true && humdt <= 65) {

    myNextion.sendCommand("power.pic=4");
    myNextion.sendCommand("power.pic2=31");

  } else if (temp <= 5 && buttonState == true) {

    myNextion.sendCommand("power.pic=5");
    myNextion.sendCommand("power.pic2=32");

  } else {

    myNextion.sendCommand("power.pic=3");
    myNextion.sendCommand("power.pic2=30");

  }

}

/*
    examine which background image to use for the button power in according to the setted theme
*/

void button_Theme_Off() {

  if (temp >= 23 && buttonState == false && humdt <= 65) {

    myNextion.sendCommand("power.pic=7");
    myNextion.sendCommand("power.pic2=28");

  } else if (temp <= 5 && buttonState == false) {

    myNextion.sendCommand("power.pic=8");
    myNextion.sendCommand("power.pic2=29");

  } else {

    myNextion.sendCommand("power.pic=6");
    myNextion.sendCommand("power.pic2=27");

  }

}

/*
    I start the relay, so I turn on the heat
*/

void buttonOn() {

  digitalWrite(5, LOW);
  buttonState = true;
  // buzzer sound one = on
  digitalWrite(6, HIGH);
  delay(100);
  digitalWrite(6, LOW);

}

/*
    I disable the relay, so I turn off the heat
*/

void buttonOff() {

  digitalWrite(5, HIGH);
  buttonState = false;

  /*
      set the font color to RED for 6 seconds
      and see the timer of the powered relay on the display
  */

  myNextion.sendCommand("timerView.pco=RED");
  // see if it is necessary to change with String (timerPlus – 1)
  myNextion.setComponentText("timerView", String(timerPlus) + String("s"));
  delay(6000);
  myNextion.sendCommand("timerView.pco=WHITE");
  myNextion.setComponentText("timerView", String("Timer"));
  timerPlus = 0;
  timerMinus = 0;
  // it sounds twice = off
  digitalWrite(6, HIGH);
  delay(100);
  digitalWrite(6, LOW);
  delay(100);
  digitalWrite(6, HIGH);
  delay(100);
  digitalWrite(6, LOW);

}

void setup() {

  Serial.begin(9600);
  myNextion.init(); // send the initialising commands for Page 0

  pinMode(5, OUTPUT); // relay temperature pin
  pinMode(6, OUTPUT); // buzzer pin
  pinMode(7, OUTPUT); // bright pin button for display Nextion

  digitalWrite(5, HIGH); // relay pin - read power on because the jumper is setted to VCC / VCC

}

void loop() {

  String message = myNextion.listen(); //check for message

  if (message != "") { // if a message is received...

    Serial.println(message); //...print it out

  }

  // read temperature and humidity sensor
  chk = DHT.read(DHT11_PIN);

  switch (chk) {

    case DHTLIB_OK:
      temp = DHT.temperature;
      humdt = DHT.humidity;
      myNextion.setComponentText("statusSensor", "Read");
      myNextion.setComponentText("temp", String(temp));
      myNextion.setComponentText("humdt", String(humdt));
      break;
    case DHTLIB_ERROR_CHECKSUM:
      myNextion.setComponentText("statusSensor", "Checksum error");
      myNextion.setComponentText("temp", String("--"));
      myNextion.setComponentText("humdt", String("--"));
      break;
    case DHTLIB_ERROR_TIMEOUT:
      myNextion.setComponentText("statusSensor", "Time out error");
      myNextion.setComponentText("temp", String("--"));
      myNextion.setComponentText("humdt", String("--"));
      break;
    default:
      myNextion.setComponentText("statusSensor", "Unknown error");
      myNextion.setComponentText("temp", String("--"));
      myNextion.setComponentText("humdt", String("--"));
      break;

  }

  if (message == "65 0 7 1 ffff ffff ffff" && buttonState == false) { // click power button on nextion

    button_Theme_On();
    buttonOn();
    // changing the status button, check theme
    theme();

  } else if (message == "65 0 7 1 ffff ffff ffff" && buttonState == true) { // click power button on nextion

    // check button on
    button_Theme_Off();
    buttonOff();
    // changing the status button, check theme
    theme();

  }

  /*
      send from the Nextion to the serial the power relay with timer selected
      from the page1 to display:
      30min | 45min | 1hr | 2hr

      send the id value (ex.: 65 1 1 0 ffff ffff ffff) from nextion to serial and read
  */

  if (message == "65 1 1 0 ffff ffff ffff" && buttonState == true || message == "65 1 1 0 ffff ffff ffff" && buttonState == false) { // 30min

    button_Theme_On();
    buttonOn();
    theme();
    timerMinus = 1801 - timerPlus;

  } else if ( message == "65 1 2 0 ffff ffff ffff" && buttonState == true || message == "65 1 2 0 ffff ffff ffff" && buttonState == false) { // 45min

    button_Theme_On();
    buttonOn();
    theme();
    timerMinus = 2701 - timerPlus;

  } else if (message == "65 1 3 0 ffff ffff ffff" && buttonState == true || message == "65 1 3 0 ffff ffff ffff" && buttonState == false) { // 1hr

    button_Theme_On();
    buttonOn();
    theme();
    timerMinus = 3601 - timerPlus;

  } else if (message == "65 1 4 0 ffff ffff ffff" && buttonState == true || message == "65 1 4 0 ffff ffff ffff" && buttonState == false) { // 2hr

    button_Theme_On();
    buttonOn();
    theme();
    timerMinus = 7201 - timerPlus;

  }

  // check timer for power relay
  if (digitalRead(5) == LOW) { // if relay is off

    timerPlus++;
    timerMinus--;

    if (timerMinus < 0) { // if timer is to the minimum, you see timerPlus on the display

      myNextion.setComponentText("timerView", String(timerPlus) + String("s"));

    } else {

      myNextion.setComponentText("timerView", String(timerMinus) + String("s"));

    }

  }

  /*
      if timer is to 0 turn off the relay;
      choose the maximum temperature to turn off the relay, on this case 24°C
  */

  if (timerMinus == 0 && digitalRead(5) == LOW || temp >= 24 && buttonState == true) {

    button_Theme_Off();
    buttonOff();
    theme();

  }

  // check status button on the display
  if (digitalRead(7) == HIGH && bright == true) {

    bright = false;
    myNextion.sendCommand("dims=0");

  } else if (digitalRead(7) == HIGH && bright == false) {

    bright = true;
    myNextion.sendCommand("dims=100");

  }

  /*
      I create a timer to verify the theme every 255 seconds (4,25 min)
      for no refresh continuously on the display
  */

  checkTheme++; // I increase for 4,25 min

  if (checkTheme == 255) {

    checkTheme = 0;
    return theme();

  }

  delay(1000);

}
