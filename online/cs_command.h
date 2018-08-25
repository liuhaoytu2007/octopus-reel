/*      Octopus - Bioelectromagnetic Source Localization System 0.9.9      *
 *                   (>) GPL 2007-2009,2018- Barkin Ilhan                  *
 *            barkin@unrlabs.org, http://www.unrlabs.org/barkin            */

/* Commands and other constants related with client-server communication. */

#ifndef CS_COMMAND_H
#define CS_COMMAND_H

#define CS_STIM_SET_PARADIGM		(0x0001)

#define CS_STIM_LOAD_PATTERN_SYN	(0x1001)
#define CS_STIM_LOAD_PATTERN_ACK	(0x1002)

#define CS_STIM_START			(0x2001)
#define CS_STIM_STOP			(0x2002)
#define CS_STIM_PAUSE			(0x2003)
#define CS_TRIG_START			(0x2004)
#define CS_TRIG_STOP			(0x2005)

#define CS_STIM_SET_IIDITD_T1		(0x1101)
#define CS_STIM_SET_IIDITD_T2		(0x1102)
#define CS_STIM_SET_IIDITD_T3		(0x1103)

#define CS_STIM_SYNTHETIC_EVENT		(0x3001)

#define CS_ACQ_INFO			(0x0001)
#define CS_ACQ_INFO_RESULT		(0x0002)
#define CS_ACQ_TRIGTEST			(0x1001)

#define CS_REBOOT			(0xFFFE)
#define CS_SHUTDOWN			(0xFFFF)

typedef struct _cs_command { /* Client-server communication structure. */
 unsigned short cmd; int iparam[3]; float fparam[3];
} cs_command;

#endif
