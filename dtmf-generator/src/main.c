/*
 ============================================================================
 Name        : main.c
 Author      : Kamil
 Version     :
 Copyright   :
 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "dtmf.h"

int main (int argc, char *argv[])
{
	config_t *config;

	if (CONFIG_Parse(argc, argv))
	{
		exit(EXIT_FAILURE);
	}

	config = CONFIG_Get();

	if (!DTMF_Init(&config->dtmf, &config->audio))
	{
		DTMF_Play(config->key_sequence);
	}
	DTMF_Fini();

	exit(EXIT_SUCCESS);
}
