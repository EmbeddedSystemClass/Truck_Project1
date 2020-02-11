// create a file called waypoints.conf that has a list of waypoints: name, latitude & longitude
// which represent the waypoints in order along a path, or trip. 
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include "../../mytypes.h"

int main(void)
{
	int fp;
	char filename[20] = "waypoints.conf\0";
	char *fptr = filename;
	WAYPOINTS ps;
	UCHAR id = 0xAB;

	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
	write(fp,&id,1);

	strcpy(ps.name,"Perkins\0");
	ps.latitude = 44.271462;
	ps.longitude = -105.494089;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Garner Lake Rd\0");
	ps.latitude = 44.247417;
	ps.longitude = -105.478203;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Bishop Rd\0");
	ps.latitude = 44.151905;
	ps.longitude = -105.447020;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Haight Rd\0");
	ps.latitude = 44.028059;
	ps.longitude = -105.465033;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Wagensen Rd\0");
	ps.latitude = 43.931134;
	ps.longitude = -105.446850;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Breen Rd\0");
	ps.latitude = 43.853904;
	ps.longitude = -105.461250;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Durham Ranch\0");
	ps.latitude = 43.807896;
	ps.longitude = -105.469165;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Bloomings\0");
	ps.latitude = 43.771580;
	ps.longitude = -105.476190;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"garage\0");
	ps.latitude = 43.759182;
	ps.longitude = -105.487160;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"apt\0");	
	ps.latitude = 43.757889;
	ps.longitude = -105.488953;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Hank's\0");
	ps.latitude = 43.761494;
	ps.longitude = -105.474731;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Wright Blvd\0");
	ps.latitude = 43.750683;
	ps.longitude = -105.475280;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Clareton Hwy\0");
	ps.latitude = 43.722882;
	ps.longitude = -105.473881;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Cosner Rd.\0");
	ps.latitude = 43.676618;
	ps.longitude = -105.469508;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Edwards Rd.\0");
	ps.latitude = 43.652444;
	ps.longitude = -105.468603;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Antelope Rd.\0");
	ps.latitude = 43.421652;
	ps.longitude = -105.405686;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Rest Area\0");
	ps.latitude = 43.320457;
	ps.longitude = -105.350739;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"NBill Disp\0");
	ps.latitude = 43.296722;
	ps.longitude = -105.330917;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Bill Store\0");
	ps.latitude = 43.232666;
	ps.longitude = -105.260056;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Trees\0");
	ps.latitude = 43.180722;
	ps.longitude = -105.259717;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Tanks\0");
	ps.latitude = 43.020543;
	ps.longitude = -105.262975;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Underpass\0");
	ps.latitude = 42.977341;
	ps.longitude = -105.263947;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"CR-43\0");
	ps.latitude = 42.882131;
	ps.longitude = -105.274969;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Dickau Rd.\0");
	ps.latitude = 42.958828;
	ps.longitude = -105.037552;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"School House\0");
	ps.latitude = 42.958070;
	ps.longitude = -105.118558;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Big Ranch\0");
	ps.latitude = 42.990751;
	ps.longitude = -104.909181;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Little Ranch\0");
	ps.latitude = 43.052063;
	ps.longitude = -104.831919;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Divide Rd.\0");
	ps.latitude = 43.043945;
	ps.longitude = -104.776352;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Five Pans\0");
	ps.latitude = 43.012962;
	ps.longitude = -104.794579;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Driveway\0");
	ps.latitude = 42.998989;
	ps.longitude = -104.804695;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"North Post\0");
	ps.latitude = 42.999920;
	ps.longitude = -104.803780;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"SW Post\0");
	ps.latitude = 42.995049;
	ps.longitude = -104.807266;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Upper Left\0");
	ps.latitude = 42.996880;
	ps.longitude = -104.802963;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"North Shack\0");
	ps.latitude = 42.998542;
	ps.longitude = -104.804031;
	write(fp,&ps,sizeof(WAYPOINTS));

	strcpy(ps.name,"Shack\0");
	ps.latitude = 42.997181;
	ps.longitude = -104.803291;
	write(fp,&ps,sizeof(WAYPOINTS));

	id = 0x54;
	write(fp,&id,1);
	
	close(fp);

	return 0;
}
