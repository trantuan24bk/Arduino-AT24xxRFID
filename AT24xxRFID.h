#ifndef _AT24XXRFID_EEPROM_H
#define _AT24XXRFID_EEPROM_H

#include <Arduino.h>

#define MAX_I2C_LEN 		(BUFFER_LENGTH-2)	// in bytes
#define MAX_BUFF_LEN 		100 				// in bytes
#define MAX_EEPROM_LEN 		32768				// AT24C256 (in bytes)

#define MAX_NUM_CARD 		160					// Maximum number of cards
#define MAX_NUM_LOG 		490					// Maximum number of log item

#define ADDR_VALID_SIGNAL 	0x0000u 			// Memory location in EEPROM
#define ADDR_NUMOF_CARD 	0x0001u
#define ADDR_FIRST_CARD 	0x0003u
#define ADDR_NUMOF_LOG	 	0x2000u
#define ADDR_FIRST_LOG	 	0x2002u

#define LEN_ENTRY_BYTE 		5 				// Entry
#define LEN_TAG_BYTE 		13 				// RFID Tag
#define LEN_NAME_TIME_BYTE 	16 				// Name
#define LEN_PINCODE_BYTE 	5 				// Pin code
#define LEN_MISC_BYTE 		5 				// Misc
#define LEN_NULL_BYTE 		1 				// (NULL)Space between each items

#define LEN_CARD_BYTE 		(LEN_ENTRY_BYTE + LEN_TAG_BYTE + LEN_NAME_TIME_BYTE + \
							 LEN_PINCODE_BYTE + LEN_MISC_BYTE)

enum status_name
{
	STT_ERROR = 0,
	STT_SUCCESS,

	STT_FOUND,
	STT_NOTFOUND,

	STT_MATCH,
	STT_NOTMATCH
};

class AT24xxRFID
{

protected:
	
private:
	
	byte ADDR_EEPROM; 				// EEPROM address

	char* pBuffer;
	char buffer[MAX_BUFF_LEN];
	
	// 	Read data from specify adrress, with length
	char readFromAddress(byte EEPROMAddr, uint16_t beginAddress, char* result, uint16_t len);
	char writeToAddress(byte EEPROMAddr, uint16_t beginAddress, char* data, uint16_t len);

public:
	AT24xxRFID();
	~AT24xxRFID();

	char begin(uint8_t address);

	// Add new card to rfid list
	char addCard(uint16_t* cardIndex, char* entry, char* rfidtag, char* cardname, char* pincode, char* misc);

	// Update card info by index
	char updateCard(uint16_t cardIndex, char* entry, char* rfidtag, char* cardname, char* pincode, char* misc);

	// Verify card info by index
	char verifyCard(uint16_t cardIndex, char* entry, char* rfidtag, char* cardname, char* pincode, char* misc);

	// Get card info by index
	char getCard(uint16_t cardIndex, char* entry, char* rfidtag, char* cardname, char* pincode, char* misc);

	// Get card index by rfidtag
	char findCardByTag(uint16_t* cardIndex, char* rfidtag);

	// Delete an entry by index
	char deleteCard(uint16_t cardIndex);

	// Write number of card
	void numOfCards(uint16_t num);

	// Read number of card
	uint16_t numOfCards(void);


	// Set number of log items 
	void numOfLog(uint16_t num);

	// Get number of log items
	uint16_t numOfLog(void);
	
	// Write to log. 
	// And its return current log index.
	char logWrite(uint16_t* logIndex, char* entry, char* rfidtag, char* datetime, uint16_t numWord, uint8_t numByte);

	// Read log by index
	char logRead(uint16_t logIndex, char* entry, char* rfidtag, char* datetime, uint16_t* numWord, uint8_t* numByte);
};


#endif /* _AT24XXRFID_EEPROM_H */