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
	this->inizializzaGSM();
}

void mandjGSM::inizializzaGSM() {
	if (settings.gsm) {
		if (gsm.begin(2400)) {
			Serial.println("\nGSM status=READY");
			this->startedGSM = true;
		} else {
			Serial.println("\nGSM status=IDLE");
			this->startedGSM = false;
		}
	}
}

void mandjGSM::leggiSMS() {
	if (settings.gsm) {
		this->position = sms.IsSMSPresent(SMS_UNREAD);
		if (this->position > 0) {
			// read new SMS
			sms.GetSMS(this->position, this->phone_number, this->sms_text, 160);

			byte telAutorizzato = 0;

			char xx[20] = "+39";
			strcat(xx, settings.phoneNumber1);
			if (strcmp(this->phone_number, xx) == 0)
				telAutorizzato = 1;

			memtozero_v(xx);
			strcpy(xx, "+39");
			strcat(xx, settings.phoneNumber2);
			if (strcmp(this->phone_number, xx) == 0)
				telAutorizzato = 2;

			memtozero_v(xx);
			strcpy(xx, "+39");
			strcat(xx, settings.phoneNumber3);
			if (strcmp(this->phone_number, xx) == 0)
				telAutorizzato = 3;

			memtozero_v(xx);
			strcpy(xx, "+39");
			strcat(xx, settings.phoneNumber4);
			if (strcmp(this->phone_number, xx) == 0)
				telAutorizzato = 4;

			memtozero_v(xx);
			strcpy(xx, "+39");
			strcat(xx, settings.phoneNumber5);
			if (strcmp(this->phone_number, xx) == 0)
				telAutorizzato = 5;

			if (telAutorizzato > 0) {
				this->returnMSG = this->decodificaComandiSMS();
			}
			sms.DeleteSMS(this->position);

#if MJDEBUG==1
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
	}
}

byte mandjGSM::decodificaComandiSMS() {
	if (!strcmp(this->sms_text, "ATTIVA"))
		return 4;
	else if (!strcmp(this->sms_text, "DISATTIVA"))
		return 5;
	else if (!strcmp(this->sms_text, "DISSENTEMP"))
		return 6;
	else if (!strcmp(this->sms_text, "STATUS"))
		return 7;
	else return 9;
}

void mandjGSM::inviaSMScomando(char *number_str, char *message_str) {
	wdt_disable();
	if (sms.SendSMS(number_str, this->sms_text))
		this->returnMSG = 2;
	else
		this->returnMSG = 3;
	wdt_enable(WDTO_8S);
}

void mandjGSM::chooseAct(String act) {
	char x = act.charAt(0);
	String msg = "";
	int startLen=2;

	switch (x) {
	case '1':
		if (settings.gsm)
		{
			if (gsm.getStatus() == 2)
				this->returnMSG = 1;
			else this->returnMSG = 9;
		}

#if MJDEBUG==1
		Serial.print("Caso 1 ");
		Serial.println(act);         // print the integer
#endif
		break;

	case '2':
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

		if (settings.gsm) {
			if (this->startedGSM) {
				inviaSMScomando(this->phone_number, this->sms_text);
			}
		}

#if MJDEBUG==1
		else
			Serial.println("GMS non attivo.");
#endif
		break;

	case '3':
		msg = act.substring(act.lastIndexOf("|") + 1, act.length());
		msg.toCharArray(sms_text, 160);
		if (settings.gsm) {
			if (this->startedGSM) {
				if (strcmp(settings.phoneNumber1, "0000000000") != 0)
					inviaSMScomando(settings.phoneNumber1, sms_text);
				if (strcmp(settings.phoneNumber2, "0000000000") != 0)
					inviaSMScomando(settings.phoneNumber2, sms_text);
				if (strcmp(settings.phoneNumber3, "0000000000") != 0)
					inviaSMScomando(settings.phoneNumber3, sms_text);
				if (strcmp(settings.phoneNumber4, "0000000000") != 0)
					inviaSMScomando(settings.phoneNumber4, sms_text);
				if (strcmp(settings.phoneNumber5, "0000000000") != 0)
					inviaSMScomando(settings.phoneNumber5, sms_text);
			}
		}
		break;

	case '4':
		msg = act.substring(startLen, act.indexOf(",",startLen) );
		settings.gsm=msg.toInt();

		startLen += msg.length()+1;
		msg = act.substring(startLen, act.indexOf(",",startLen) );
		msg.toCharArray(settings.phoneNumber1, sizeof(settings.phoneNumber1));

		startLen += msg.length()+1;
		msg = act.substring(startLen, act.indexOf(",",startLen) );
		msg.toCharArray(settings.phoneNumber2, sizeof(settings.phoneNumber2));

		startLen += msg.length()+1;
		msg = act.substring(startLen, act.indexOf(",",startLen) );
		msg.toCharArray(settings.phoneNumber3, sizeof(settings.phoneNumber3));

		startLen += msg.length()+1;
		msg = act.substring(startLen, act.indexOf(",",startLen) );
		msg.toCharArray(settings.phoneNumber4, sizeof(settings.phoneNumber4));

		startLen += msg.length()+1;
		msg = act.substring(startLen, act.indexOf(",",startLen) );
		msg.toCharArray(settings.phoneNumber5, sizeof(settings.phoneNumber5));
		Serial.println(settings.gsm);
		Serial.println(settings.phoneNumber1);
		Serial.println(settings.phoneNumber2);
		Serial.println(settings.phoneNumber3);
		Serial.println(settings.phoneNumber4);
		Serial.println(settings.phoneNumber5);

		this->saveSettings();
		if (settings.gsm==1)
		{
			wdt_disable();
			this->inizializzaGSM();
			wdt_enable(WDTO_8S);
		}
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
	Serial.println("Salvo EPROM");
	byte* p = (byte*) &settings;
	for (int i = 0; i < sizeof(GSMSettings); i++)
		EEPROM.write(i, p[i]);
}

void mandjGSM::loadSettings(void) {
	byte* p = (byte*) &settings;
	for (int i = 0; i < sizeof(GSMSettings); i++)
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
 * 5:
 * 6:
 * 7:
 * 8:
 * 9: gsm NOREADY
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

#if MJDEBUG==1
	Serial.print("GSM:");
	Serial.println(settings.gsm);
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
