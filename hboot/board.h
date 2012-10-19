#ifndef __BOARD_H__
#define __BOARD_H__

void board_init();
const char *board_get_cmdline();

/* Check defines */

#ifdef BOARD_UMTS_SHOLES

#define BOARD_DEBUG_UART_BASE			0x49020000
#define BOARD_WDTIMER2_BASE				0x48314000

#else

#error "No board defined!"

#endif

#endif // __BOARD_H__

