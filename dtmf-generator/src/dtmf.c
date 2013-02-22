/*
 * dtmf.c
 *
 *  Created on: 2010-07-04
 *      Author: kamil
 */

#include "dtmf.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

static int* dtmf_window = NULL;
static dtmf_config_t dtmf_config;

int DTMF_Init(dtmf_config_t *config, audio_config_t* audio_config)
{
	if (AUDIO_Init(audio_config))
	{
		return -1;
	}

	memcpy(&dtmf_config, config, sizeof(dtmf_config_t));

	int N = AUDIO_GetSettings()->sample_rate * (dtmf_config.digit_duration_ms / 1000.0);

	if (N < 2)
	{
		return -1;
	}

    dtmf_window = (int*) malloc(N * sizeof(int));

    if (strlen(dtmf_config.window_name) == 0 || !strcmp(dtmf_config.window_name, "none"))
    {
		for (int n = 0; n < N; n++)
		{
			dtmf_window[n] = INT32_MAX;
		}
    }
    else
    if (!strcmp(dtmf_config.window_name, "hann"))
    {
    	for (int n = 0; n < N; n++)
		{
			dtmf_window[n] = INT32_MAX * 0.5 * (1 - cos(2*M_PI*n / (N - 1)));
		}
    }
    else
	if (!strcmp(dtmf_config.window_name, "hamming"))
	{
		for (int n = 0; n < N; n++)
		{
			dtmf_window[n] = INT32_MAX * (0.54 - 0.46*cos(2*M_PI*n / (N - 1)));
		}
	}
	else
	if (!strcmp(dtmf_config.window_name, "triangular"))
	{
		for (int n = 0; n < N; n++)
		{
			dtmf_window[n] = INT32_MAX * ((2.0 / (N - 1)) * ((N - 1)/2.0 - abs(n - (N - 1)/2.0)));
		}
	}
    else
    {
    	return -1;
    }

	return 0;
}

static int dtmf_get_digit_index(char k)
{
	if 		(k >= '0' && k <= '9')	return k - '0';
	else if (k >= 'a' && k <= 'd')	return k - 'a' + 10;
	else if (k >= 'A' && k <= 'D')	return k - 'A' + 10;
	else if (k == '*')				return 14;
	else if (k == '#')				return 15;
	else							return -1;
}

int DTMF_Play(char* phone)
{
	int digits_count = strlen(phone);
	if (digits_count == 0) return 0;

	audio_sound_t* sound;

	int frames_digit_duration_count = AUDIO_GetSettings()->sample_rate * (dtmf_config.digit_duration_ms / 1000.0);
	int frames_per_full_digit_count = AUDIO_GetSettings()->sample_rate * (dtmf_config.digit_duration_ms / 1000.0 + dtmf_config.digit_pause_ms / 1000.0);
	int sound_length = (digits_count - 1)* frames_per_full_digit_count + frames_digit_duration_count;
    sound = (audio_sound_t*) malloc(sound_length * sizeof(audio_sound_t));

    const int freq_low[4] = {697, 770, 852, 941};
    const int freq_high[4] = {1209, 1336, 1477, 1633};

    /* 0 1 2 3 4 5 6 7 8 9 A B C D * # */
    const int freq_digit[16] = {0x13, 0x00, 0x10, 0x20, 0x01, 0x11, 0x21, 0x02, 0x12, 0x22, 0x30, 0x31, 0x32, 0x33, 0x03, 0x23};

    for (int i = 0; i < digits_count; i++)
    {
		int digit = dtmf_get_digit_index(phone[i]);

		double freq_1, freq_2;

		if (digit >= 0)
		{
			freq_1 = freq_low[freq_digit[digit] & 0x0F];
			freq_2 = freq_high[freq_digit[digit] >> 4];
		}
		else
		{
			freq_1 = 0;
			freq_2 = 0;
		}

    	for(int frame = 0; frame < frames_per_full_digit_count; frame++)
		{
    		if (frame < frames_digit_duration_count)
			{
				sound[i*frames_per_full_digit_count + frame].left = dtmf_window[frame] * sin(2*M_PI*frame*freq_1 / AUDIO_GetSettings()->sample_rate);
				sound[i*frames_per_full_digit_count + frame].right = dtmf_window[frame] * sin(2*M_PI*frame*freq_2 / AUDIO_GetSettings()->sample_rate);
			}
			else
			if (i < digits_count - 1)
			{
				sound[i*frames_per_full_digit_count + frame].left = 0;
				sound[i*frames_per_full_digit_count + frame].right = 0;
			}
		}
    }

    AUDIO_Play(sound, sound_length);

    free(sound);

	return 0;
}

int DTMF_Fini(void)
{
	if (dtmf_window != NULL)
		free(dtmf_window);

	if (AUDIO_Fini())
	{
		return -1;
	}

	return 0;
}
