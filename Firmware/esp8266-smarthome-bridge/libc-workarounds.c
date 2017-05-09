/*
* libc_workarounds.c
*
*  Created on: 07.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#define LIBC_WORKAROUNDS_DEBUG

#ifdef LIBC_WORKAROUNDS_DEBUG
#	include <stdio.h>
#	define debug(fmt, ...) printf("%s: " fmt "\n", "LIBC", ## __VA_ARGS__)
#else
#	define debug(fmt, ...)
#endif

/*!
* \brief Pure-virtual workaround.
*
* The libc does not support a default implementation for handling
* possible pure-virtual calls. This is a short workaround for this.
*/
void __cxa_pure_virtual()
{
	debug("%s", "Pure-virtual function was called.");
}