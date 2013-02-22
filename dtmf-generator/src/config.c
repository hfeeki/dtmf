/*
 * config.c
 *
 *  Created on: 2010-07-05
 *      Author: kamil
 */



#include "config.h"
#include <getopt.h>		/* getopt_long() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static const char short_options[] = "hk:w:d:f:t:";
static const struct option long_options[] =
{
{ "help", no_argument, NULL, 'h' },
{ "keys", required_argument, NULL, 'k' },
{ "window", required_argument, NULL, 'w' },
{ "device", required_argument, NULL, 'd' },
{ "frequency", required_argument, NULL, 'f' },
{ "time", required_argument, NULL, 't' },
{ 0, 0, 0, 0 } };



static config_t config;



static void config_usage(FILE * fp, int argc, char **argv)
{
	fprintf(fp, "Usage: %s [options]\n\n"
		"Options:\n"
		"-h | --help               Prints this message\n"
		"-k | --keys KEYS_SEQUENCE Generates DTMF tones for given keys sequence\n"
		"-w | --window WINDOW      Uses window: none|hann|hamming|triangular\n"
		"                          DEFAULT: none\n"
		"-d | --device DEVICE      Uses audio device DEVICE\n"
		"                          DEFAULT: plughw:0,0\n"
		"-f | --frequency F        Sample rate [Hz]\n"
		"                          DEFAULT: 4096\n"
		"-t | --time T             Pulse and pause duration time [ms]\n"
		"                          DEFAULT: 64\n"
		"", argv[0]);
}



int CONFIG_Parse(int argc, char *argv[])
{
	strcpy(config.key_sequence, "");

	strcpy(config.audio.device_name, "plughw:0,0");
	config.audio.sample_rate = 4096;

	config.dtmf.digit_duration_ms = 64;
	config.dtmf.digit_pause_ms = 64;
	strcpy(config.dtmf.window_name, "");

	for (;;)
	{
		int index;
		int c;

		c = getopt_long(argc, argv, short_options, long_options, &index);

		if (-1 == c)
			break;

		switch (c)
		{
		case 0: /* getopt_long() flag */
			break;

		case 'h':
			config_usage(stdout, argc, argv);
			return 0;
			break;

		case 'k':
			if (sscanf(optarg, "%s", config.key_sequence) != 1)
			{
				fprintf(stderr,"Bad key sequence specified {%s}\n", optarg);
				config_usage(stderr, argc, argv);
				return -1;
			}
			break;

		case 'w':
			if (sscanf(optarg, "%s", config.dtmf.window_name) != 1 ||
				(	strcmp(config.dtmf.window_name, "none") &&
					strcmp(config.dtmf.window_name, "hann") &&
					strcmp(config.dtmf.window_name, "hamming") &&
					strcmp(config.dtmf.window_name, "triangular")	))
			{
				fprintf(stderr,"Bad window name specified {%s}\n", optarg);
				config_usage(stderr, argc, argv);
				return -1;
			}
			break;

		case 'd':
			if (sscanf(optarg, "%s", config.audio.device_name) != 1)
			{
				fprintf(stderr,"Bad device name specified {%s}\n", optarg);
				config_usage(stderr, argc, argv);
				return -1;
			}
			break;

		case 'f':
			if (sscanf(optarg, "%u", &config.audio.sample_rate) != 1)
			{
				fprintf(stderr,"Bad sample rate specified {%s}\n", optarg);
				config_usage(stderr, argc, argv);
				return -1;
			}
			break;

		case 't':
			if (sscanf(optarg, "%u", &config.dtmf.digit_duration_ms) != 1)
			{
				fprintf(stderr,"Bad sample rate specified {%s}\n", optarg);
				config_usage(stderr, argc, argv);
				return -1;
			}
			config.dtmf.digit_pause_ms = config.dtmf.digit_duration_ms;
			break;

		default:
			config_usage(stderr, argc, argv);
			return -1;
		}
	}

	if (strlen(config.key_sequence) == 0)
	{
		fprintf(stderr,"Pass key sequence (use --help for more info)\n");
		return -1;
	}

	return 0;
}



config_t* CONFIG_Get(void)
{
	return &config;
}
