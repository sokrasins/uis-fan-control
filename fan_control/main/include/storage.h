#ifndef STORAGE_H_
#define STORAGE_H_

#include <stdint.h>
#include "status.h"

status_t storage_init(void);

status_t storage_read_u32(char *key, uint32_t *data);

status_t storage_write_u32(char *key, uint32_t data);

#endif /*STORAGE_H_*/