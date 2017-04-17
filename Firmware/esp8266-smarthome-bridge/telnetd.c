/*
 * telnetd.c
 *
 *  Created on: 17.04.2017
 *      Author: Björn Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#include "telnetd.h"

#include <lwip/debug.h>
#include <lwip/stats.h>
#include <lwip/tcp.h>

#include <string.h>
#include <stdlib.h>

#if LWIP_TCP

#ifndef TELNETD_DEBUG
#define TELNETD_DEBUG           LWIP_DBG_LEVEL_ALL
#endif

/** Maximum retries before the connection is aborted/closed.
 * - number of times pcb->poll is called -> default is 4*500ms = 2s;
 * - reset when pcb->sent is called
 */
#ifndef TELNETD_MAX_RETRIES
#define TELNETD_MAX_RETRIES     4
#endif

/** Priority for tcp pcbs created by HTTPD (very low by default).
 *  Lower priorities get killed first when running out of memroy.
 */
#ifndef TELNETD_TCP_PRIO
#define TELNETD_TCP_PRIO        TCP_PRIO_MIN
#endif

/**
 * Connection pool
 */
static uint8_t telnetd_client_txbuffers[TELNETD_MAX_CONN][TELNETD_MAX_TXBUFFER];
static struct telnet_client_state telnet_client_states[TELNETD_MAX_CONN];

/**
 * Find telnet client state from given tcp socket.
 */
static struct telnet_client_state *telnetd_find_telnet_client_state(struct tcp_pcb *pcb)
{
	for (int i = 0; i < TELNETD_MAX_CONN; i++)
	{
		if (telnet_client_states[i].pcb == pcb)
		{
			return &telnet_client_states[i];
		}
	}

	return NULL;
}

/**
 * Prototypes
 */
static err_t telnet_close_conn(struct tcp_pcb *pcb, struct telnet_client_state *tcs);
static err_t telnet_close_or_abort_conn(struct tcp_pcb *pcb, struct telnet_client_state *tcs, uint8_t abort_conn);
static err_t telnet_send(struct tcp_pcb *pcb, struct telnet_client_state *tcs);
static err_t telnet_poll(void *arg, struct tcp_pcb *pcb);

/**
 * Callback functions
 */
static tTcHandler telnetd_cb = NULL;
static tTcOpenHandler telnetd_open_cb = NULL;

void telnetd_register_callbacks(tTcOpenHandler tc_open_cb, tTcHandler tc_cb)
{
  telnetd_open_cb = tc_open_cb;
  telnetd_cb = tc_cb;
}

err_t telnetd_client_write(struct tcp_pcb *pcb, const uint8_t *data, uint16_t len)
{
	// Find telnet client state that uses tcp socket
	struct telnet_client_state *tcs = telnetd_find_telnet_client_state(pcb);
	if (tcs == NULL)
	{
		return ERR_CONN;
	}

	// Check if buffer could be copied
	if (tcs->txbufferlen + len > TELNETD_MAX_TXBUFFER)
	{
		LWIP_DEBUGF(HTTPD_DEBUG, ("telnetd_client_write: txbuffer full on telnet_client_state %p\n", tcs));
		return ERR_MEM;
	}

	// Copy data into txbuffer
	memcpy(tcs->txbuffer + tcs->txbufferlen, data, len);
	tcs->txbufferlen += len;

	// Check if data could be send immediate
	if (tcs->readytosend)
	{
		telnet_send(pcb, tcs);
	}

	return ERR_OK;
}

/** Call tcp_write() in a loop trying smaller and smaller length
 *
 * @param pcb tcp_pcb to send
 * @param ptr Data to send
 * @param length Length of data to send (in/out: on return, contains the
 *        amount of data sent)
 * @param apiflags directly passed to tcp_write
 * @return the return value of tcp_write
 */
static err_t telnet_write(struct tcp_pcb *pcb, const void* dataPtr, uint16_t *length, uint8_t apiflags)
{
	uint16_t len;
	err_t err;

	LWIP_ASSERT("length != NULL", length != NULL);
	len = *length;
	if (len == 0)
	{
		return ERR_OK;
	}

	do
	{
		LWIP_DEBUGF(TELNETD_DEBUG | LWIP_DBG_TRACE, ("Trying to send %d bytes\n", len));
		err = tcp_write(pcb, dataPtr, len, apiflags);
		if (err == ERR_MEM)
		{
			if ((tcp_sndbuf(pcb) == 0) || (tcp_sndqueuelen(pcb) >= TCP_SND_QUEUELEN))
			{
				// No need to try smaller sizes
				len = 1;
			}
			else
			{
				// Try with half size
				len /= 2;
			}
			LWIP_DEBUGF(TELNETD_DEBUG | LWIP_DBG_TRACE, ("Send failed, trying less (%d bytes)\n", len));
		}
	} while ((err == ERR_MEM) && (len > 1));

   if (err == ERR_OK)
   {
	   LWIP_DEBUGF(TELNETD_DEBUG | LWIP_DBG_TRACE, ("Sent %d bytes\n", len));
   }
   else
   {
	   LWIP_DEBUGF(TELNETD_DEBUG | LWIP_DBG_TRACE, ("Send failed with err %d (\"%s\")\n", err, lwip_strerr(err)));
   }

   // Give bytes sent back to client
   *length = len;

   return err;
}

/**
 * Try to send more data on this pcb.
 *
 * @param pcb the pcb to send data
 * @param hs connection state
 */
static err_t telnet_send(struct tcp_pcb *pcb, struct telnet_client_state *tcs)
{
	LWIP_DEBUGF(TELNETD_DEBUG | LWIP_DBG_TRACE, ("telnet_send: pcb=%p tcs=%p txbufferleft=%d\n",
		(void*)pcb, (void*)tcs, tcs != NULL ? (int)(tcs->txbufferlen - tcs->txbuffersent) : 0));

	uint16_t len, mss;
	uint16_t left = tcs->txbufferlen - tcs->txbuffersent;
	err_t err = ERR_OK;

	/* Check if data to send is available. */
	if (tcs->txbufferlen != 0)
	{
		tcs->readytosend = false;

		/* We cannot send more data than space available in the send buffer. */
		if (tcp_sndbuf(pcb) < left)
		{
			len = tcp_sndbuf(pcb);
		}
		else
		{
			len = left;
		}

		/* We cannot send more data than double maximum segment size. */
		mss = tcp_mss(pcb);
		if (len > (2 * mss))
		{
			len = 2 * mss;
		}

		/* Write data to the telnet client socket */
		err = telnet_write(pcb, tcs->txbuffer + tcs->txbuffersent, &len, TCP_WRITE_FLAG_COPY);
		if (err == ERR_OK)
		{
			tcs->txbuffersent += len;
		}

		/* Check if all data where send out */
		if (tcs->txbufferlen == tcs->txbuffersent)
		{
			tcs->txbufferlen = 0;
			tcs->txbuffersent = 0;
		}
	}

	return err;
}

/**
 * Data has been received on this pcb.
 */
static err_t telnet_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	struct telnet_client_state *tcs = (struct telnet_client_state *)arg;
	LWIP_DEBUGF(TELNETD_DEBUG | LWIP_DBG_TRACE, ("telnet_recv: pcb=%p pbuf=%p err=%s\n",
		(void*)pcb, (void*)p, lwip_strerr(err)));

	if (tcs != NULL)
	{
		if (p == NULL)
		{
			LWIP_DEBUGF(TELNETD_DEBUG, ("telnet_recv: buffer error\n"));
			telnet_close_or_abort_conn(pcb, tcs, 0);
			return ERR_BUF;
	    }

	    tcp_recved(pcb, p->tot_len);
	    telnetd_cb(pcb, (uint8_t *)p->payload, p->len);
	    if (p != NULL)
	    {
	    	/* otherwise tcp buffer hogs */
	    	LWIP_DEBUGF(TELNETD_DEBUG, ("telnet_recv: freeing buffer\n"));
	    	pbuf_free(p);
	    }

	    /* reset timeout */
	    tcs->retries = 0;
	    return ERR_OK;
	  }

	  if ((p == NULL) || (tcs == NULL))
	  {
		  /* error or closed by other side? */
		  if (p != NULL)
		  {
			  /* Inform TCP that we have taken the data. */
			  tcp_recved(pcb, p->tot_len);
			  pbuf_free(p);
		  }

		  /* this should not happen, only to be robust */
		  if (tcs == NULL)
		  {
			  LWIP_DEBUGF(TELNETD_DEBUG, ("Error, http_recv: tcs is NULL, close\n"));
		  }

		  telnet_close_conn(pcb, tcs);
		  return ERR_OK;
	  }

	  return ERR_OK;
}

/**
 * The connection shall be actively closed (using RST to close from fault states).
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param tcs telnet client state to free
 */
static err_t telnet_close_or_abort_conn(struct tcp_pcb *pcb, struct telnet_client_state *tcs, uint8_t abort_conn)
{
	err_t err;
	LWIP_DEBUGF(TELNETD_DEBUG, ("Closing connection %p\n", (void*)pcb));

	tcp_arg(pcb, NULL);
	tcp_recv(pcb, NULL);
	tcp_err(pcb, NULL);
	tcp_poll(pcb, NULL, 0);
	tcp_sent(pcb, NULL);

	if (tcs != NULL)
	{
		tcs->pcb = NULL;
		tcs->txbufferlen = 0;
		tcs->txbuffersent = 0;
		tcs->readytosend = true;
		tcs->retries = 0;
	}

	if (abort_conn)
	{
		tcp_abort(pcb);
		return ERR_OK;
	}

	err = tcp_close(pcb);
	if (err != ERR_OK)
	{
		LWIP_DEBUGF(TELNETD_DEBUG, ("Error %d closing %p\n", err, (void*)pcb));

		/* error closing, try again later in poll */
		tcp_poll(pcb, telnet_poll, TELNETD_MAX_RETRIES);
	}

	return err;
}

/**
 * The connection shall be actively closed.
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param tcs telnet client state to free
 */
static err_t telnet_close_conn(struct tcp_pcb *pcb, struct telnet_client_state *tcs)
{
	return telnet_close_or_abort_conn(pcb, tcs, 0);
}

/**
 * The pcb had an error and is already deallocated.
 * The argument might still be valid (if != NULL).
 */
static void telnet_err(void *arg, err_t err)
{
	struct telnet_client_state *tcs = (struct telnet_client_state *)arg;
	LWIP_UNUSED_ARG(err);

	LWIP_DEBUGF(TELNETD_DEBUG, ("http_err: %s", lwip_strerr(err)));

	// Reset telnet client state
	if (tcs != NULL)
	{
		tcs->pcb = NULL;
		tcs->txbufferlen = 0;
		tcs->txbuffersent = 0;
		tcs->readytosend = true;
		tcs->retries = 0;
	}
}

/**
 * Data has been sent and acknowledged by the remote host.
 * This means that more data can be sent.
 */
static err_t telnet_sent(void *arg, struct tcp_pcb *pcb, uint16_t len)
{
	struct telnet_client_state *tcs = (struct telnet_client_state *)arg;

	LWIP_DEBUGF(TELNETD_DEBUG | LWIP_DBG_TRACE, ("http_sent %p\n", (void*)pcb));
	LWIP_UNUSED_ARG(len);

	if (tcs == NULL)
	{
		return ERR_OK;
	}

	tcs->readytosend = true;
	tcs->retries = 0;

	telnet_send(pcb, tcs);

	return ERR_OK;
}

/**
 * The poll function is called every 2nd second.
 * If there has been no data sent (which resets the retries) in 8 seconds, close.
 * If the last portion of a file has not been sent in 2 seconds, close.
 *
 * This could be increased, but we don't want to waste resources for bad connections.
 */
static err_t telnet_poll(void *arg, struct tcp_pcb *pcb)
{
	struct telnet_client_state *tcs = (struct telnet_client_state *)arg;
	LWIP_DEBUGF(TELNETD_DEBUG | LWIP_DBG_TRACE, ("telnet_poll: pcb=%p tcs=%p pcb_state=%s\n",
		(void*)pcb, (void*)tcs, tcp_debug_state_str(pcb->state)));

	if (tcs == NULL)
	{
		LWIP_DEBUGF(HTTPD_DEBUG, ("telnet_poll: arg is NULL, close\n"));
		telnet_close_conn(pcb, NULL);

		return ERR_OK;
	}
	else
	{
		tcs->retries++;
		if (tcs->retries == TELNETD_MAX_RETRIES)
		{
			LWIP_DEBUGF(HTTPD_DEBUG, ("telnet_poll: too many retries, close\n"));
			telnet_close_conn(pcb, tcs);

			return ERR_OK;
		}

		if(tcs)
		{
			LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_poll: try to send more data\n"));
			if(telnet_send(pcb, tcs) == ERR_OK)
			{
				/* If we wrote anything to be sent, go ahead and send it now. */
				LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("tcp_output\n"));
				tcp_output(pcb);
			}
		}
	}

	return ERR_OK;
}

/**
 * A new incoming connection has been accepted.
 */
static err_t telnet_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
	int i = 0;
	struct telnet_client_state *tcs;
	struct tcp_pcb_listen *lpcb = (struct tcp_pcb_listen*)arg;
	LWIP_UNUSED_ARG(err);
	LWIP_DEBUGF(HTTPD_DEBUG, ("telnet_accept %p / %p\n", (void*)pcb, arg));

	/* Decrease the listen backlog counter */
	tcp_accepted(lpcb);

	/* Set priority */
	tcp_setprio(pcb, TELNETD_TCP_PRIO);

	/* Find free structure that holds the state of the
       connection - initialized by that function. */
	for (i = 0; i < TELNETD_MAX_CONN; i++) if (telnet_client_states[i].pcb == NULL) break;
	if (i == TELNETD_MAX_CONN)
	{
		LWIP_DEBUGF(HTTPD_DEBUG, ("http_accept: Out of memory, RST\n"));
		return ERR_MEM;
	}
	else
	{
		tcs = &telnet_client_states[i];
		tcs->pcb = pcb;
	}
	LWIP_DEBUGF(HTTPD_DEBUG, ("http_accept: Using pool slot %d\n", i));

	/* Tell TCP that this is the structure we
	 * wish to be passed for our callbacks.
	 */
	tcp_arg(pcb, tcs);

	/* Set up the various callback functions */
	tcp_recv(pcb, telnet_recv);
	tcp_err(pcb, telnet_err);
	tcp_sent(pcb, telnet_sent);

	return ERR_OK;
}

/**
 * Initialize the telnetd with the specified local address.
 */
static void telnetd_init_addr(ip_addr_t *local_addr, uint16_t port)
{
	struct tcp_pcb *pcb;
	err_t err;

	pcb = tcp_new();
	LWIP_ASSERT("telnetd_init: tcp_new failed", pcb != NULL);
	tcp_setprio(pcb, TELNETD_TCP_PRIO);

	/* set SOF_REUSEADDR here to explicitly bind telnetd to multiple interfaces */
	err = tcp_bind(pcb, local_addr, port);
	LWIP_ASSERT("telnetd_init: tcp_bind failed", err == ERR_OK);
	pcb = tcp_listen(pcb);
	LWIP_ASSERT("telnetd_init: tcp_listen failed", pcb != NULL);

	/* initialize callback arg and accept callback */
	tcp_arg(pcb, pcb);
	tcp_accept(pcb, telnet_accept);
}

/**
 * Initialize the telnetd: set up a listening PCB and bind it to the defined port
 */
void telnetd_init(uint16_t port)
{
	int i = 0;
	LWIP_DEBUGF(HTTPD_DEBUG, ("telnetd_init\n"));

	// Initialize telnetd client states
	for (i = 0; i < TELNETD_MAX_CONN; i++)
	{
		telnet_client_states[i].pcb = NULL;
		telnet_client_states[i].txbuffer = telnetd_client_txbuffers[i];
		telnet_client_states[i].txbufferlen = 0;
		telnet_client_states[i].txbuffersent = 0;
		telnet_client_states[i].readytosend = true;
		telnet_client_states[i].retries = 0;
	}

	telnetd_init_addr(IP_ADDR_ANY, port);
}

#endif /* LWIP_TCP */
