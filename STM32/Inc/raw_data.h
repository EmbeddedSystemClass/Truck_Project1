typedef struct
{
	char str[10];
	int raw;
}RAW_DATA;

static RAW_DATA raw_data[360] =
{{"31.1\0", 511},
{"30.2\0", 510},
{"29.3\0", 509},
{"28.4\0", 508},
{"27.5\0", 507},
{"26.6\0", 506},
{"25.7\0", 505},
{"24.8\0", 504},
{"23.9\0", 503},
{"23.0\0", 502},
{"22.1\0", 501},
{"21.2\0", 500},
{"20.3\0", 499},
{"19.4\0", 498},
{"18.5\0", 497},
{"17.6\0", 496},
{"16.7\0", 495},
{"15.8\0", 494},
{"14.9\0", 493},
{"14.0\0", 492},
{"13.1\0", 491},
{"12.2\0", 490},
{"11.3\0", 489},
{"10.4\0", 488},
{"9.5\0", 487},
{"8.6\0", 486},
{"7.7\0", 485},
{"6.8\0", 484},
{"5.9\0", 483},
{"5.0\0", 482},
{"4.1\0", 481},
{"3.2\0", 480},
{"2.3\0", 479},
{"1.4\0", 478},
{"0.5\0", 477},
{"-0.4\0", 476},
{"-1.3\0", 475},
{"-2.2\0", 474},
{"-3.1\0", 473},
{"-4.0\0", 472},
{"-4.9\0", 471},
{"-5.8\0", 470},
{"-6.7\0", 469},
{"-7.6\0", 468},
{"-8.5\0", 467},
{"-9.4\0", 466},
{"-10.3\0", 465},
{"-11.2\0", 464},
{"-12.1\0", 463},
{"-13.0\0", 462},
{"-13.9\0", 461},
{"-14.8\0", 460},
{"-15.7\0", 459},
{"-16.6\0", 458},
{"-17.5\0", 457},
{"-18.4\0", 456},
{"-19.3\0", 455},
{"-20.2\0", 454},
{"-21.1\0", 453},
{"-22.0\0", 452},
{"-22.9\0", 451},
{"-23.8\0", 450},
{"-24.7\0", 449},
{"-25.6\0", 448},
{"-26.5\0", 447},
{"-27.4\0", 446},
{"-28.3\0", 445},
{"-29.2\0", 444},
{"-30.1\0", 443},
{"-31.0\0", 442},
{"-31.9\0", 441},
{"-32.8\0", 440},
{"-33.7\0", 439},
{"-34.6\0", 438},
{"-35.5\0", 437},
{"-36.4\0", 436},
{"-37.3\0", 435},
{"-38.2\0", 434},
{"-39.1\0", 433},
{"-40.0\0", 432},
{"-40.9\0", 431},
{"-41.8\0", 430},
{"-42.7\0", 429},
{"-43.6\0", 428},
{"-44.5\0", 427},
{"-45.4\0", 426},
{"-46.3\0", 425},
{"-47.2\0", 424},
{"-48.1\0", 423},
{"-49.0\0", 422},
{"-49.9\0", 421},
{"-50.8\0", 420},
{"-51.7\0", 419},
{"-52.6\0", 418},
{"-53.5\0", 417},
{"-54.4\0", 416},
{"-55.3\0", 415},
{"-56.2\0", 414},
{"-57.1\0", 413},
{"-58.0\0", 412},
{"-58.9\0", 411},
{"-59.8\0", 410},
{"-60.7\0", 409},
{"-61.6\0", 408},
{"-62.5\0", 407},
{"-63.4\0", 406},
{"-64.3\0", 405},
{"-65.2\0", 404},
{"-66.1\0", 403},
{"257.0\0", 250},
{"256.1\0", 249},
{"255.2\0", 248},
{"254.3\0", 247},
{"253.4\0", 246},
{"252.5\0", 245},
{"251.6\0", 244},
{"250.7\0", 243},
{"249.8\0", 242},
{"248.9\0", 241},
{"248.0\0", 240},
{"247.1\0", 239},
{"246.2\0", 238},
{"245.3\0", 237},
{"244.4\0", 236},
{"243.5\0", 235},
{"242.6\0", 234},
{"241.7\0", 233},
{"240.8\0", 232},
{"239.9\0", 231},
{"239.0\0", 230},
{"238.1\0", 229},
{"237.2\0", 228},
{"236.3\0", 227},
{"235.4\0", 226},
{"234.5\0", 225},
{"233.6\0", 224},
{"232.7\0", 223},
{"231.8\0", 222},
{"230.9\0", 221},
{"230.0\0", 220},
{"229.1\0", 219},
{"228.2\0", 218},
{"227.3\0", 217},
{"226.4\0", 216},
{"225.5\0", 215},
{"224.6\0", 214},
{"223.7\0", 213},
{"222.8\0", 212},
{"221.9\0", 211},
{"221.0\0", 210},
{"220.1\0", 209},
{"219.2\0", 208},
{"218.3\0", 207},
{"217.4\0", 206},
{"216.5\0", 205},
{"215.6\0", 204},
{"214.7\0", 203},
{"213.8\0", 202},
{"212.9\0", 201},
{"212.0\0", 200},
{"211.1\0", 199},
{"210.2\0", 198},
{"209.3\0", 197},
{"208.4\0", 196},
{"207.5\0", 195},
{"206.6\0", 194},
{"205.7\0", 193},
{"204.8\0", 192},
{"203.9\0", 191},
{"203.0\0", 190},
{"202.1\0", 189},
{"201.2\0", 188},
{"200.3\0", 187},
{"199.4\0", 186},
{"198.5\0", 185},
{"197.6\0", 184},
{"196.7\0", 183},
{"195.8\0", 182},
{"194.9\0", 181},
{"194.0\0", 180},
{"193.1\0", 179},
{"192.2\0", 178},
{"191.3\0", 177},
{"190.4\0", 176},
{"189.5\0", 175},
{"188.6\0", 174},
{"187.7\0", 173},
{"186.8\0", 172},
{"185.9\0", 171},
{"185.0\0", 170},
{"184.1\0", 169},
{"183.2\0", 168},
{"182.3\0", 167},
{"181.4\0", 166},
{"180.5\0", 165},
{"179.6\0", 164},
{"178.7\0", 163},
{"177.8\0", 162},
{"176.9\0", 161},
{"176.0\0", 160},
{"175.1\0", 159},
{"174.2\0", 158},
{"173.3\0", 157},
{"172.4\0", 156},
{"171.5\0", 155},
{"170.6\0", 154},
{"169.7\0", 153},
{"168.8\0", 152},
{"167.9\0", 151},
{"167.0\0", 150},
{"166.1\0", 149},
{"165.2\0", 148},
{"164.3\0", 147},
{"163.4\0", 146},
{"162.5\0", 145},
{"161.6\0", 144},
{"160.7\0", 143},
{"159.8\0", 142},
{"158.9\0", 141},
{"158.0\0", 140},
{"157.1\0", 139},
{"156.2\0", 138},
{"155.3\0", 137},
{"154.4\0", 136},
{"153.5\0", 135},
{"152.6\0", 134},
{"151.7\0", 133},
{"150.8\0", 132},
{"149.9\0", 131},
{"149.0\0", 130},
{"148.1\0", 129},
{"147.2\0", 128},
{"146.3\0", 127},
{"145.4\0", 126},
{"144.5\0", 125},
{"143.6\0", 124},
{"142.7\0", 123},
{"141.8\0", 122},
{"140.9\0", 121},
{"140.0\0", 120},
{"139.1\0", 119},
{"138.2\0", 118},
{"137.3\0", 117},
{"136.4\0", 116},
{"135.5\0", 115},
{"134.6\0", 114},
{"133.7\0", 113},
{"132.8\0", 112},
{"131.9\0", 111},
{"131.0\0", 110},
{"130.1\0", 109},
{"129.2\0", 108},
{"128.3\0", 107},
{"127.4\0", 106},
{"126.5\0", 105},
{"125.6\0", 104},
{"124.7\0", 103},
{"123.8\0", 102},
{"122.9\0", 101},
{"122.0\0", 100},
{"121.1\0", 99},
{"120.2\0", 98},
{"119.3\0", 97},
{"118.4\0", 96},
{"117.5\0", 95},
{"116.6\0", 94},
{"115.7\0", 93},
{"114.8\0", 92},
{"113.9\0", 91},
{"113.0\0", 90},
{"112.1\0", 89},
{"111.2\0", 88},
{"110.3\0", 87},
{"109.4\0", 86},
{"108.5\0", 85},
{"107.6\0", 84},
{"106.7\0", 83},
{"105.8\0", 82},
{"104.9\0", 81},
{"104.0\0", 80},
{"103.1\0", 79},
{"102.2\0", 78},
{"101.3\0", 77},
{"100.4\0", 76},
{"99.5\0", 75},
{"98.6\0", 74},
{"97.7\0", 73},
{"96.8\0", 72},
{"95.9\0", 71},
{"95.0\0", 70},
{"94.1\0", 69},
{"93.2\0", 68},
{"92.3\0", 67},
{"91.4\0", 66},
{"90.5\0", 65},
{"89.6\0", 64},
{"88.7\0", 63},
{"87.8\0", 62},
{"86.9\0", 61},
{"86.0\0", 60},
{"85.1\0", 59},
{"84.2\0", 58},
{"83.3\0", 57},
{"82.4\0", 56},
{"81.5\0", 55},
{"80.6\0", 54},
{"79.7\0", 53},
{"78.8\0", 52},
{"77.9\0", 51},
{"77.0\0", 50},
{"76.1\0", 49},
{"75.2\0", 48},
{"74.3\0", 47},
{"73.4\0", 46},
{"72.5\0", 45},
{"71.6\0", 44},
{"70.7\0", 43},
{"69.8\0", 42},
{"68.9\0", 41},
{"68.0\0", 40},
{"67.1\0", 39},
{"66.2\0", 38},
{"65.3\0", 37},
{"64.4\0", 36},
{"63.5\0", 35},
{"62.6\0", 34},
{"61.7\0", 33},
{"60.8\0", 32},
{"59.9\0", 31},
{"59.0\0", 30},
{"58.1\0", 29},
{"57.2\0", 28},
{"56.3\0", 27},
{"55.4\0", 26},
{"54.5\0", 25},
{"53.6\0", 24},
{"52.7\0", 23},
{"51.8\0", 22},
{"50.9\0", 21},
{"50.0\0", 20},
{"49.1\0", 19},
{"48.2\0", 18},
{"47.3\0", 17},
{"46.4\0", 16},
{"45.5\0", 15},
{"44.6\0", 14},
{"43.7\0", 13},
{"42.8\0", 12},
{"41.9\0", 11},
{"41.0\0", 10},
{"40.1\0", 9},
{"39.2\0", 8},
{"38.3\0", 7},
{"37.4\0", 6},
{"36.5\0", 5},
{"35.6\0", 4},
{"34.7\0", 3},
{"33.8\0", 2},
{"32.9\0", 1},
{"0   \0", 0}};

