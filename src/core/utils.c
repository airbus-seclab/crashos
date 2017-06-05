// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
/*
 * utils.c
 *
 *  Created on: 11 juil. 2016
 *      Author: anais
 */

/**
 * Sleep function
 */
void sleep() {
	unsigned int i;
	for (i=0; i<0xffffffff; i++); //sleep()
}
