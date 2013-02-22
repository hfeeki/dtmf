/*
 * audio.c
 *
 *  Created on: 2010-07-04
 *      Author: kamil
 */

#include "audio.h"
#include <alsa/asoundlib.h>

static snd_pcm_t *audio_playback_handle = NULL;
static audio_config_t audio_config;

int AUDIO_Init(audio_config_t* config)
{
	int err;
	snd_pcm_hw_params_t *hw_params;

	memcpy(&audio_config, config, sizeof(audio_config));

	for (;;)
	{
		if ((err = snd_pcm_open(&audio_playback_handle, audio_config.device_name,
				SND_PCM_STREAM_PLAYBACK, 0)) < 0)
		{
			fprintf(stderr, "Audio device error: cannot open %s (%s)\n", audio_config.device_name,
					snd_strerror(err));
			break;
		}

		if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
		{
			fprintf(stderr,
					"Audio device error: cannot allocate hardware parameter structure (%s)\n",
					snd_strerror(err));
			break;
		}

		if ((err = snd_pcm_hw_params_any(audio_playback_handle, hw_params)) < 0)
		{
			fprintf(stderr,
					"Audio device error: cannot initialize hardware parameter structure (%s)\n",
					snd_strerror(err));
			break;
		}

		if ((err = snd_pcm_hw_params_set_access(audio_playback_handle,
				hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
		{
			fprintf(stderr, "Audio device error: cannot set access type (%s)\n", snd_strerror(err));
			break;
		}

		if ((err = snd_pcm_hw_params_set_format(audio_playback_handle,
				hw_params, SND_PCM_FORMAT_S32_LE)) < 0)
		{
			fprintf(stderr, "Audio device error: cannot set sample format (%s)\n",
					snd_strerror(err));
			break;
		}

		//FIXME: doesn't seem to work properly for other values than default
		if ((err = snd_pcm_hw_params_set_rate_near(audio_playback_handle,
				hw_params, &audio_config.sample_rate, 0)) < 0)
		{
			fprintf(stderr, "Audio device error: cannot set sample rate (%s)\n", snd_strerror(err));
			break;
		}

		if ((err = snd_pcm_hw_params_set_channels(audio_playback_handle,
				hw_params, 2)) < 0)
		{
			fprintf(stderr, "Audio device error: cannot set channels count (%s)\n",
					snd_strerror(err));
			break;
		}

		if ((err = snd_pcm_hw_params(audio_playback_handle, hw_params)) < 0)
		{
			fprintf(stderr, "Audio device error: cannot set parameters (%s)\n", snd_strerror(err));
			break;
		}

		snd_pcm_hw_params_free(hw_params);

		return 0;
	}

	audio_playback_handle = NULL;
	return -1;
}

int AUDIO_Play(audio_sound_t* sound, int length)
{
	if (audio_playback_handle == NULL) return -1;
	if (length == 0) return -1;

	snd_pcm_drain(audio_playback_handle);

	int err;
	if ((err = snd_pcm_prepare (audio_playback_handle)) < 0) {
		fprintf (stderr, "Audio device error: cannot prepare audio interface for use (%s)\n",
			 snd_strerror (err));
		return -1;
	}

	if (snd_pcm_writei(audio_playback_handle, sound, length) != length)
	{
		fprintf (stderr, "Audio device error: cannot write to audio device (%s)\n",
			 snd_strerror (err));
		return -1;
	}

	return 0;
}

audio_config_t* AUDIO_GetSettings(void)
{
	return &audio_config;
}

int AUDIO_Fini(void)
{
	if (audio_playback_handle == NULL) return -1;

	snd_pcm_drain(audio_playback_handle);
	snd_pcm_close (audio_playback_handle);

	return 0;
}

