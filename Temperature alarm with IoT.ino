//Libraries
#include "ThingSpeak.h"
#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
#define DHT22_PIN 8
dht DHT;
//Variables
bool sent = false; //flag variable to insure that the mail was succefully sent
int number = 0; //in case of errors updating IoT channel

// the server to send mail
char server[] = "mail.smtp2go.com";




byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// change network settings to yours, comment them all to use DHCP
// the dns server ip
IPAddress dnServer(**, **, **, **);
// the router's gateway address:
IPAddress gateway(**, **, **, **);
// the subnet:
IPAddress subnet(**, **, **, **);
//the IP address is dependent on your network
IPAddress ip(**, **, **, **);


EthernetClient client;

void setup()
{ Serial.begin(9600);
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("Temp:  Humidity:");

  // Declare the following pins as an output
  pinMode (A0, OUTPUT);
  pinMode (A1, OUTPUT);
  pinMode (A2, OUTPUT);
  pinMode (A3, OUTPUT);
  pinMode (A4, OUTPUT);
  pinMode (A5, OUTPUT);
  //For DHCP, set only the mac address 
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);
  ThingSpeak.begin(client);

}








void loop() {
  while (true)
  { //writing channel id and api write  key
    int x = ThingSpeak.writeFields(***, "***************");

    delay(15000); // Wait 15 seconds to update the channel again
    //receiving data from thingspeak channel and show the feedback on pin
    //channel id , field, readapikey
    int i1 = ThingSpeak.readIntField(***, ***, "*******");
    int i1 = ThingSpeak.readIntField(***, ***, "*******");
    int i1 = ThingSpeak.readIntField(***, ***, "*******");
    int i1 = ThingSpeak.readIntField(***, ***, "*******");
    int i1 = ThingSpeak.readIntField(***, ***, "*******");
    int i1 = ThingSpeak.readIntField(***, ***, "*******");
    //declaring a feedback
    int currentStatus1, currentStatus2, currentStatus3, currentStatus4, currentStatus5, currentStatus6 ;
    //receiving from app to control the pins
    if (i1 == 1)
    { digitalWrite(A0, HIGH);
      currentStatus1 = 1;
    }
    else   {
      digitalWrite(A0, LOW);
      currentStatus1 = 0;
    }
    if (i2 == 1)
    { digitalWrite(A1, HIGH);
      currentStatus2 = 1;
    }
    else   {
      digitalWrite(A1, LOW);
      currentStatus2 = 0;
    }
    if (i3 == 1)
    { digitalWrite(A2, HIGH);
      currentStatus3 = 1;
    }
    else   {
      digitalWrite(A2, LOW);
      currentStatus3 = 0;
    }
    if (i4 == 1)
    { digitalWrite(A3, HIGH);
      currentStatus4 = 1;
    }
    else   {
      digitalWrite(A3, LOW);
      currentStatus4 = 0;
    }
    if (i5 == 1)
    { digitalWrite(A4, HIGH);
      currentStatus5 = 1;
    }
    else   {
      digitalWrite(A4, LOW);
      currentStatus5 = 0;
    }
    if (i6 == 1)
    { digitalWrite(A5, HIGH);
      currentStatus6 = 1;
    }
    else   {
      digitalWrite(A5, LOW);
      currentStatus6 = 0;
    }





    // set the cursor to column 0, line (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);

    int chk = DHT.read22(DHT22_PIN);
    //Read data and store it to variables hum and temp

    // read humidity
    float hum = DHT.humidity;

    //read temperature in Celisius
    float temp = DHT.temperature;

    //Print on LCD the values
    lcd.print(temp);
    lcd.setCursor(7, 1);
    lcd.print(hum);

    //Print on serial the values
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");

    //sending feedback to IoT server and the reads of DHT22
    ThingSpeak.setField(1, temp);
    ThingSpeak.setField(2, hum);
    ThingSpeak.setField(3, currentStatus1);
    ThingSpeak.setField(4, currentStatus2);
    ThingSpeak.setField(5, currentStatus3);
    ThingSpeak.setField(6, currentStatus4);
    ThingSpeak.setField(7, currentStatus5);
    ThingSpeak.setField(8, currentStatus6);
    //checking conncetion
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

    number++;
    if (number > 99) {
      number = 0;
    }

    // The reset condition
    if (temp < 27)
    {
      sent = false;
    }



    //The condition of the critical Temperature or humidity

    if (temp >= 30)
    { //flag
      if (sent == true) continue;
      //send
      sendEmail();
      sent = true;
    }



  }


}



byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;

  while (!client.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  respCode = client.peek();

  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  if (respCode >= '4')
  {
    efail();
    return 0;
  }

  return 1;
}


void efail()
{
  byte thisByte = 0;
  int loopCount = 0;

  client.println(F("QUIT"));

  while (!client.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }

  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  client.stop();

  Serial.println(F("disconnected"));
}



byte sendEmail()
{
  byte thisByte = 0;
  byte respCode;
  { //connecting
    client.connect(server, 25);
    //checking connection
    if (client.connected()) {
      Serial.println("connected");
    } else {
      Serial.println(F("connection failed"));
      return 0;
    }

    if (!eRcv()) return 0;

    Serial.println(F("Sending hello"));
    // replace 1.2.3.4 with your Arduino's ip
    client.println("EHLO 10.10.30.224");

    if (!eRcv()) return 0;

    Serial.println(F("Sending auth login"));
    client.println("auth login");
    if (!eRcv()) return 0;

    Serial.println(F("Sending User"));
    // Change to your base64 encoded user
    client.println(F("**********"));

    if (!eRcv()) return 0;

    Serial.println(F("Sending Password"));
    // change to your base64 encoded password
    client.println(F("*********"));

    if (!eRcv()) return 0;
    // change to your email address (sender)
    Serial.println(F("Sending From"));
    client.println("MAIL From: <username@domain.com>");
    if (!eRcv()) return 0;

    // change to recipient address
    Serial.println(F("Sending To"));
    client.println("RCPT To: <username@domain.com>");
    if (!eRcv()) return 0;

    Serial.println(F("Sending To"));
    client.println("RCPT To: <username@domain.com>");
    if (!eRcv()) return 0;

    
  */
    Serial.println(F("Sending DATA"));
    client.println("DATA");
    if (!eRcv()) return 0;

    Serial.println(F("Sending email"));

    // change to recipient address
    client.println("To: ***** <username@domain.com>");
    client.println("To: ***** <username@domain.com>");
    // change to your address
    client.println("From: ***** <username@domain.com>");

    client.println("Subject: Warning temperature ");
    client.println("Warning : Temperature exceeded **C");

    client.println("Current Temperature :");
    client.println(DHT.temperature);
    client.println("and current humidity :");
    client.println(DHT.humidity);
    client.println(".");

    if (!eRcv()) return 0;

    Serial.println(F("Sending QUIT"));
    client.println("QUIT");
    if (!eRcv()) return 0;

    client.stop();

    Serial.println(F("disconnected"));

    return 1;

  }
}
