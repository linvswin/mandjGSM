/*
 * mandjGSM.h
 *
 *  Created on: 03 ott 2016
 *      Author: peppe
 */

#ifndef MANDJGSM_H_
#define MANDJGSM_H_

#include "Arduino.h"
#include <Wire.h>
#include <avr/wdt.h>

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"

#define GSM_ATTIVO   1
#define MJDEBUG      1
#define I2CADDR   0x08

//char phone_number[40]; // array for the phone number string
//char sms_text[160];

class mandjGSM {
private:
	byte returnMSG;
	boolean startedGSM;
	int position;

	char phone_number[40]; // array for the phone number string
	char sms_text[160];

	SMSGSM sms;
public:
	mandjGSM();
	virtual ~mandjGSM();

	void inizializza();
	void chooseAct(String act);
	void leggiSMS();

	byte getReturnMSG();
	void setReturnMSG(byte in);
};

#endif /* MANDJGSM_H_ */
