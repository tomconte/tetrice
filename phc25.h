#ifndef PHC25_H
#define PHC25_H

#include <stdint.h>

/************************************************************/
/* Thomson PHC-25 platform definitions                      */
/*                                                          */
/* TODO: Fill in real hardware addresses and color values.  */
/************************************************************/

/* Memory access macros (update addresses if needed) */
#define POKE(addr, value) (*((volatile uint8_t *)(addr)) = (value))
#define PEEK(addr) (*((volatile uint8_t *)(addr)))

/* Basic color codes - placeholder values */
#define black 0
#define red 1
#define green 2
#define orange 3
#define blue 4
#define magenta 5
#define cyan 6
#define pink 7
#define lgreen 10
#define yellow 11
#define lmagenta 13
#define white 15

#endif // PHC25_H
