// Copyright (c) 2018 Kari Karvonen, OH1KK

#include "ext.h"	// all calls to the extension interface begin with "ext_", e.g. ext_register()

#include "kiwi.h"
#include "cfg.h"
#include "str.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>

//#define ANT_SWITCH_DEBUG_MSG	true
#define ANT_SWITCH_DEBUG_MSG	false

// rx_chan is the receiver channel number we've been assigned, 0..RX_CHAN
// We need this so the extension can support multiple users, each with their own ant_switch[] data structure.

struct ant_switch_t {
	int rx_chan;
	int run;
};

static ant_switch_t ant_switch[RX_CHANS];

char * ant_switch_queryantennas() {
	char *cmd, *reply;
	static char selected_antennas[256];
	int n;
	asprintf(&cmd, "/root/extensions/ant_switch/frontend/ant-switch-frontend s");
	reply = non_blocking_cmd(cmd, NULL);
	n = sscanf(kstr_sp(reply), "Selected antennas: %s", selected_antennas);
	free(cmd);
	kstr_free(reply);
	if (!n) printf("ant_switch_queryantenna BAD STATUS? <%s>\n", reply);
	return(selected_antennas);
}

int ant_switch_setantenna(char* antenna) {
        char *cmd, *reply;
	int status;
	int n;
        asprintf(&cmd, "/root/extensions/ant_switch/frontend/ant-switch-frontend %s", antenna);
	reply = non_blocking_cmd(cmd,NULL);
	free(cmd);
	kstr_free(reply);
	return(0);
}

int ant_switch_toggleantenna(char* antenna) {
        char *cmd, *reply;
	int status;
	int n;
        asprintf(&cmd, "/root/extensions/ant_switch/frontend/ant-switch-frontend t%s", antenna);
	reply = non_blocking_cmd(cmd, NULL);
	free(cmd);
	kstr_free(reply);
	return(0);
}

int ant_switch_validate_cmd(char *cmd) {
	int is_valid_cmd = false;
	if (strcmp(cmd, "1") == 0) is_valid_cmd=true;
	if (strcmp(cmd, "2") == 0) is_valid_cmd=true;
	if (strcmp(cmd, "3") == 0) is_valid_cmd=true;
	if (strcmp(cmd, "4") == 0) is_valid_cmd=true;
	if (strcmp(cmd, "5") == 0) is_valid_cmd=true;
	if (strcmp(cmd, "6") == 0) is_valid_cmd=true;
	if (strcmp(cmd, "7") == 0) is_valid_cmd=true;
	if (strcmp(cmd, "8") == 0) is_valid_cmd=true;
	if (strcmp(cmd, "g") == 0) is_valid_cmd=true;
	return(is_valid_cmd);
}

bool ant_switch_read_denyswitching() {
     bool error;
     char cfgparam[26]="ant_switch.denyswitching\0";
     int result = cfg_int(cfgparam, &error, CFG_OPTIONAL);
     // error handling: if deny parameter is not defined, or it is 0, then switching is allowed
     if (result == 1) return true; else return false;
}

bool ant_switch_read_denymixing() {
     bool error;
     char cfgparam[26]="ant_switch.denymixing\0";
     int result = cfg_int(cfgparam, &error, CFG_OPTIONAL);
     // error handling: if deny parameter is not defined, or it is 0, then mixing is allowed
     if (result == 1) return true; else return false;
}

bool ant_switch_read_denymultiuser() {
     bool error;
     char cfgparam[26]="ant_switch.denymultiuser\0";
     int result = cfg_int(cfgparam, &error, CFG_OPTIONAL);
     // error handling: if deny parameter is not defined, or it is 0, then switching is allowed
     if (result == 1) {
         // option is set. Now check if more than 1 user online rx_util.cpp current_nusers variable
        if(current_nusers > 1) return true; else return false;
    } else {
        return false;
    }
}

bool ant_switch_msgs(char *msg, int rx_chan)
{
	ant_switch_t *e = &ant_switch[rx_chan];
	int n=0;
	char antenna[256];

	printf("### ant_switch_msgs RX%d <%s>\n", rx_chan, msg);
	
	if (strcmp(msg, "SET ext_server_init") == 0) {
		e->rx_chan = rx_chan;	// remember our receiver channel number
		ext_send_msg(e->rx_chan, ANT_SWITCH_DEBUG_MSG, "EXT ready");
		return true;
	}

        n = sscanf(msg, "SET Antenna=%s", antenna);
        if (n == 1) {
                if (ant_switch_read_denyswitching()==true || ant_switch_read_denymultiuser()==true) {
                    ext_send_msg(e->rx_chan, ANT_SWITCH_DEBUG_MSG, "EXT AntennaDenySwitching=1");
                    return true;            
                } else {
                    ext_send_msg(e->rx_chan, ANT_SWITCH_DEBUG_MSG, "EXT AntennaDenySwitching=0");                 
                }
                // FIXME: or toggle antenna if antenna mixing is allowed
		if (ant_switch_validate_cmd(antenna)) {
		        if (ant_switch_read_denymixing() == 1) {
		            ant_switch_setantenna(antenna);
                        } else {
		            ant_switch_toggleantenna(antenna);
                        }
		} else {
			ext_send_msg(e->rx_chan, ANT_SWITCH_DEBUG_MSG, "Command not valid SET Antenna=%s",antenna);   
		}
            return true;
        }

        if (strcmp(msg, "GET Antenna") == 0) {
            char *selected_antennas = ant_switch_queryantennas();
            ext_send_msg(e->rx_chan, ANT_SWITCH_DEBUG_MSG, "EXT Antenna=%s", selected_antennas);
            if (ant_switch_read_denyswitching()==true || ant_switch_read_denymultiuser()==true) {
            	ext_send_msg(e->rx_chan, ANT_SWITCH_DEBUG_MSG, "EXT AntennaDenySwitching=1");
	    } else {
            	ext_send_msg(e->rx_chan, ANT_SWITCH_DEBUG_MSG, "EXT AntennaDenySwitching=0");
            }
            if (ant_switch_read_denymixing()==true) {
            	ext_send_msg(e->rx_chan, ANT_SWITCH_DEBUG_MSG, "EXT AntennaDenyMixing=1");
	    } else {
            	ext_send_msg(e->rx_chan, ANT_SWITCH_DEBUG_MSG, "EXT AntennaDenyMixing=0");
            }
            return true;
        }

    	return false;
}


void ant_switch_close(int rx_chan)
{
    // do nothing
}

void ant_switch_main();

ext_t ant_switch_ext = {
	"ant_switch",
	ant_switch_main,
	ant_switch_close,
	ant_switch_msgs,
};

void ant_switch_main()
{
	ext_register(&ant_switch_ext);
}
