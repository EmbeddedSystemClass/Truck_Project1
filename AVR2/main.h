#define EEPROM_SIZE 0x400
#define RT_OFFSET 0x70
UCHAR eeprom_sim[EEPROM_SIZE];

#ifdef SHOW_EEPROM
int burn_eeprom(void);
#endif

// warning: these are also defined in the esos1 directory
enum key_types
{
	TEST1 = 0xC9,
	TEST2,			//  - CA
	TEST3,			//  - CB
	TEST4,			//  - CC
	TEST5,			//	- CD
	TEST6,			//	- CE
	TEST7,			//	- CF
	TEST8,			//	- D0
	TEST9,			//	- D1
	TEST10,			//	- D2
	TEST11,			//	- D3
	TEST12,			//	- D4
	TEST13,			//	- D5
	TEST14,			//	- D6
	TEST15,			//	- D7
	TEST16,			//	- D8
	TEST17,			//  - D9
	LOAD_RAM,		//  - DA
	INIT, 			//	- DB
	SPACE,			//	- DC
	BURN_PART,		//  - DD
	READ_EEPROM1,	//	- DE
	READ_EEPROM2,	//	- DF

	KP_POUND,	// '#'	- E0
	KP_AST, // '*'		- E1
	KP_0, // '0'		- E2
	KP_1, // '1'		- E3
	KP_2, // '2'		- E4
	KP_3, // '3'		- E5
	KP_4, // '4'		- E6
	KP_5, // '5'		- E7
	KP_6, // '6'		- E8
	KP_7, // '7'		- E9
	KP_8, // '8'		- EA
	KP_9, // '9'		- EB
	KP_A, // 'A'		- EC
	KP_B, // 'B'		- ED
	KP_C, // 'C'		- EE
	KP_D, // 'D'		- EF
} KEY_TYPES;

enum rt_types
{
	RT_RPM = RT_OFFSET,
	RT_ENGT,
	RT_TRIP,
	RT_TIME,
	RT_AIRT,
	RT_MPH,
	RT_OILP,
	RT_MAP,
	RT_OILT,
	RT_O2,
} RT_TYPES;

