#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>


void report(uint8_t err, const char *prefix, const char *label, const char *value, const char* unit);
void report_uint16(uint8_t err, const char *prefix, const char *label, uint16_t value, const char* unit);
void report_int16(uint8_t err, const char *prefix, const char *label, int16_t value, const char* unit);
void report_decimal(uint8_t err, const char *label, int16_t value, const char *unit);

#endif /* UTILS_H_ */