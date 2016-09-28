#include "Arduino.h"
#include <avr/wdt.h>

#include <Wire.h>

#define GSM_ATTIVO 1
#define DEBUG      0

#if GSM_ATTIVO==1

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"

SMSGSM sms;
boolean startedGSM = false;

#endif

char phone_number[40]; // array for the phone number string
char sms_text[160];
String inputString = "";
byte returnMSG = 0;

String outputString = "";
boolean stringComplete = false;

#define I2CADDR   0x08
int maxExecute = 10000;
int previousT = 0;
int i = 0;

void setup() {
	Wire.begin(I2CADDR);              // join i2c bus with address #8
	Wire.onReceive(receiveEvent);     // register event
	//Wire.onRequest(requestEvent);     // register event

	Serial.begin(9600);           // start serial for output
	Serial.println("Start SLAVE");

#if GSM_ATTIVO==1
	if (gsm.begin(2400)) {
		Serial.println("\nGSM status=READY");
		startedGSM = true;
	} else {
		Serial.println("\nGSM status=IDLE");
		startedGSM = false;
	}
#endif
	// attivo watchdog 8s
	wdt_enable(WDTO_8S);
}

void loop() {
#if DEBUG==1
	 int m = millis();
	 if ((m - previousT) > maxExecute) {
		 Serial.print(i++);
		 Serial.println("Funzeca !!!");
		 previousT = m;
	 }
#endif

	if (stringComplete) {
		chooseAct(inputString);
		stringComplete = false;
		inputString = "";
	}
	wdt_reset();
	delay(100);
}

void receiveEvent(int howMany) {
	/*while (1 < Wire.available()) {
	 byte c = Wire.read();
	 Serial.print(c);
	 inputString += c;
	 }*/
	char x = (char) Wire.read();    // receive byte as an integer
	if (x == '~') {
#if DEBUG==1
		Serial.println("s:" + inputString);
#endif
		stringComplete = true;
	} else inputString.concat(x);
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
		num = act.substring(2, act.indexOf("|", 2));
		num.toCharArray(phone_number, 40);
		msg = act.substring(act.lastIndexOf("|") + 1, act.length());
		msg.toCharArray(sms_text, 160);

#if DEBUG==1
		Serial.print("Caso 2: ");
		Serial.println(act);         // print the integer
		Serial.print("num: ");
		Serial.println(phone_number);
		Serial.print("msg: ");
		Serial.println(sms_text);
#endif
#if GSM_ATTIVO==1
		if (startedGSM){
			wdt_disable();
			sms.SendSMS(phone_number, sms_text);
			wdt_enable(WDTO_8S);
		}
	#if DEBUG==1
		else Serial.println("GMS non attivo.");
	#endif
#endif
		break;
	default:
#if DEBUG==1
		Serial.print("default: ");
		Serial.println(act);         // print the integer
#endif
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
