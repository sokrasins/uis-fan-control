#ifndef STATUS_H_
#define STATUS_H_

#include <stdint.h>

#define STATUS_OK           0U
#define STATUS_GENERIC_ERR  1U
#define STATUS_BAD_PARAM    2U
#define STATUS_UNSUPPORTED  3U

typedef uint32_t status_t;

#endif /*STATUS_H_*/