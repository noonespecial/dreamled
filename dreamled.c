/*#############################################################################
# Title   : dedupe                                                            #
# Version : 0.1                                                               #
# Date    : Mar, 20 2011                                                      #
# Desc.   : Control the LED's on a Dreamplug/Guruplug                         #
# Author  : Jon Beckmann                                                      #
# Liscense: BSD/Apache                                                        # 
#############################################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
  
#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)
 
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

// MAGIC NUMBERS! These are the mem-mapped LED IO locations
// for Globalscale Sheeva, Guru, and Dream plug computers.
#define AP_TARGET          0xf1010142
#define WLAN_TARGET        0xf1010142
#define BT_TARGET          0xf1010141
#define AP_BLINK_TARGET    0xf101014A
#define WLAN_BLINK_TARGET  0xf101014A
#define BT_BLINK_TARGET    0xf1010149
#define AP_BITMASK         0x02
#define WLAN_BITMASK       0x01
#define BT_BITMASK         0x80

//################################# How to use ################################
void usage(char *input) {
 fprintf(stderr, "\nUsage:\t%s (ap|wlan|bt|all) [on|off|blink]\n"
                        "\tNote    : You must specify a valid light: ap, wlan, or bt.\n"
			"\tNote    : If no action is specified, %s will return the current state.\n"
			"\tNote    : Blink supercedes both on and off, making the light come on blinking.\n\n",input,input);
		exit(1);
}

//################################# Read a value ##############################
unsigned long get_val(off_t target) {
 int fd;
 void *map_base, *virt_addr; 
 unsigned long read_result;
  if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
   map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
    if (map_base == (void *) -1) FATAL;
    virt_addr = map_base + (target & MAP_MASK);
    read_result = *((unsigned char *) virt_addr);
   if(munmap(map_base, MAP_SIZE) == -1) FATAL;
  close(fd);
 return read_result;
}

//########################## Write a value ####################################
unsigned long put_val(off_t target, unsigned long writeval) {
 int fd;
 void *map_base, *virt_addr; 
  if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
   map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
    if (map_base == (void *) -1) FATAL;
    virt_addr = map_base + (target & MAP_MASK);
    *((unsigned char *) virt_addr) = writeval;
   if(munmap(map_base, MAP_SIZE) == -1) FATAL;
  close(fd);
}

//################################## Main #####################################
int main(int argc, char **argv) {

	unsigned long readval, writeval;
	off_t target;
        char *state;
        char c=0;
	char a=3;
        int temp = argc;

//Hack so I can use switch instead of if else... sorry.
if(argc > 1){
 if (strcmp ("ap",   argv[1]) ==0) {c='a';}
 if (strcmp ("wlan", argv[1]) ==0) {c='w';}
 if (strcmp ("bt",   argv[1]) ==0) {c='b';}
 if (strcmp ("all",  argv[1]) ==0) {c='l';}
}
 if(argc > 2){
  if (strcmp ("off",  argv[2]) ==0) {a=0;}
  if (strcmp ("on",   argv[2]) ==0) {a=1;}
  if (strcmp ("blink",argv[2]) ==0) {a=2;}
 }

switch(c) {
// The AP LED
 case 'a':
  if (a < 3) {
   readval=get_val(AP_BLINK_TARGET);
   writeval=(readval & ~AP_BITMASK);
   put_val(AP_BLINK_TARGET,writeval);
  }
  switch(a) {
   case 0:
    readval=get_val(AP_TARGET);
    writeval=(readval | AP_BITMASK);
    put_val(AP_TARGET,writeval);
   break;
   case 1:
    readval=get_val(AP_TARGET);
    writeval=(readval & ~AP_BITMASK);
    put_val(AP_TARGET,writeval);
   break;
   case 2:
    readval=get_val(AP_BLINK_TARGET);
    writeval=(readval | AP_BITMASK);
    put_val(AP_BLINK_TARGET,writeval);
   break;
  }
  readval = get_val(AP_BLINK_TARGET);
  if (readval & AP_BITMASK) {state = "blinking";}
  else {
   readval = get_val(AP_TARGET);
   if (readval & AP_BITMASK) {state = "off";} else {state = "on";}
  }
  break;
  
//The Wlan LED
 case 'w':
  if (a < 3) {
   readval=get_val(WLAN_BLINK_TARGET);
   writeval=(readval & ~WLAN_BITMASK);
   put_val(WLAN_BLINK_TARGET,writeval);
  }
  switch(a) {
   case 0:
    readval=get_val(WLAN_TARGET);
    writeval=(readval | WLAN_BITMASK);
    put_val(WLAN_TARGET,writeval);
   break;
   case 1:
    readval=get_val(WLAN_TARGET);
    writeval=(readval & ~WLAN_BITMASK);
    put_val(WLAN_TARGET,writeval);
   break;
   case 2:
    readval=get_val(WLAN_BLINK_TARGET);
    writeval=(readval | WLAN_BITMASK);
    put_val(WLAN_BLINK_TARGET,writeval);
   break;
  }
  readval = get_val(WLAN_BLINK_TARGET);
  if (readval & WLAN_BITMASK) {state = "blinking";}
  else {
   readval = get_val(WLAN_TARGET);
   if (readval & WLAN_BITMASK) {state = "off";} else {state = "on";}
  }
  break;

//The Bluetooth LED
 case 'b':
  if (a < 3) {
   readval=get_val(BT_BLINK_TARGET);
   writeval=(readval & ~BT_BITMASK);
   put_val(BT_BLINK_TARGET,writeval);
  }
  switch(a) {
   case 0:
    readval=get_val(BT_TARGET);
    writeval=(readval | BT_BITMASK);
    put_val(BT_TARGET,writeval);
   break;
   case 1:
    readval=get_val(BT_TARGET);
    writeval=(readval & ~BT_BITMASK);
    put_val(BT_TARGET,writeval);
   break;
   case 2:
    readval=get_val(BT_BLINK_TARGET);
    writeval=(readval | BT_BITMASK);
    put_val(BT_BLINK_TARGET,writeval);
   break;
  }
  readval = get_val(BT_BLINK_TARGET);
  if (readval & BT_BITMASK) {state = "blinking";}
  else {
   readval = get_val(BT_TARGET);
   if (readval & BT_BITMASK) {state = "off";} else {state = "on";}
  }
  break;
  
//All At Once
 case 'l':
  if (a < 3) {
   readval=get_val(AP_BLINK_TARGET);
    writeval=(readval & ~AP_BITMASK);
    put_val(AP_BLINK_TARGET,writeval);
   readval=get_val(WLAN_BLINK_TARGET);
    writeval=(readval & ~WLAN_BITMASK);
    put_val(WLAN_BLINK_TARGET,writeval);
   readval=get_val(BT_BLINK_TARGET);
    writeval=(readval & ~BT_BITMASK);
    put_val(BT_BLINK_TARGET,writeval);
  }
  switch(a) {
   case 0:
    readval=get_val(AP_TARGET);
     writeval=(readval | AP_BITMASK);
     put_val(AP_TARGET,writeval);
    readval=get_val(WLAN_TARGET);
     writeval=(readval | WLAN_BITMASK);
     put_val(WLAN_TARGET,writeval); 
    readval=get_val(BT_TARGET);
     writeval=(readval | BT_BITMASK);
     put_val(BT_TARGET,writeval);
   break;
   case 1:
    readval=get_val(AP_TARGET);
     writeval=(readval & ~AP_BITMASK);
     put_val(AP_TARGET,writeval);
    readval=get_val(WLAN_TARGET);
     writeval=(readval & ~WLAN_BITMASK);
     put_val(WLAN_TARGET,writeval); 
    readval=get_val(BT_TARGET);
     writeval=(readval & ~BT_BITMASK);
     put_val(BT_TARGET,writeval);
   break;
   case 2:
    readval=get_val(AP_BLINK_TARGET);
     writeval=(readval | AP_BITMASK);
     put_val(AP_BLINK_TARGET,writeval);
    readval=get_val(WLAN_BLINK_TARGET);
     writeval=(readval | WLAN_BITMASK);
     put_val(WLAN_BLINK_TARGET,writeval);
    readval=get_val(BT_BLINK_TARGET);
     writeval=(readval | BT_BITMASK);
     put_val(BT_BLINK_TARGET,writeval);
   break;
  }
  state = "";
  break;

 default:
 usage(argv[0]);
}

 printf("%s\n",state);

 return 0;
}

