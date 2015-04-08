#include <rthw.h>
#include <stm32f10x.h>
#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "sdp.pb.h"

// 0 pack

static int
pack_seg(const uint8_t *src, uint8_t * buffer, int sz, int n) {
	uint8_t header = 0;
	int notzero = 0;
	int i;
	uint8_t * obuffer = buffer;
	++buffer;
	--sz;
	if (sz < 0)
		obuffer = NULL;

	for (i=0;i<8;i++) {
		if (src[i] != 0) {
			notzero++;
			header |= 1<<i;
			if (sz > 0) {
				*buffer = src[i];
				++buffer;
				--sz;
			}
		}
	}
	if ((notzero == 7 || notzero == 6) && n > 0) {
		notzero = 8;
	}
	if (notzero == 8) {
		if (n > 0) {
			return 8;
		} else {
			return 10;
		}
	}
	if (obuffer) {
		*obuffer = header;
	}
	return notzero + 1;
}

__STATIC_INLINE void
write_ff(const uint8_t * src, uint8_t * des, int n) {
	int i;
	int align8_n = (n+7)&(~7);

	des[0] = 0xff;
	des[1] = align8_n/8 - 1;
	memcpy(des+2, src, n);
	for(i=0; i< align8_n-n; i++){
		des[n+2+i] = 0;
	}
}

int
sproto_pack(const void * srcv, int srcsz, void * bufferv, int bufsz) {
	uint8_t tmp[8];
	int i;
	const uint8_t * ff_srcstart = NULL;
	uint8_t * ff_desstart = NULL;
	int ff_n = 0;
	int size = 0;
	const uint8_t * src = srcv;
	uint8_t * buffer = bufferv;
	for (i=0;i<srcsz;i+=8) {
		int n;
		int padding = i+8 - srcsz;
		if (padding > 0) {
			int j;
			memcpy(tmp, src, 8-padding);
			for (j=0;j<padding;j++) {
				tmp[7-j] = 0;
			}
			src = tmp;
		}
		n = pack_seg(src, buffer, bufsz, ff_n);
		bufsz -= n;
		if (n == 10) {
			// first FF
			ff_srcstart = src;
			ff_desstart = buffer;
			ff_n = 1;
		} else if (n==8 && ff_n>0) {
			++ff_n;
			if (ff_n == 256) {
				if (bufsz >= 0) {
					write_ff(ff_srcstart, ff_desstart, 256*8);
				}
				ff_n = 0;
			}
		} else {
			if (ff_n > 0) {
				if (bufsz >= 0) {
					write_ff(ff_srcstart, ff_desstart, ff_n*8);
				}
				ff_n = 0;
			}
		}
		src += 8;
		buffer += n;
		size += n;
	}
	if(bufsz >= 0){
		if(ff_n == 1)
			write_ff(ff_srcstart, ff_desstart, 8);
		else if (ff_n > 1)
			write_ff(ff_srcstart, ff_desstart, srcsz - (intptr_t)(ff_srcstart - (const uint8_t*)srcv));
	}
	return size;
}

int
sproto_unpack(const void * srcv, int srcsz, void * bufferv, int bufsz) {
	const uint8_t * src = srcv;
	uint8_t * buffer = bufferv;
	int size = 0;
	while (srcsz > 0) {
		uint8_t header = src[0];
		--srcsz;
		++src;
		if (header == 0xff) {
			int n;
			if (srcsz < 0) {
				return -1;
			}
			n = (src[0] + 1) * 8;
			if (srcsz < n + 1)
				return -1;
			srcsz -= n + 1;
			++src;
			if (bufsz >= n) {
				memcpy(buffer, src, n);
			}
			bufsz -= n;
			buffer += n;
			src += n;
			size += n;
		} else {
			int i;
			for (i=0;i<8;i++) {
				int nz = (header >> i) & 1;
				if (nz) {
					if (srcsz < 0)
						return -1;
					if (bufsz > 0) {
						*buffer = *src;
						--bufsz;
						++buffer;
					}
					++src;
					--srcsz;
				} else {
					if (bufsz > 0) {
						*buffer = 0;
						--bufsz;
						++buffer;
					}
				}
				++size;
			}
		}
	}
	return size;
}

void print_hex(void *srcv, size_t srcsz)
{
    uint8_t *src = srcv;
    int32_t sz = srcsz;
    while(srcsz--)
    {
        rt_kprintf("%02X ", *src++);
    }
}

uint8_t buffer[Message_size];
uint8_t pack[Message_size];



    Message message = Message_init_zero;
int test(void)
{
    /* This is the buffer where we will store our message. */
    size_t pack_length;
    size_t message_length;
    bool status;
    pb_istream_t istream;
    /* Encode our message */
    /* Allocate space on the stack to store the message data.
     *
     * Nanopb generates simple struct definitions for all the messages.
     * - check out the contents of simple.pb.h! */
    /* Create a stream that will write to our buffer. */
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    /* Fill in the MSG Type */
    message.has_msg_type = true;
    message.msg_type = MSG_REQ_KEEP_ALIVE;
    message.has_sequence = true;
    message.sequence = 1;
    message.has_request = true;
    message.request.has_login = true;
    message.request.login.has_id = true;
    message.request.login.id.size = 8;
    rt_memcpy(message.request.login.id.bytes, "asdfasasdf", 8);
    /* Now we are ready to encode the message! */
    status = pb_encode(&stream, Message_fields, &message);
    rt_kprintf("%d\n", status);
    message_length = stream.bytes_written;
    rt_kprintf("msg_len = %d\n", stream.bytes_written);
    rt_hw_show_memory((rt_uint32_t)buffer, message_length);
    /* Then just check for any errors.. */
    if (!status)
    {
        rt_kprintf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }
    
        rt_kprintf("\n==================\n");
            print_hex(buffer, message_length);
        rt_kprintf("\n==================\n");
        pack_length = sproto_pack(buffer, message_length, pack, sizeof(pack));
        print_hex(pack, pack_length);
    
        rt_kprintf("\n==================\n");
        message_length = sproto_unpack(pack, pack_length, buffer, sizeof(buffer));
    print_hex(buffer, message_length);
    
        rt_kprintf("\n==================\n");
    /* Now we could transmit the message over network, store it in a file or
     * wrap it to a pigeon's leg.
     */
    
    /* But because we are lazy, we will just decode it immediately. */
    /* Allocate space for the decoded message. */

    /* Create a stream that reads from the buffer. */
    istream = pb_istream_from_buffer(buffer, message_length);

    /* Now we are ready to decode the message. */
    status = pb_decode(&istream, Message_fields, &message);

    /* Check for errors... */
    if (!status)
    {
        rt_kprintf("Decoding failed: %s\n", PB_GET_ERROR(&istream));
        return 1;
    }

    /* Print the data contained in the message. */
    rt_kprintf("Your msg type was %d!\n", message.msg_type);
    print_hex(message.request.login.id.bytes, 8);
    return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(test, set led[0 - 1] on[1] or off[0].)
#endif

