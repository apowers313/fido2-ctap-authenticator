#include <stdio.h>
#include <unistd.h> // for sleep()
#include <stdlib.h> // for malloc() & free()
#include <string.h> // for strlen()
// BSD Queues: just a bunch of macros for linked lists; should be broadly available and / or easily portable
// XXX - not sure that BSD Queues are thread safe...
#include <sys/queue.h>

static void processCborRequest(unsigned char *msg);
struct msgQueue {
	unsigned char *cborMsg;
	STAILQ_ENTRY(msgQueue) list;
};

STAILQ_HEAD(msgQueueHead, msgQueue) msg_queue_head;

int init(void *param) {
	// param is ignored
	// printf ("Doing init\n");
	STAILQ_INIT(&msg_queue_head);
	// sleep(1);
	return 0;
}

int shutdown(void *param) {
	// param is ignored
	// printf ("Doing shutdown\n");
	return 0;
}

int sendCborMessage(unsigned char *cborBuf, int len) {
	printf ("Sending CBOR message, len %d\n", len);
	processCborRequest (cborBuf);
	return 0;
}

unsigned char *receiveCborMessage(unsigned int *len) {
	unsigned char *cborBuf = NULL;
	// if (cborBuf == NULL || *cborBuf == NULL) {
	// 	printf ("receiveCborMessage: invalid buffer reference\n");
	// 	return -1; // TODO: define error messages
	// }

	if (len == NULL) {
		printf ("receiveCborMessage: invalid length reference\n");
		return NULL; // TODO: define error messages
	}
	*len = 0;

	while (STAILQ_EMPTY (&msg_queue_head)) {
		printf ("receiveCborMessage waiting for message...\n");
		sleep(1);
	}

	struct msgQueue *newMsg;
	newMsg	= STAILQ_FIRST(&msg_queue_head);
	STAILQ_REMOVE_HEAD(&msg_queue_head, list);
	printf ("receiveCborMessage got message: \"%s\"\n", newMsg->cborMsg);
	cborBuf = newMsg->cborMsg;
	free(newMsg);

	*len = strlen ((char *)cborBuf);
	return cborBuf;
}

static void processCborRequest(unsigned char *msg) {
	printf ("msg is: %s\n", msg);
	struct msgQueue* response = (struct msgQueue *)malloc (sizeof (struct msgQueue));
	response->cborMsg = malloc(3);
	response->cborMsg[0] = 'h';
	response->cborMsg[1] = 'i';
	response->cborMsg[2] = 0;
	STAILQ_INSERT_TAIL(&msg_queue_head, response, list);
}