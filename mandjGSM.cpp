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
	this->position=0;
}

mandjGSM::~mandjGSM() {
	// TODO Auto-generated destructor stub
}

void mandjGSM::inizializza() {

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
}

void mandjGSM::leggiSMS() {
#if GSM_ATTIVO==1
	this->position = sms.IsSMSPresent(SMS_UNREAD);
	if (this->position > 0) {
		// read new SMS
		sms.GetSMS(this->position, phone_number, sms_text, 160);

		int telAutorizzato = 0;
		char xx[20] = "+39";
/*		strcat(xx, settings.phoneNumber1);

		if (strcmp(phone_number, settings.phoneNumber1) != 0)
			telAutorizzato = 1;
		else if (strcmp(phone_number, settings.phoneNumber2) != 0)
			telAutorizzato = 2;
		else if (strcmp(phone_number, settings.phoneNumber3) != 0)
			telAutorizzato = 3;
		else if (strcmp(phone_number, settings.phoneNumber4) != 0)
			telAutorizzato = 4;
		else if (strcmp(phone_number, settings.phoneNumber5) != 0)
			telAutorizzato = 5;
*/
#ifdef MJDEBUGMM
		Serial.print("Num tel: ");
		Serial.println(phone_number);
		Serial.print("Text: ");
		Serial.println(sms_text);
		Serial.print("Auth Tel: ");
		Serial.println(xx);
		Serial.print("Auth: ");
		Serial.println(telAutorizzato);
#endif
		if (telAutorizzato > 0) {
			this->returnMSG=4;
			//this->eseguiSMSComando(sms_text);
			sms.DeleteSMS(this->position);
		}
	}
#endif
}

void mandjGSM::chooseAct(String act) {
	char x = act.charAt(0);

	String num = "";
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
		if (this->startedGSM) {
			wdt_disable();
			if (sms.SendSMS(phone_number, sms_text))
				this->returnMSG = 2;
			else
				this->returnMSG = 3;
			wdt_enable(WDTO_8S);
		}
#if DEBUG==1
		else
			Serial.println("GMS non attivo.");
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

byte mandjGSM::getReturnMSG()
{
	return this->returnMSG;
}

void mandjGSM::setReturnMSG(byte in)
{
	this->returnMSG=in;
}
