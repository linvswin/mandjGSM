/*
 * mandjGSM.cpp
 *
 *  Created on: 03 ott 2016
 *      Author: peppe
 */

#include "mandjGSM.h"

mandjGSM::mandjGSM() {
	// TODO Auto-generated constructor stub
	this->returnMSG = 0;
	this->startedGSM = false;
	this->position = 0;
}

mandjGSM::~mandjGSM() {
	// TODO Auto-generated destructor stub
}

void mandjGSM::inizializza() {

	//this->saveSettings();
	this->loadSettings();
}

void mandjGSM::inizializzaGSM() {
	if (settings.gsm) {
		if (gsm.begin(2400)) {
			Serial.println("\nGSM status=READY");
			startedGSM = true;
		} else {
			Serial.println("\nGSM status=IDLE");
			startedGSM = false;
		}
	}
}

void mandjGSM::leggiSMS() {
#if GSM_ATTIVO==1
	this->position = sms.IsSMSPresent(SMS_UNREAD);
	if (this->position > 0) {
		// read new SMS
		sms.GetSMS(this->position, this->phone_number, this->sms_text, 160);

		int telAutorizzato = 0;
		char xx[20] = "+39";
		strcat(xx, settings.phoneNumber1);

		if (strcmp(this->phone_number, settings.phoneNumber1) != 0)
			telAutorizzato = 1;
		else if (strcmp(this->phone_number, settings.phoneNumber2) != 0)
			telAutorizzato = 2;
		else if (strcmp(this->phone_number, settings.phoneNumber3) != 0)
			telAutorizzato = 3;
		else if (strcmp(this->phone_number, settings.phoneNumber4) != 0)
			telAutorizzato = 4;
		else if (strcmp(this->phone_number, settings.phoneNumber5) != 0)
			telAutorizzato = 5;

		if (telAutorizzato > 0) {
			this->returnMSG = this->decodificaComandi();
		}
		sms.DeleteSMS(this->position);

#ifdef MJDEBUG
		Serial.print("Num tel: ");
		Serial.println(this->phone_number);
		Serial.print("Text: ");
		Serial.println(this->sms_text);
		Serial.print("Auth Tel: ");
		Serial.println(xx);
		Serial.print("Auth: ");
		Serial.println(telAutorizzato);
#endif

	}
#endif
}

byte mandjGSM::decodificaComandi() {
	if (!strcmp(this->sms_text, "ATTIVA"))
		return 4;
	else if (!strcmp(this->sms_text, "DISATTIVA"))
		return 5;
	else if (!strcmp(this->sms_text, "DISSENTEMP"))
		return 6;
	else if (!strcmp(this->sms_text, "STATUS"))
		return 7;
	//else return 9;
}

void mandjGSM::chooseAct(String act) {
	char x = act.charAt(0);

	//String num = "";
	String msg = "";

	switch (x) {
	case '1':
#if GSM_ATTIVO==1
		if (gsm.getStatus() == 2)
			this->returnMSG = 1;
#endif
		Serial.print("Caso 1 ");
		Serial.println(act);         // print the integer
		break;
	case '2':
		//num = act.substring(2, act.indexOf("|", 2));
		//if (num != "0")	num.toCharArray(phone_number, 40);
		msg = act.substring(act.lastIndexOf("|") + 1, act.length());
		msg.toCharArray(sms_text, 160);

#if MJDEBUG==1
		Serial.print("Caso 2: ");
		Serial.println(act);         // print the integer
		Serial.print("num: ");
		Serial.println(this->phone_number);
		Serial.print("msg: ");
		Serial.println(this->sms_text);
#endif
#if GSM_ATTIVO==1
		if (this->startedGSM) {
			wdt_disable();
			if (sms.SendSMS(this->phone_number, this->sms_text))
				this->returnMSG = 2;
			else
				this->returnMSG = 3;
			wdt_enable(WDTO_8S);
		}
#if MJDEBUG==1
		else
			Serial.println("GMS non attivo.");
#endif
#endif
		break;
	default:
#if MJDEBUG==1
		Serial.print("default: ");
		Serial.println(act);         // print the integer
#endif
		break;
	}
}

byte mandjGSM::getReturnMSG() {
	return this->returnMSG;
}

void mandjGSM::setReturnMSG(byte in) {
	this->returnMSG = in;
}

void mandjGSM::saveSettings(void) {
	byte* p = (byte*) &settings;
	for (int i = 1; i < sizeof(GSMSettings); i++)
		EEPROM.write(i, p[i]);
}

void mandjGSM::loadSettings(void) {
	byte* p = (byte*) &settings;
	for (int i = 1; i < sizeof(GSMSettings); i++)
		p[i] = EEPROM.read(i);
}

/** MAIN ***/
String inputString = "";
boolean stringComplete = false;

#if MJDEBUG==1
int maxExecute = 10000;
int previousT = 0;
int i = 0;
#endif

mandjGSM mjGSM;

void receiveEvent(int howMany) {
	char x = (char) Wire.read();    // receive byte as an integer
	if (x == '~') {
#if MJDEBUG==1
		Serial.println("s:" + inputString);
#endif
		stringComplete = true;
	} else
		inputString.concat(x);
}

/**
 * Function that executes whenever data is requested by master
 * this function is registered as an event, see setup()
 *
 * 0: nulla
 * 1: gsm READY
 * 2: sms inviato
 * 3: sms errore
 * 4: richiede lo stato dell'allarme
 *
 **/
void requestEvent() {
	Wire.write(mjGSM.getReturnMSG());
	mjGSM.setReturnMSG(0);
}

void setup() {
	Wire.begin(I2CADDR);              // join i2c bus with address #8
	Wire.onReceive(receiveEvent);     // register event
	Wire.onRequest(requestEvent);     // register event

	Serial.begin(9600);           // start serial for output
#if MJDEBUG==1
	Serial.println("Start SLAVE");
#endif

	mjGSM.inizializza();
	mjGSM.inizializzaGSM();

#ifdef MJDEBUG
	Serial.print("Tel1:");
	Serial.println(settings.phoneNumber1);
	Serial.print("Tel2:");
	Serial.println(settings.phoneNumber2);
	Serial.print("Tel3:");
	Serial.println(settings.phoneNumber3);
	Serial.print("Tel4:");
	Serial.println(settings.phoneNumber4);
	Serial.print("Tel5:");
	Serial.println(settings.phoneNumber5);
#endif

	// attivo watchdog 8s
	wdt_enable(WDTO_8S);
}

void loop() {

#if MJDEBUG==2
	int m = millis();
	if ((m - previousT) > maxExecute) {
		Serial.print(i++);
		Serial.print("Funzeca !!! - ");
		Serial.println(mjGSM.getReturnMSG());
		previousT = m;
	}
#endif

	if (stringComplete) {
		mjGSM.chooseAct(inputString);
		stringComplete = false;
		inputString = "";
	}
	mjGSM.leggiSMS();

	wdt_reset();
	delay(100);
}
