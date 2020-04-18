
## ARDUINO LIBRARY FOR LOGGING TO I2C-EEPROM

1. Swipe RFID card
2. Check, if it is in the EEPROM -return name of cardholder(also stored in EEPROM)
3. If so, chek pin-code(in EEPROM)
4. If correct, log some data in EEPROM

### Example of stored information

Entry,RFIDTAG*******,NameMax20Char,pin***,misc
123,4E00A71C7580,Benny Andersson,1234,1011

### Example of logged information

Entry,RFIDTAG*******,Date&Time,a number,a byte
123,4E00A71C7580,01012015-13:30,9999,240

- RFID tags in EEPR- OM needs to be numbered, 1,2,3....
- Needs some way to check how many entrys there are
- Needs some way to access/delete a whole entry
- Need some function to maintain EEPROM, delete, verify, update rfid-list.

**Byte orders**
- 0x0000 		1-byte 		Valid signal
- 0x0001 		2-byte 		Num of card

Entry,RFIDTAG*******,NameMax20Char,pin***,misc
123,4E00A71C7580,Benny Andersson,1234,1011

Max 160 cards
Card address (49 bytes)
- 0x0003 		5-byte 		Entry
- 0x0008 		13-byte 	RFID Tag
- 0x0014 		20-byte 	Name
- 0x0028 		5-byte 		Pin code
- 0x002C 		5-byte 		Misc
- 0x0030 		1-byte 		(NULL)Space between each items

Entry,RFIDTAG*******,Date&Time,a number,a byte
123,4E00A71C7580,01012015-13:30,9999,240

Log address (49 bytes)
- 0x8192 		2-byte 		Num of log items

- 0x8194 		5-byte 		Entry
- 0x8199 		13-byte 	RFID Tag
- 0x8192 		20-byte 	Date time
- 0x8192 		5-byte 		A word
- 0x8192 		5-byte 		A byte
- 0x8192 		1-byte 		(NULL)Space between each items

## Memory Organization

Max 160 cards
Max 490 log items.

Card address (49 bytes)
Log address (49 bytes)

- 0x0000 		1-byte 		Valid signal
- 0x0001 		2-byte 		Num of card

- 0x0003 		5-byte 		Entry
- 0x0008 		13-byte 	RFID Tag
- 0x0014 		20-byte 	Name
- 0x0028 		5-byte 		Pin code
- 0x002C 		5-byte 		Misc
- 0x0030 		1-byte 		(NULL)Space between each items


- 0x8192 		2-byte 		Num of log items

- 0x8194 		5-byte 		Entry
- 0x8199 		13-byte 	RFID Tag
- 0x8212 		20-byte 	Date time
- 0x8232 		5-byte 		A word
- 0x8237 		5-byte 		A byte
- 0x8242 		1-byte 		(NULL)Space between each items

## Contribution

- Any contribute please make a pull request.
- Free to use for any purpose. Use at your own risk.
