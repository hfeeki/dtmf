/*
 * dtmf.h
 *
 *  Created on: 2010-07-04
 *      Author: kamil
 */

#ifndef DTMF_H_
#define DTMF_H_

#include "audio.h"

struct dtmf_config_tag
{
	unsigned int digit_duration_ms;
	unsigned int digit_pause_ms;
	char window_name[256];
};
typedef struct dtmf_config_tag dtmf_config_t;

int DTMF_Init(dtmf_config_t *config, audio_config_t* audio_config);
int DTMF_Play(char* phone);
int DTMF_Fini(void);

#endif /* DTMF_H_ */
