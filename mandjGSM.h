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
#include <EEPROM.h>

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"

//#define GSM_ATTIVO   1
#define MJDEBUG      1
#define I2CADDR   0x08

// azzera variabili
#define memtozero_s(var) var ^= var;
#define memtozero_v(var) memset((void*)&var,0,sizeof(var));

struct GSMSettings {
	uint8_t gsm;			// attiva gms
	char phoneNumber1[20]; // array for the phone number string
	char phoneNumber2[20]; // array for the phone number string
	char phoneNumber3[20]; // array for the phone number string
	char phoneNumber4[20]; // array for the phone number string
	char phoneNumber5[20]; // array for the phone number string
} settings = {
	0,				// gsm
	"0000000000",	// phoneNumber1
	"0000000000",	// phoneNumber2
	"0000000000",	// phoneNumber3
	"0000000000",	// phoneNumber4
	"0000000000"	// phoneNumber5;
};

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
	void inizializzaGSM();
	void chooseAct(String act);
	void leggiSMS();
	byte decodificaComandiSMS();
	void inviaSMScomando(char *number_str, char *message_str);

	byte getReturnMSG();
	void setReturnMSG(byte in);

	void saveSettings(void);
	void loadSettings(void);
};

#endif /* MANDJGSM_H_ */
