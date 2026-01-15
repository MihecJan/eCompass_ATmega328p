#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <stdint.h>

typedef struct {
	uint8_t *buffer;
	volatile uint8_t head;  // Points to the next write position — i.e., the next empty slot.
	volatile uint8_t tail;  // Points to the next read position — i.e., the oldest valid data.
	uint8_t mask;		// size - 1
} ring_buffer_t;

void rb_init(ring_buffer_t *rb, uint8_t *storage, uint8_t size);

void rb_push_overwrite(ring_buffer_t *rb, uint8_t c);

uint8_t rb_pop(ring_buffer_t *rb, uint8_t *out);

uint8_t rb_peek(const ring_buffer_t *rb, uint8_t offset);

uint8_t rb_available(const ring_buffer_t *rb);


#endif /* RING_BUFFER_H_ */