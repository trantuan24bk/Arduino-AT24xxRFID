#include <Arduino.h>
#include <Wire.h>
#include "AT24xxRFID.h"


/*===============================================================================================
  Name         :  
  Description  :  Constructor
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
AT24xxRFID::AT24xxRFID()
{	
	// Assign pointer to buffer
	pBuffer = (char*)buffer;
}

/*===============================================================================================
  Name         :  
  Description  :  Destructor
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
AT24xxRFID::~AT24xxRFID()
{
	delete[] buffer;
}

/*===============================================================================================
  Name         :  
  Description  :  Start I2C communication and validate data in EEPROM.
  Argument(s)  :  
  				- address : Enter your EEPROM address.
  Return value :  
				- STT_ERROR
				- STT_SUCCESS
================================================================================================*/
char AT24xxRFID::begin(uint8_t address)
{
	byte counter = 3;
	char* p = pBuffer;

	// Device address
	ADDR_EEPROM = address;

	// Init I2C bus
	Wire.begin();
    delay(10);

    while(counter)
    {
    	// Check if data in EEPROM is valid
		if(readFromAddress(ADDR_EEPROM, ADDR_VALID_SIGNAL, p, 1) == STT_SUCCESS)
		{
			if(*p != 'V')
			{
				// Write valid signal
				*(p + 0) = 'V';
				*(p + 1) = '\0';

				if(writeToAddress(ADDR_EEPROM, ADDR_VALID_SIGNAL, p, 1) == STT_ERROR)
				{
					Serial.println("EEPROM: write valid signal error!");
					return STT_ERROR;
				}

				// Init total items to zero
				numOfCards(0);
				numOfLog(0);
			}
			break;
		}
		counter--;
    }

    if(counter == 0)
    {
    	Serial.println("EEPROM: Init error!");
		return STT_ERROR;
    }
	
	return STT_SUCCESS;
}

/*===============================================================================================
  Name         :  
  Description  :  Read data from specify adrress with specific length.
  Argument(s)  :  
  				- EEPROMAddr 	: EEPROM Address
				- beginAddress 	: Location address
				- len			: number of bytes to read
				- result		: Readback data
  Return value :  
				- STT_ERROR 
				- STT_SUCCESS
================================================================================================*/
char AT24xxRFID::readFromAddress(byte EEPROMAddr, uint16_t beginAddress, char* result, uint16_t len)
{
	uint16_t i, nextAddress;
	
	// Time break between each actions
	delay(10);

	Wire.beginTransmission(EEPROMAddr);

	// Location address
	Wire.write((uint8_t)(beginAddress >> 8)); 			// MSB
	Wire.write((uint8_t)(beginAddress & 0xFF)); 		// LSB
	
	if(Wire.endTransmission() != 0)
	{
		Serial.println("EEPROM: I2C send slave address error!");
		return STT_ERROR;
	}
	
	// Read data
	Wire.requestFrom(EEPROMAddr, len);

	// Check num of bytes
	if(Wire.available() != len)
	{
		Serial.println("EEPROM: I2C read error!");
		return STT_ERROR;
	}

	for(i = 0; i < len; ++i)
	{
		*(result + i) = Wire.read();
	}

	// Add NULL character
	*(result + i) = '\0';

	return STT_SUCCESS;
}


/*===============================================================================================
  Name         :  
  Description  :  Write a string of data to EEPROM with specific length.
  				  AT24CXX has 64-byte page address.
				  So we must divide it to separate parts if max length is larger than one page.
  Argument(s)  :  
  				- EEPROMAddr 	: EEPROM Address
				- beginAddress 	: Starting address
				- data			: A string of data to write
  Return value : 
				- STT_ERROR 
				- STT_SUCCESS
================================================================================================*/
char AT24xxRFID::writeToAddress(byte EEPROMAddr, uint16_t beginAddress, char* data, uint16_t len)
{
	uint8_t page;
	uint16_t i, k;
	uint16_t nextAddress, nextAddress2;
	char buf[MAX_I2C_LEN];
	
	// Copy data
	memset((char*)buf, '\0', MAX_I2C_LEN);
	strcpy((char*)buf, data);

	i = ((beginAddress + len)/64);
	k = (beginAddress/64);

	if(abs(i - k) >= 1)
	{
		// Num of pages
		page = 2;
		nextAddress = (beginAddress + len)/64;
		nextAddress *= 64;

		nextAddress2 = beginAddress + len;
	}
	else
	{
		// Num of pages
		page = 1;
		nextAddress = (beginAddress + len);
	}

	i = 0;
	while(1)
	{
		// Add time break between writes
		delay(10);

		Wire.beginTransmission(EEPROMAddr);

		// Location address
		Wire.write((uint8_t)(beginAddress >> 8)); 			// MSB
		Wire.write((uint8_t)(beginAddress & 0xFF)); 		// LSB

		// Write data
		for(k = beginAddress; k < nextAddress; ++k)
		{
			Wire.write(buf[i++]);
		}

		if(Wire.endTransmission() != 0)
		{
			Serial.println("EEPROM: I2C write error!");
			return STT_ERROR;
		}

		if((page == 1) || (page == 0))
			break;

		page = 0;
		beginAddress = nextAddress;
		nextAddress = nextAddress2;
	}

	return STT_SUCCESS;
}


/*===============================================================================================
  Name         :  
  Description  :  Add new card to rfid list. And return its index.
  Argument(s)  :  
  Return value :  
				- STT_ERROR
				- STT_SUCCESS
================================================================================================*/
char AT24xxRFID::addCard(uint16_t* cardIndex, char* entry, char* rfidtag, char* cardname, char* pincode, char* misc)
{
	char status;
	uint16_t totalCard, address;

	char tmp_entry[LEN_ENTRY_BYTE+1];
	char tmp_rfidtag[LEN_TAG_BYTE+1];
	char tmp_cardname[LEN_NAME_TIME_BYTE+1];
	char tmp_pincode[LEN_PINCODE_BYTE+1];
	char tmp_misc[LEN_MISC_BYTE+1];

	// Find if card is exist in EEPROM
	if(findCardByTag(cardIndex, rfidtag) == STT_FOUND)
	{
		return STT_SUCCESS;
	}

	// Total items
	totalCard = numOfCards();

	if(totalCard >= MAX_NUM_CARD)
	{
		// Number of card items has exceed the maximum
		return STT_ERROR;
	}

	strcpy((char*)tmp_entry, entry);
	strcpy((char*)tmp_rfidtag, rfidtag);
	strcpy((char*)tmp_cardname, cardname);
	strcpy((char*)tmp_pincode, pincode);
	strcpy((char*)tmp_misc, misc);

	// Calculate address for new card
	address = ADDR_FIRST_CARD + (totalCard * LEN_CARD_BYTE);

	address += 0;
	status = writeToAddress(ADDR_EEPROM, address, tmp_entry, LEN_ENTRY_BYTE);			// Write entry
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_ENTRY_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_rfidtag, LEN_TAG_BYTE);			// Write tag
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_TAG_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_cardname, LEN_NAME_TIME_BYTE);	// Write card name
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_NAME_TIME_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_pincode, LEN_PINCODE_BYTE);		// Write pin code
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_PINCODE_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_misc, LEN_MISC_BYTE);				// Write misc
	
	if(status == STT_ERROR)
		return STT_ERROR;

	// Return card index
	*cardIndex = totalCard;

	// Increase total cards
	numOfCards(++totalCard);

	if(verifyCard(*cardIndex, entry, rfidtag, cardname, pincode, misc) != STT_MATCH)
	{
		numOfCards(--totalCard);
		return STT_ERROR;
	}	

	return STT_SUCCESS;
}

/*===============================================================================================
  Name         :  
  Description  :  Update card by index.
  Argument(s)  :  
  Return value :  
				- STT_ERROR
				- STT_SUCCESS
================================================================================================*/
char AT24xxRFID::updateCard(uint16_t cardIndex, char* entry, char* rfidtag, char* cardname, char* pincode, char* misc)
{
	char status;
	uint16_t address;

	char tmp_entry[LEN_ENTRY_BYTE+1];
	char tmp_rfidtag[LEN_TAG_BYTE+1];
	char tmp_cardname[LEN_NAME_TIME_BYTE+1];
	char tmp_pincode[LEN_PINCODE_BYTE+1];
	char tmp_misc[LEN_MISC_BYTE+1];

	if(cardIndex > numOfCards())
		return STT_ERROR;
	
	strcpy((char*)tmp_entry, entry);
	strcpy((char*)tmp_rfidtag, rfidtag);
	strcpy((char*)tmp_cardname, cardname);
	strcpy((char*)tmp_pincode, pincode);
	strcpy((char*)tmp_misc, misc);

	// Calculate address for new card
	address = ADDR_FIRST_CARD + (cardIndex * LEN_CARD_BYTE);

	address += 0;
	status = writeToAddress(ADDR_EEPROM, address, tmp_entry, LEN_ENTRY_BYTE);			// Write entry
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_ENTRY_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_rfidtag, LEN_TAG_BYTE);			// Write tag
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_TAG_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_cardname, LEN_NAME_TIME_BYTE);	// Write card name
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_NAME_TIME_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_pincode, LEN_PINCODE_BYTE);		// Write pin code
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_PINCODE_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_misc, LEN_MISC_BYTE);				// Write misc
	
	if(status == STT_ERROR)
		return STT_ERROR;

	// Verify result
	if(verifyCard(cardIndex, entry, rfidtag, cardname, pincode, misc) != STT_MATCH)
		return STT_ERROR;

	return STT_SUCCESS;
}

/*===============================================================================================
  Name         :  
  Description  :  Verify card by index.
  Argument(s)  :  
  Return value :  
				- STT_ERROR
				- STT_MATCH
				- STT_NOTMATCH
================================================================================================*/
char AT24xxRFID::verifyCard(uint16_t cardIndex, char* entry, char* rfidtag, char* cardname, char* pincode, char* misc)
{
	char tmp_entry[LEN_ENTRY_BYTE+1];
	char tmp_rfidtag[LEN_TAG_BYTE+1];
	char tmp_cardname[LEN_NAME_TIME_BYTE+1];
	char tmp_pincode[LEN_PINCODE_BYTE+1];
	char tmp_misc[LEN_MISC_BYTE+1];

	if(getCard(cardIndex, (char*)tmp_entry, (char*)tmp_rfidtag, (char*)tmp_cardname, (char*)tmp_pincode, (char*)tmp_misc) == STT_ERROR)
	{
		return STT_ERROR;
	}

	if(strcmp(entry, (char*)tmp_entry) != 0)
		return STT_NOTMATCH;
	
	if(strcmp(rfidtag, (char*)tmp_rfidtag) != 0)
		return STT_NOTMATCH;
	
	if(strcmp(cardname, (char*)tmp_cardname) != 0)
		return STT_NOTMATCH;
	
	if(strcmp(pincode, (char*)tmp_pincode) != 0)
		return STT_NOTMATCH;
	
	if(strcmp(misc, (char*)tmp_misc) != 0)
		return STT_NOTMATCH;

	return STT_MATCH;
}

/*===============================================================================================
  Name         :  
  Description  :
  Argument(s)  :  
  Return value :  
				- STT_ERROR
				- STT_SUCCESS
================================================================================================*/
char AT24xxRFID::getCard(uint16_t cardIndex, char* entry, char* rfidtag, char* cardname, char* pincode, char* misc)
{
	char status;
	uint16_t address;

	// Check if index is valid
	if(cardIndex > numOfCards())
		return STT_ERROR;

	// Calculate address
	address = ADDR_FIRST_CARD + (cardIndex * LEN_CARD_BYTE);

	address += 0;
	status = readFromAddress(ADDR_EEPROM, address, entry, LEN_ENTRY_BYTE);			// Read entry
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_ENTRY_BYTE;
	status = readFromAddress(ADDR_EEPROM, address, rfidtag, LEN_TAG_BYTE);			// Read tag
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_TAG_BYTE;
	status = readFromAddress(ADDR_EEPROM, address, cardname, LEN_NAME_TIME_BYTE);	// Read card name
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_NAME_TIME_BYTE;
	status = readFromAddress(ADDR_EEPROM, address, pincode, LEN_PINCODE_BYTE);		// Read pin code
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_PINCODE_BYTE;
	status = readFromAddress(ADDR_EEPROM, address, misc, LEN_MISC_BYTE);			// Read misc
	
	if(status == STT_ERROR)
		return STT_ERROR;

	return STT_SUCCESS;
}

/*===============================================================================================
  Name         :  
  Description  :  Find tag in RFID list and return card index.
  Argument(s)  :  
  				- cardIndex : If found, it contains index of found card.
  				- rfidtag 	: Tag to find.
  Return value :  
				- STT_ERROR
  				- STT_FOUND
  				- STT_NOTFOUND
================================================================================================*/
char AT24xxRFID::findCardByTag(uint16_t* cardIndex, char* rfidtag)
{
	uint16_t i, total;

	char tmp_entry[LEN_ENTRY_BYTE+1];
	char tmp_rfidtag[LEN_TAG_BYTE+1];
	char tmp_cardname[LEN_NAME_TIME_BYTE+1];
	char tmp_pincode[LEN_PINCODE_BYTE+1];
	char tmp_misc[LEN_MISC_BYTE+1];

	total = numOfCards();

	for (i = 0; i < total; ++i)
	{
		if(getCard(i, tmp_entry, tmp_rfidtag, tmp_cardname, tmp_pincode, tmp_misc) == STT_ERROR)
			return STT_ERROR;

		if(strcmp(rfidtag, tmp_rfidtag) == 0)
		{
			*cardIndex = i;
			return STT_FOUND;
		}
	}

	return STT_NOTFOUND;
}

/*===============================================================================================
  Name         :  
  Description  :  Delete card in rfid-list and automatic decrease number of cards.
  				  This function move data from a location to another location. If it return ERROR
  				  it may cause an EEPROM data corrupt!!!
  Argument(s)  :  
  				- cardIndex : 0, 1, 2, 3,..
  Return value :  
				- STT_ERROR
				- STT_SUCCESS
================================================================================================*/
char AT24xxRFID::deleteCard(uint16_t cardIndex)
{
	uint8_t i;
	char *p = pBuffer;
	uint16_t totalCard, addr1, addr2;

	totalCard = numOfCards();

	// Check if index is valid
	if(cardIndex > totalCard)
		return STT_ERROR;
	
	// There is only one item in the list
	// Just set number of item to zero.
	if(totalCard == 1)
	{
		numOfCards(0);
		return STT_SUCCESS;
	}

	// This is the last item. 
	// Just decrease total number of card.
	if(cardIndex == (totalCard-1))
	{
		numOfCards(--totalCard);
		return STT_SUCCESS;
	}

	while(cardIndex < (totalCard-1))
	{
		// Calculate address
		addr1 = ADDR_FIRST_CARD + (cardIndex * LEN_CARD_BYTE);
		addr2 = addr1 + LEN_CARD_BYTE;

		for (i = 0; i < LEN_CARD_BYTE; ++i)
		{
			if(readFromAddress(ADDR_EEPROM, addr2 + i, p, 1) == STT_ERROR)
				return STT_ERROR;

			if(writeToAddress(ADDR_EEPROM, addr1 + i, p, 1) == STT_ERROR)
				return STT_ERROR;
		}
				
		// Increase index
		cardIndex++;
	}
	
	numOfCards(--totalCard);

	return STT_SUCCESS;
}

/*===============================================================================================
  Name         :  
  Description  :  
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
void AT24xxRFID::numOfCards(uint16_t num)
{
	byte counter = 3;
	char* p = pBuffer;

	// Store as ASCII format
	*(p + 0) = (char)(num >>8) + 48;
	*(p + 1) = (char)(num & 0xFF) + 48;
	*(p + 2) = '\0';

	while(counter)
	{
		if(writeToAddress(ADDR_EEPROM, ADDR_NUMOF_CARD, p, 2) == STT_SUCCESS)
			break;

		counter--;
	}
}

/*===============================================================================================
  Name         :  
  Description  :  
  Argument(s)  :  
  Return value :  Number of cards.
  				 - 0xFFFF : Read error!
================================================================================================*/
uint16_t AT24xxRFID::numOfCards(void)
{
	byte counter = 3;
	byte high, low;
	uint16_t result = 0;
	char* p = pBuffer;

	while(counter)
	{
		if(readFromAddress(ADDR_EEPROM, ADDR_NUMOF_CARD, p, 2) == STT_SUCCESS)
		{
			// Decode form ASCII format
			high = (byte)(*(p + 0)) - 48;
			low  = (byte)(*(p + 1)) - 48;

			result = (high<<8) + low;
			break;
		}
		counter--;
	}

	// Error
	if(counter == 0)
		return 0xFFFF;

	return result;
}

/*===============================================================================================
  Name         :  
  Description  :  
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
void AT24xxRFID::numOfLog(uint16_t num)
{
	byte counter = 3;
	char* p = pBuffer;

	// Store as ASCII format
	*(p + 0) = (char)(num >>8) + 48;
	*(p + 1) = (char)(num & 0xFF) + 48;
	*(p + 2) = '\0';

	while(counter)
	{
		if(writeToAddress(ADDR_EEPROM, ADDR_NUMOF_LOG, p, 2) == STT_SUCCESS)
			break;

		counter--;
	}
}

/*===============================================================================================
  Name         :  
  Description  :  
  Argument(s)  :  
  Return value : - Number of logs.
  				 - 0xFFFF : Read error!
================================================================================================*/
uint16_t AT24xxRFID::numOfLog(void)
{
	byte counter = 3;
	byte high, low;
	uint16_t result = 0;
	char* p = pBuffer;

	while(counter)
	{
		if(readFromAddress(ADDR_EEPROM, ADDR_NUMOF_LOG, p, 2) == STT_SUCCESS)
		{
			// Decode form ASCII format
			high = (byte)(*(p + 0)) - 48;
			low  = (byte)(*(p + 1)) - 48;

			result = (high<<8) + low;
			break;
		}
		counter--;
	}
	
	// Error
	if(counter == 0)
		return 0xFFFF;

	return result;
}

/*===============================================================================================
  Name         :  
  Description  :  Write log to EEPROM.
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
char AT24xxRFID::logWrite(uint16_t* logIndex, char* entry, char* rfidtag, char* datetime, 
						  uint16_t numWord, uint8_t numByte)
{
	char status;
	uint16_t totalLog, address;

	char tmp_entry[LEN_ENTRY_BYTE+1];
	char tmp_rfidtag[LEN_TAG_BYTE+1];
	char tmp_datetime[LEN_NAME_TIME_BYTE+1];
	char tmp_numWord[LEN_PINCODE_BYTE+1];
	char tmp_numByte[LEN_MISC_BYTE+1];

	// Total items in list
	totalLog = numOfLog();

	if(totalLog > MAX_NUM_LOG)
	{
		// Number of log items has exceed the maximum
		// Roll index to zero
		*logIndex = 0;
	}
	else
	{
		*logIndex = totalLog;
		numOfLog(++totalLog);
	}

	strcpy((char*)tmp_entry, entry);
	strcpy((char*)tmp_rfidtag, rfidtag);
	strcpy((char*)tmp_datetime, datetime);

	itoa(numWord, (char*)tmp_numWord, 10);
	itoa(numByte, (char*)tmp_numByte, 10);

	// Calculate address for new log
	address = ADDR_FIRST_LOG + ((*logIndex) * LEN_CARD_BYTE);

	address += 0;
	status = writeToAddress(ADDR_EEPROM, address, tmp_entry, LEN_ENTRY_BYTE);			// Write entry
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_ENTRY_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_rfidtag, LEN_TAG_BYTE);			// Write tag
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_TAG_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_datetime, LEN_NAME_TIME_BYTE);	// Write card name
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_NAME_TIME_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_numWord, LEN_PINCODE_BYTE);		// Write word
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_PINCODE_BYTE;
	status = writeToAddress(ADDR_EEPROM, address, tmp_numByte, LEN_MISC_BYTE);			// Write byte
	
	if(status == STT_ERROR)
		return STT_ERROR;

	return STT_SUCCESS;
}

/*===============================================================================================
  Name         :  
  Description  :  Read log item from EEPROM by index.
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
char AT24xxRFID::logRead(uint16_t logIndex, char* entry, char* rfidtag, char* datetime, 
						 uint16_t* numWord, uint8_t* numByte)
{
	char status;
	char *p = pBuffer;	
	uint16_t totalCard, address;

	// Check if index is valid
	if(logIndex > numOfLog())
		return STT_ERROR;

	// Calculate address
	address = ADDR_FIRST_LOG + (logIndex * LEN_CARD_BYTE);

	address += 0;
	status = readFromAddress(ADDR_EEPROM, address, entry, LEN_ENTRY_BYTE);			// Read entry
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_ENTRY_BYTE;
	status = readFromAddress(ADDR_EEPROM, address, rfidtag, LEN_TAG_BYTE);			// Read tag
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_TAG_BYTE;
	status = readFromAddress(ADDR_EEPROM, address, datetime, LEN_NAME_TIME_BYTE);	// Read date time
	
	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_NAME_TIME_BYTE;
	status = readFromAddress(ADDR_EEPROM, address, p, LEN_PINCODE_BYTE);			// Read a word
	*numWord = atoi(p);

	if(status == STT_ERROR)
		return STT_ERROR;

	address += LEN_PINCODE_BYTE;
	status = readFromAddress(ADDR_EEPROM, address, p, LEN_MISC_BYTE);				// Read a byte
	*numByte = atoi(p);

	if(status == STT_ERROR)
		return STT_ERROR;

	return STT_SUCCESS;
}

