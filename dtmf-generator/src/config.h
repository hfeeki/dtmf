/*
 * config.h
 *
 *  Created on: 2010-07-05
 *      Author: kamil
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "dtmf.h"
#include "audio.h"

struct config_tag
{
	char key_sequence[256];
	dtmf_config_t dtmf;
	audio_config_t audio;
};
typedef struct config_tag config_t;

int CONFIG_Parse(int argc, char *argv[]);
config_t* CONFIG_Get(void);

#endif /* CONFIG_H_ */
