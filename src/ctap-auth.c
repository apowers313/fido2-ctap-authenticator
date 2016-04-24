#include <stdio.h>
#include <unistd.h> // for sleep()
#include <stdlib.h> // for malloc() & free()
#include <string.h> // for strlen()
// BSD Queues: just a bunch of macros for linked lists; should be broadly available and / or easily portable
// XXX - not sure that BSD Queues are thread safe...
#include <sys/queue.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "cbor.h"

static void processCborRequest(unsigned char *msg, unsigned int len);
static void parseMakeCredentialRequest(unsigned char *msg, unsigned int len);
struct msgQueue {
	unsigned char *cborMsg;
	int len;
	STAILQ_ENTRY(msgQueue) list;
};

STAILQ_HEAD(msgQueueHead, msgQueue) msg_queue_head;

int init(void *param) {
	// param is ignored
	// printf ("Doing init\n");
	STAILQ_INIT(&msg_queue_head);
	// sleep(1);

	/* Load the human readable error strings for libcrypto */
	ERR_load_crypto_strings();
	/* Load all digest and cipher algorithms */
	OpenSSL_add_all_algorithms();
	/* Load config file, and other important initialisation */
	OPENSSL_config(NULL);

	return 0;
}

int shutdown(void *param) {
	// param is ignored
	// printf ("Doing shutdown\n");

	/* Removes all digests and ciphers */
	EVP_cleanup();
	/* if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations */
	CRYPTO_cleanup_all_ex_data();
	/* Remove error strings */
	ERR_free_strings();

	return 0;
}

int sendCborMessage(unsigned char *cborBuf, int len) {
	// printf ("Sending CBOR message, len %d\n", len);
	processCborRequest (cborBuf, len);
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

	// poll for message
	// semaphores would make this more efficient, bue perhaps less portable
	while (STAILQ_EMPTY (&msg_queue_head)) {
		// printf ("receiveCborMessage waiting for message...\n");
		sleep(1);
	}

	// pull a response message off the queue
	struct msgQueue *newMsg;
	newMsg	= STAILQ_FIRST(&msg_queue_head);
	STAILQ_REMOVE_HEAD(&msg_queue_head, list);
	cborBuf = newMsg->cborMsg;
	*len = newMsg->len;
	free(newMsg);

	return cborBuf;
}

enum {
	MAKE_CREDENTIAL = 1,
	GET_ATTESTATION,
	CANCEL,
	GET_INFO
};

static void processCborRequest(unsigned char *msg, unsigned int len) {
	switch (msg[0]) {
		case MAKE_CREDENTIAL:
			printf ("make credential\n");
			parseMakeCredentialRequest(&msg[1], len);
			break;
		case GET_ATTESTATION:
			printf("get attestation\n");
			break;
		case CANCEL:
			printf("cancel\n");
			break;
		case GET_INFO:
			printf("get info\n");
			break;
		default:
			printf ("processCborRequest: unknown message type\n");
	}

	// send messge
	struct msgQueue* response = (struct msgQueue *)malloc (sizeof (struct msgQueue));
	response->cborMsg = (unsigned char *)strdup ((char *)msg);
	response->len = strlen ((char *)msg);
	STAILQ_INSERT_TAIL(&msg_queue_head, response, list);
}

static void parseMakeCredentialRequest(unsigned char *msg, unsigned int len) {
	/* Assuming `buffer` contains `info.st_size` bytes of input data */
	struct cbor_load_result result;
	cbor_item_t * item = cbor_load(msg, len, &result);
	/* Pretty-print the result */
	cbor_describe(item, stdout);
	fflush(stdout);
	/* Deallocate the result */
	cbor_decref(&item);
}

static void createMakeCredentialResponse() {
	// /* Preallocate the map structure */
	// cbor_item_t * root = cbor_new_definite_map(2);
	// /* Add the content */
	// cbor_map_add(root, (struct cbor_pair) {
	// 	.key = cbor_move(cbor_build_string("Is CBOR awesome?")),
	// 	 .value = cbor_move(cbor_build_bool(true))
	// });
	// cbor_map_add(root, (struct cbor_pair) {
	// 	.key = cbor_move(cbor_build_uint8(42)),
	// 	 .value = cbor_move(cbor_build_string("Is the answer"))
	// });
	// /* Output: `length` bytes of data in the `buffer` */
	// unsigned char * buffer;
	// size_t buffer_size, length = cbor_serialize_alloc(root, &buffer, &buffer_size);
}