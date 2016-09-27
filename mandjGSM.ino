#include "Arduino.h"

#include <Wire.h>
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"

SMSGSM sms;
char phone_number[40]; // array for the phone number string
char sms_text[160];
boolean startedGSM = false;
String inputString = "";
byte returnMSG = 0;

String outputString = "";
boolean stringComplete = false;

#define I2CADDR   0x08
int maxExecute=10000;
int previousT=0;
int i=0;

void setup() {
	Serial.begin(9600);           // start serial for output
	Serial.println("Start SLAVE");

	if (gsm.begin(2400)) {
		Serial.println("\nGSM status=READY");
		//Serial.println(gsm.getStatus());
		startedGSM = true;
	} else {
		Serial.println("\nGSM status=IDLE");
		startedGSM = false;
		//if (settings.gsm) settings.gsm=0;
	}
	Wire.begin(I2CADDR);                // join i2c bus with address #8
	Wire.onReceive(receiveEvent); // register event
	//Wire.onRequest(requestEvent); // register event
}

void loop() {
	int m=millis();
	if ( (m-previousT)>maxExecute )
	{
		Serial.print(i++);
		Serial.println("Funzeca !!!");
		previousT=m;
	} //else previousT=m;
	delay(100);
}

void receiveEvent(int howMany) {
	//inputString = "";
	//returnMSG = 0;
	//char c="";
	/*while (1 < Wire.available()) {
		c = Wire.read();
		//Serial.print(c);
		inputString += c;
	}*/
	char x = Wire.read();    // receive byte as an integer
	//Serial.print(x);
	inputString += x;
	if (x == char(10))
	{
		Serial.println("s:"+inputString);
		chooseAct(inputString);
		//inputString="";
	}
	//Serial.println(howMany);
	//Serial.println();

	//chooseAct(inputString);
	//if (inputString.length() + 1 == howMany)
	//	returnMSG = 1;

	//Serial.print("s: ");
	//Serial.println(inputString);
}

void chooseAct(String act) {
	char x = act.charAt(0);

	String num = "";
	String msg = "";

	switch (x) {
	case '1':
		Serial.print("Caso 1 ");
		Serial.println(act);         // print the integer
		break;
	case '2':
		Serial.print("Caso 2: ");
		Serial.println(act);         // print the integer

		num = act.substring(2, act.indexOf("|", 2));
		num.toCharArray(phone_number, 40);
		msg = act.substring(act.lastIndexOf("|") + 1, act.length());
		msg.toCharArray(sms_text, 160);

		Serial.print("num: ");
		Serial.println(phone_number);
		Serial.print("msg: ");
		Serial.println(sms_text);

		if (startedGSM)
			sms.SendSMS(phone_number, sms_text);
		else
			Serial.println("GMS non attivo.");
		break;
	default:
		Serial.print("default: ");
		Serial.println(act);         // print the integer
		break;
	}
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
/*void requestEvent() {
	switch (returnMSG) {
	case 0:
		Wire.write("KO");
		break;
	case 1:
		Wire.write("OK");
		break;
	}
	// as expected by master
}*/

/*void serialEvent() {
	while (Serial.available()) {
		char inChar = Serial.read();
		outputString += inChar;
		if (inChar == '\n') {
			stringComplete = true;
		}
	}
}*/
