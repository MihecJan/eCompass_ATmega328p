#include "../config.h"

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "ring_buffer.h"

void rb_init(ring_buffer_t *rb, uint8_t *storage, uint8_t size)
{
	// size must be power of two
	rb->buffer = storage;
	rb->head = 0;
	rb->tail = 0;
	rb->mask = size - 1;
}

void rb_push_overwrite(ring_buffer_t *rb, uint8_t c)
{
	// Write data to current head first
	rb->buffer[rb->head] = c;			

	uint8_t next_head = (rb->head + 1) & rb->mask;
	
	// If head moves into the tail position, next push we are overwriting oldest data.
	// Move tail forward to maintain avoid going into "empty" state.
	if (next_head == rb->tail)
	{
		rb->tail = (rb->tail + 1) & rb->mask;
	}

	rb->head = next_head;
}

uint8_t rb_pop(ring_buffer_t *rb, uint8_t *out)
{
	if (rb->head == rb->tail)
	{
		return 0; // empty
	}

	*out = rb->buffer[rb->tail];
	rb->tail = (rb->tail + 1) & rb->mask;
	return 1;
}

uint8_t rb_peek(const ring_buffer_t *rb, uint8_t offset)
{
	if (offset >= rb_available(rb)) return 0;

	return rb->buffer[(rb->tail + offset) & rb->mask];
}

uint8_t rb_available(const ring_buffer_t *rb)
{
	uint8_t head = rb->head;
	uint8_t tail = rb->tail;
	return (uint8_t)(head - tail) & rb->mask;
}