#ifndef _SETIODATA_H
#define _SETIODATA_H
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

char *choices[] =
{
	"Load Config",
	"Write Config",
	"STARTSWITCH",
	"SHUTDOWN",
	"BRAKES",
	"HEADLIGHTS",
	"LEFTBLINKERI",
	"RIGHTBLINKERI     ",
	"RUNNINGLIGHTS",
	"LEFTDOOROPEN",
	"RIGHTDOOROPEN",
	"TESTINPUT9",
	"TESTINPUT10",
	"TESTINPUT11",
	"TESTINPUT12",
	"TESTINPUT13",
	"TESTINPUT14",
	"TESTINPUT15",
	"TESTINPUT16",
	"TESTINPUT17",
	"TESTINPUT18",
	"TESTINPUT19",
	"UNKNOWN1A",		// there's a gap here for the same reason as above (see output defines)
	"UNKNOWN2A",
	"UNKNOWN3A",
	"UNKNOWN4A",
	"TESTINPUT24",
	"TESTINPUT25",
	"TESTINPUT26",
	"TESTINPUT27",
	"TESTINPUT28",
	"TESTINPUT29",
	"TESTINPUT30",
	"TESTINPUT31",
	"TESTINPUT32",
	"TESTINPUT33",
	"TESTINPUT34",
	"TESTINPUT35",
	"TESTINPUT36",
	"TESTINPUT37",
	"TESTINPUT38",
	"TESTINPUT39",
	"TESTINPUT40",
	"TESTINPUT41",
	"TESTINPUT42",
	"TESTINPUT43",
	"Exit",
	(char *)NULL,
};
char *choice_desc[] =
{
	"load config",
	"write config",
	"start switch",
	"panic button",
	"brake lights",
	"headlights",
	"left blinker",
	"RIGHTBLINKERI    ",
	"RUNNINGLIGHTS",
	"LEFTDOOROPEN",
	"RIGHTDOOROPEN",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"blank",		// there's a gap here for the same reason as above (see output defines)
	"blank",
	"blank",
	"blank",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"not used",
	"exit",
	(char *)NULL,
};
#endif
