#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/dispatch.h>

#define NO_OF_MSGS			5

#define ATTACH_POINT 		"firstService"

/* Fist header used by GNS and application */
typedef struct _pulse header_t;

/* header types
 *  0 .. _IO_BASE -1 						: Pulse Messages
 * 	_IO_BASE = 0x100 .. _IO_MAX = 0x1FF 	: Sync Messages of the System
 * 	_IO_MAX + 1 ..							: Sync Messages of our application
 */

// Messages types of our application
#define STR_MSG   (_IO_MAX + 1)
#define DATA_MSG  (_IO_MAX + 2)

/* Second header: used by application - if required */
typedef struct {
	int size; // size of data block
	int count; // some counter used by the application
	// further data fields required by our application
} app_header_t;


int client() { // Client side of the code
    int server_coid;
    char  *payload0 = "Dies ist ein sehr grosser Datenbereich ...";
    char  *payload1 = "Eine andere Payload ...";

    header_t     header;
    app_header_t app_header;
    iov_t        iov[3]; // multi part msg

    char  r_msg[512]; // buffer for the answer

    // Use GNS for setting up the connection to the server - running somewhere in the network
    if ((server_coid = name_open(ATTACH_POINT, NAME_FLAG_ATTACH_GLOBAL)) == -1) {
    	perror("Client: name_open failed");
        return EXIT_FAILURE;
    }

    // Do whatever work you wanted with server connection
    for (int no = 0; no < NO_OF_MSGS; no++) {
    	printf("Client sending msg %d \n", no);

    	// Construct payload
    	char * payload = ((no % 2) == 0) ? payload0 : payload1; // use dummy values as payload
    	int payload_size = strlen(payload)+1;   // +1 due to \0 at the end of a string

    	// Compose the msg using an IOV
    	header.type = STR_MSG;    // define msg type
    	header.subtype = 0x00;

    	app_header.size = payload_size;	// fill application header
    	app_header.count = no;

    	SETIOV(iov+0, &header, sizeof(header));
    	SETIOV(iov+1, &app_header, sizeof(app_header));
    	SETIOV(iov+2, payload, payload_size);

        // send msg
    	if (-1 == MsgSendvs(server_coid, iov, 3, r_msg, sizeof(r_msg))){
    		perror("Client: MsgSend failed");
    	    exit(EXIT_FAILURE);
    	}
    	// Answer form server should be structured, too.
    	// Did not check for buffer overflow before printing r_msg
    	printf("Client: Answer from server: %s \n", r_msg);
    }
    // close connection
    name_close(server_coid);
    return EXIT_SUCCESS;
}


static void handle_pulse(header_t hdr, int rcvid){
	switch (hdr.code) {
	case _PULSE_CODE_DISCONNECT:
		printf("Server received _PULSE_CODE_DISCONNECT\n");
	    /* A client disconnected all its connections (called
	     * name_close() for each name_open() of our name) or
	     * terminated. */
	    ConnectDetach(hdr.scoid);
	    break;
	case _PULSE_CODE_UNBLOCK:
	    printf("Server received _PULSE_CODE_UNBLOCK\n");
	    /* REPLY blocked client wants to unblock (was hit by
	     * a signal or timed out).  It's up to you if you
	     * reply now or later. */
	    break;
	default:
	    /* A pulse sent by one of your processes or a
	     * _PULSE_CODE_COIDDEATH or _PULSE_CODE_THREADDEATH
	     * from the kernel? */
	     printf("Server received some pulse msg.\n");
	     break;
	}
}

static void handle_ONX_IO_msg(header_t hdr, int rcvid){
	if (hdr.type == _IO_CONNECT ) {
		// QNX IO msg _IO_CONNECT was received; answer with EOK
	    printf("Server received _IO_CONNECT (sync. msg) \n");
	    MsgReply( rcvid, EOK, NULL, 0 );
	    return;
	}
	// Some other QNX IO message was received; reject it
	printf("Server received unexpected (sync.) msg type = 0x%04X\n", hdr.type);
	MsgError( rcvid, ENOSYS );
}

static void handle_app_msg(header_t hdr, int rcvid){
	// check type
	const char *ret_msg = "Got the message";

	if (DATA_MSG == hdr.type) {
		printf("Server: DATA_MSG not supported.\n");
		MsgError(rcvid,EPERM);
		return;
	}
	if (STR_MSG == hdr.type) {
		printf("Server: STR_MSG received.\n");
		// read app header
		app_header_t app_header;
		MsgRead(rcvid, &app_header, sizeof(app_header), sizeof(header_t));
		// ToDo return value of MsgRead must be checked, in particular number of read bytes

		// read payload
		char * buf = (char *) malloc(app_header.size);
		// ToDo return value of malloc must be checked

		MsgRead(rcvid, buf, app_header.size, sizeof(header_t) + sizeof(app_header_t));
		// ToDo return value of MsgRead must be checked, in particular number of read bytes

		printf("Server: got the STR_MSG (count = %d): %s \n", app_header.count, buf);
		// send reply
		MsgReply(rcvid, EOK, ret_msg, strlen(ret_msg) + 1);
		free(buf);
		return;
	}
	// Wrong msg type
	printf("Server: Wrong message type 0x%04X\n", hdr.type);
	MsgError(rcvid,EPERM);
}

int server() { // Server side of the code
   name_attach_t *attach;

   // Create a global name (/dev/name/global/...) known within the network
   if ((attach = name_attach(NULL, ATTACH_POINT, NAME_FLAG_ATTACH_GLOBAL)) == NULL) {
   	   perror("Server: name_attach failed");
       return EXIT_FAILURE;
   }

   // Do your MsgReceive's here now with the chid
   while (1) {
	   // Waiting for a message and read first header
	   header_t header;
	   int rcvid = MsgReceive (attach->chid, &header, sizeof (header_t), NULL);

       if (rcvid == -1) { // Error occurred
    	   perror("Server: MsgReceived failed");
           break;
       }

       if (rcvid == 0) {// Pulse was received
    	   handle_pulse(header, rcvid);
           continue;
       }
       // some sync msg was received
       if ((_IO_BASE <= header.type) && (header.type <= _IO_MAX)) {
    	   // Some QNX IO msg generated by gns was received
    	   handle_ONX_IO_msg(header, rcvid);
    	   continue;
       }
       //  A sync msg (presumable ours) was received; handle it
       handle_app_msg(header, rcvid);
   }

   // Server stopped; remove the name from the space
   name_detach(attach, 0);

   return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    int ret;
    printf("gns must be running.\n");
    if (argc < 2) {
        printf("Usage %s -s | -c \n", argv[0]);
        ret = EXIT_FAILURE;
    }
    else if (strcmp(argv[1], "-c") == 0) {
        printf("Running Client ... \n");
        ret = client();   /* see name_open() for this code */
    }
    else if (strcmp(argv[1], "-s") == 0) {
        printf("Running Server ... \n");
        ret = server();   /* see name_attach() for this code */
    }
    else {
        printf("Usage %s -s | -c \n", argv[0]);
        ret = EXIT_FAILURE;
    }
    return ret;
}

