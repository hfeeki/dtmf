/*
 * audio.h
 *
 *  Created on: 2010-07-04
 *      Author: kamil
 */

#ifndef AUDIO_H_
#define AUDIO_H_

struct audio_config_tag
{
	char device_name[256];
	unsigned int sample_rate;
};
typedef struct audio_config_tag audio_config_t;

struct audio_sound_tag
{
	int left;
	int right;
};
typedef struct audio_sound_tag audio_sound_t;

int AUDIO_Init(audio_config_t* config);
int AUDIO_Play(audio_sound_t* sound, int length);
audio_config_t* AUDIO_GetSettings(void);
int AUDIO_Fini(void);

#endif /* AUDIO_H_ */
