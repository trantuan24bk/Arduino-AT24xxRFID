#include <Wire.h>
#include "AT24xxRFID.h"

AT24xxRFID eeprom;

void setup() 
{
	char status;
	uint16_t cardIndex, num;
	char message[40];
	char entry[10], pincode[10], misc[10];
	char rfidtag[25], cardname[25];

	Serial.begin(9600);
	while(!Serial);

	Serial.println("Starting EEPROM...");
	eeprom.begin(0x50);

	//=================================================================
	//================ Clear cards & logs==============================
	//=================================================================

	Serial.println("-----------------------------\n");
	Serial.println("Clear cards & logs to zero!\n");

	eeprom.numOfCards(0);
	eeprom.numOfLog(0);

	//=================================================================
	//================ Add new cards ==================================
	//=================================================================

	Serial.println("-----------------------------\n");
	Serial.println("Add new cards...");

	// #1
	if(eeprom.addCard(&cardIndex, "A101", "123456789ABC", "Benny Andersson", "1000", "2345") == STT_ERROR)
	{
		sprintf((char*)message, "Error: Add new card #%d error!", cardIndex);
		Serial.println((char*)message);
	}
	else
	{
		sprintf((char*)message, "Add card #%d success!", cardIndex);
		Serial.println((char*)message);
	}	

	// #2
	if(eeprom.addCard(&cardIndex, "B102", "123456789DEF", "Barack Obama", "1001", "2346") == STT_ERROR)
	{
		sprintf((char*)message, "Error: Add new card #%d error!", cardIndex);
		Serial.println((char*)message);
	}
	else
	{
		sprintf((char*)message, "Add card #%d success!", cardIndex);
		Serial.println((char*)message);
	}

	// #3
	if(eeprom.addCard(&cardIndex, "C103", "123456ABC789", "George W.Bush", "1002", "2347") == STT_ERROR)
	{
		sprintf((char*)message, "Error: Add new card #%d error!", cardIndex);
		Serial.println((char*)message);
	}
	else
	{
		sprintf((char*)message, "Add card #%d success!", cardIndex);
		Serial.println((char*)message);
	}

	// Total number of cards
	Serial.print("Total cards: ");
	Serial.println(eeprom.numOfCards());

	//=================================================================
	//============= Read info from EEPROM =============================
	//=================================================================

	Serial.println("-----------------------------\n");
	Serial.println("Read info from EEPROM. \n");

	cardIndex = 0;
	eeprom.getCard(cardIndex, (char*)entry, (char*)rfidtag, (char*)cardname, (char*)pincode, (char*)misc);

	Serial.print("#");
	Serial.print(cardIndex);
	Serial.print(": ");
	Serial.print((char*)entry);		Serial.print(" - ");
	Serial.print((char*)rfidtag);	Serial.print(" - ");
	Serial.print((char*)cardname);	Serial.print(" - ");
	Serial.print((char*)pincode);	Serial.print(" - ");
	Serial.println((char*)misc);

	cardIndex = 1;
	eeprom.getCard(cardIndex, (char*)entry, (char*)rfidtag, (char*)cardname, (char*)pincode, (char*)misc);

	Serial.print("#");
	Serial.print(cardIndex);
	Serial.print(": ");
	Serial.print((char*)entry);		Serial.print(" - ");
	Serial.print((char*)rfidtag);	Serial.print(" - ");
	Serial.print((char*)cardname);	Serial.print(" - ");
	Serial.print((char*)pincode);	Serial.print(" - ");
	Serial.println((char*)misc);

	cardIndex = 2;
	eeprom.getCard(cardIndex, (char*)entry, (char*)rfidtag, (char*)cardname, (char*)pincode, (char*)misc);

	Serial.print("#");
	Serial.print(cardIndex);
	Serial.print(": ");
	Serial.print((char*)entry);		Serial.print(" - ");
	Serial.print((char*)rfidtag);	Serial.print(" - ");
	Serial.print((char*)cardname);	Serial.print(" - ");
	Serial.print((char*)pincode);	Serial.print(" - ");
	Serial.println((char*)misc);


	Serial.println("-----------------------------\n");
	Serial.println("Delete #1 card. ");

	if(eeprom.deleteCard(1) != STT_SUCCESS)
	{
		Serial.println("Delete card failed!");
	}

	// Total number of cards
	Serial.print("Total cards: ");
	Serial.println(eeprom.numOfCards());

	//=================================================================
	//=============Find card by tag ===================================
	//=================================================================

	Serial.println("-----------------------------\n");
	Serial.println("Find card by tag: 123456789DEF");

	if(eeprom.findCardByTag(&cardIndex, "123456789DEF") == STT_FOUND)
	{
		Serial.print("Found: #");
		Serial.println(cardIndex);

		eeprom.getCard(cardIndex, (char*)entry, (char*)rfidtag, (char*)cardname, (char*)pincode, (char*)misc);

		Serial.print("#");
		Serial.print(cardIndex);
		Serial.print(": ");
		Serial.print((char*)entry);		Serial.print(" - ");
		Serial.print((char*)rfidtag);	Serial.print(" - ");
		Serial.print((char*)cardname);	Serial.print(" - ");
		Serial.print((char*)pincode);	Serial.print(" - ");
		Serial.println((char*)misc);
	}
	else
	{
		Serial.println("RFID tag not found!");
	}

	Serial.println("-----------------------------\n");
	Serial.println("Find card by tag: 123456ABC789");

	if(eeprom.findCardByTag(&cardIndex, "123456ABC789") == STT_FOUND)
	{
		Serial.print("Found: #");
		Serial.println(cardIndex);

		eeprom.getCard(cardIndex, (char*)entry, (char*)rfidtag, (char*)cardname, (char*)pincode, (char*)misc);

		Serial.print("#");
		Serial.print(cardIndex);
		Serial.print(": ");
		Serial.print((char*)entry);		Serial.print(" - ");
		Serial.print((char*)rfidtag);	Serial.print(" - ");
		Serial.print((char*)cardname);	Serial.print(" - ");
		Serial.print((char*)pincode);	Serial.print(" - ");
		Serial.println((char*)misc);
	}
	else
	{
		Serial.println("RFID tag not found!");
	}

	Serial.println("-----------------------------\n");

	//=================================================================
	//============= Read/write logs ===================================
	//=================================================================
	uint16_t logIndex;
	uint16_t numWord;
	uint8_t numByte;

	Serial.println("Write log...");

	// Log #0
	if(eeprom.logWrite(&logIndex, "A101", "123456789ABC", "01012015-13:30", 9876, 215) == STT_ERROR)
	{
		sprintf((char*)message, "Error: Add new log #%d error!", logIndex);
		Serial.println((char*)message);
	}
	else
	{
		sprintf((char*)message, "Add log #%d success!", logIndex);
		Serial.println((char*)message);
	}

	// Log #1
	if(eeprom.logWrite(&logIndex, "B102", "123456789DEF", "01012015-14:30", 9877, 216) == STT_ERROR)
	{
		sprintf((char*)message, "Error: Add new log #%d error!", logIndex);
		Serial.println((char*)message);
	}
	else
	{
		sprintf((char*)message, "Add log #%d success!", logIndex);
		Serial.println((char*)message);
	}

	// Log #2
	if(eeprom.logWrite(&logIndex, "C103", "123456ABC789", "01012015-15:30", 9878, 217) == STT_ERROR)
	{
		sprintf((char*)message, "Error: Add new log #%d error!", logIndex);
		Serial.println((char*)message);
	}
	else
	{
		sprintf((char*)message, "Add log #%d success!", logIndex);
		Serial.println((char*)message);
	}

	// Total
	Serial.print("Total logs: ");
	Serial.println(eeprom.numOfLog());

	Serial.println("-----------------------------\n");
	Serial.println("Read log...");

	// Log #0
	logIndex = 0;
	eeprom.logRead(logIndex, (char*)entry, (char*)rfidtag, (char*)cardname, &numWord, &numByte);

	Serial.print("#");
	Serial.print(logIndex);
	Serial.print(": ");
	Serial.print((char*)entry);		Serial.print(" - ");
	Serial.print((char*)rfidtag);	Serial.print(" - ");
	Serial.print((char*)cardname);	Serial.print(" - ");
	Serial.print(numWord);			Serial.print(" - ");
	Serial.println(numByte);

	// Log #1
	logIndex = 1;
	eeprom.logRead(logIndex, (char*)entry, (char*)rfidtag, (char*)cardname, &numWord, &numByte);

	Serial.print("#");
	Serial.print(logIndex);
	Serial.print(": ");
	Serial.print((char*)entry);		Serial.print(" - ");
	Serial.print((char*)rfidtag);	Serial.print(" - ");
	Serial.print((char*)cardname);	Serial.print(" - ");
	Serial.print(numWord);			Serial.print(" - ");
	Serial.println(numByte);

	// Log #2
	logIndex = 2;
	eeprom.logRead(logIndex, (char*)entry, (char*)rfidtag, (char*)cardname, &numWord, &numByte);

	Serial.print("#");
	Serial.print(logIndex);
	Serial.print(": ");
	Serial.print((char*)entry);		Serial.print(" - ");
	Serial.print((char*)rfidtag);	Serial.print(" - ");
	Serial.print((char*)cardname);	Serial.print(" - ");
	Serial.print(numWord);			Serial.print(" - ");
	Serial.println(numByte);

}

void loop() 
{

}
