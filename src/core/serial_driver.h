// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
/*
 * serial_driver.h
 *
 *  Created on: 27 mai 2016
 *      Author: anais
 */

#ifndef SRC_SERIAL_DRIVER_H_
#define SRC_SERIAL_DRIVER_H_

#include <core/types.h>
/*     Hardware :																			Software :
 *                                                         |    ...    |
 *                         _____________                   |-----------|<- @port_COMx
 *                        |             |-- 0 - TX/RX ---->|           |            	<--- send_char_to__serial_uart(c, @port_COMx)
 *                        |             |                  |-----------|                ---> recv_char_from_serial_uart(@port_COMx)
 *                        |             |-- 1 - IER ------>|           |
 *                        |             |                  |-----------|
 *                   	  |             |-- 2 - IIR/FCR -->|           |
 *                        |             |                  |-----------|
 * _________________      |             |-- 3 - LCR ------>|           |				<--- uart_ready_to_send(@port_COMx) (THR is empty) ?
 * \   _________   /      |             |                  |-----------|				---> uart_ready_to_recv(@port_COMx) (data available) ?
 *  \ |_:_:_:_:_| /-------| serial UART |-- 4 - MCR ------>|           |
 *   \___________/        |             |                  |-----------|
 *    serial port         |             |-- 5 - LSR ------>|           |
 *                        |             |                  |-----------|
 *                        |             |-- 6 - MSR ------>|           |
 *                        |             |                  |-----------|
 *                        |             |-- 7 - SCR ------>|           |
 *                        |_____________|                  |-----------|
 *                                                         |    ...    |
 *                                                         	    RAM
 *
 */

#include <core/io.h>

/* I/O ports usually used to communicate with the serial UART */
#define PORT_COM1 0x3f8
#define PORT_COM2 0x2f8
#define PORT_COM3 0x3e8
#define PORT_COM4 0x2e8

/* LCR.dla = 0 */
#define SERIAL_TXRX(BASE)       (BASE)
#define SERIAL_TX(BASE)         (BASE)     /* WO */
#define SERIAL_RX(BASE)         (BASE)     /* RO */
#define SERIAL_IER(BASE)       ((BASE)+1)  /* RW */

#define SERIAL_IIR(BASE)       ((BASE)+2)  /* RO */
#define SERIAL_FCR(BASE)       ((BASE)+2)  /* WO */

#define SERIAL_LCR(BASE)       ((BASE)+3)  /* RW */
#define SERIAL_MCR(BASE)       ((BASE)+4)  /* RW */
#define SERIAL_LSR(BASE)       ((BASE)+5)  /* RO */
#define SERIAL_MSR(BASE)       ((BASE)+6)  /* RO */
#define SERIAL_SCR(BASE)       ((BASE)+7)  /* RW */

/* LCR.dla = 1 */
#define SERIAL_DLA_LSB(BASE)    (BASE)     /* RW */
#define SERIAL_DLA_MSB(BASE)   ((BASE)+1)  /* RW */

/* LCR = 0xbf */
#define SERIAL_EFR(BASE)       ((BASE)+2)  /* RW */
#define SERIAL_XON1(BASE)      ((BASE)+4)  /* RW */
#define SERIAL_XON2(BASE)      ((BASE)+5)  /* RW */
#define SERIAL_XOFF1(BASE)     ((BASE)+6)  /* RW */
#define SERIAL_XOFF2(BASE)     ((BASE)+7)  /* RW */

/*
** Interrupt Enable Register
*/
#define SERIAL_IER_RECV              (1<<0)
#define SERIAL_IER_THRE              (1<<1)
#define SERIAL_IER_RLSR              (1<<2)
#define SERIAL_IER_MSR               (1<<3)
#define SERIAL_IER_SLEEP             (1<<4)
#define SERIAL_IER_LOW               (1<<5)
#define SERIAL_IER_RESERVED1         (1<<6)
#define SERIAL_IER_RESERVED2         (1<<7)

/*
** Interrupt Info Register
*/
#define SERIAL_IIR_RECV_MASK                  2

#define SERIAL_IIR_INFO_MODEM_STATUS_CHANGE   0
#define SERIAL_IIR_INFO_THR_EMPTY             1
#define SERIAL_IIR_INFO_RECV_DATA_AVAILABLE   2
#define SERIAL_IIR_INFO_LINE_STATUS_CHANGE    3
#define SERIAL_IIR_INFO_CHAR_TIMEOUT          6

#define SERIAL_IIR_INFO_FIFO_NO               0
#define SERIAL_IIR_INFO_FIFO_USABLE           2
#define SERIAL_IIR_INFO_FIFO_ENABLED          3

/*
 * Some FIFO size values
 */
#define SERIAL_FCR_RX_FIFO_1      0
#define SERIAL_FCR_RX_FIFO_4      1
#define SERIAL_FCR_RX_FIFO_8      2
#define SERIAL_FCR_RX_FIFO_14     3

/*
** Line Status Register
*/
#define SERIAL_LSR_DATA_READY_BIT    0
#define SERIAL_LSR_OVERRUN_ERR_BIT   1
#define SERIAL_LSR_PARITY_ERR_BIT    2
#define SERIAL_LSR_FRAME_ERR_BIT     3
#define SERIAL_LSR_BREAK_INT_BIT     4
#define SERIAL_LSR_THRE_BIT          5
#define SERIAL_LSR_TSRE_BIT          6
#define SERIAL_LSR_ZERO_BIT          7

#define SERIAL_LSR_DATA_READY        (1<<SERIAL_LSR_DATA_READY_BIT)
#define SERIAL_LSR_OVERRUN_ERR       (1<<SERIAL_LSR_OVERRUN_ERR_BIT)
#define SERIAL_LSR_PARITY_ERR        (1<<SERIAL_LSR_PARITY_ERR_BIT)
#define SERIAL_LSR_FRAME_ERR         (1<<SERIAL_LSR_FRAME_ERR_BIT)
#define SERIAL_LSR_BREAK_INT         (1<<SERIAL_LSR_BREAK_INT_BIT)
#define SERIAL_LSR_THRE              (1<<SERIAL_LSR_THRE_BIT)
#define SERIAL_LSR_TSRE              (1<<SERIAL_LSR_TSRE_BIT)
#define SERIAL_LSR_ZERO              (1<<SERIAL_LSR_ZERO_BIT)

/* Check the line status for the transmission */
/*
 * Line Status Register (serial_LSR):
 *  ___________________________________
 * | 0	Data available                 |
 * | 1	Overrun error                  |
 * | 2	Parity error                   |
 * | 3	Framing error                  |
 * | 4	Break signal received          |
 * | 5	THR is empty                   |
 * | 6	THR is empty, and line is idle |
 * | 7	Errornous data in FIFO         |
 * |___________________________________|
 *
 */

/*
 *  Types
 */
 typedef union serial_lcr_register
 {
    struct
    {
       uint8_t  word_len:2;   /** (00) 5 bits, (01) 6 bits, (10) 7 bits, (11) 8 bits */
       uint8_t  stop:1;       /** (0) 1 stop bit, (1) 1.5 or 2 stop bits */
       uint8_t  parity:3;     /** (xx0) no parity
 			     	 	 	 	 ** (001) odd
 			     	 	 	 	 ** (011) even
 			     	 	 	 	 ** (101) mark
 			     	 	 	 	 ** (111) space */
       uint8_t  brk:1;        /** (1) break enable */
       uint8_t  dla:1;        /** (1) Divisor Latch Access Bit */

    } __attribute__((packed));
    uint8_t raw;
 } __attribute__((packed)) serial_lcr_reg_t;

 typedef union serial_ier_register
 {
    struct
    {
       uint8_t   recv:1;     /** (1) enable received data available interrupt */
       uint8_t   thre:1;     /** (1) enable THR empty interrupt */
       uint8_t   lsr:1;      /** (1) enable receiver line status interrupt */
       uint8_t   msr:1;      /** (1) enable modem status interrupt */
       uint8_t   sleep:1;
       uint8_t   xoff:1;     /** (1) enable xoff interrupt */
       uint8_t   rts:1;      /** (1) enable RTS interrupts */
       uint8_t   cts:1;      /** (1) enable CTS interrupts */

    } __attribute__((packed));

    uint8_t raw;

 } __attribute__((packed)) serial_ier_reg_t;


/*
** Functions
*/
#define __uart_send_char(BASE,c)     out((c), SERIAL_TX((BASE)))
#define __uart_recv_char(BASE)       in(SERIAL_TX((BASE)))
#define __uart_can_send(BASE)	     (in(SERIAL_LSR((BASE))) & SERIAL_LSR_THRE)
#define __uart_can_recv(BASE)	     (in(SERIAL_LSR((BASE))) & SERIAL_LSR_DATA_READY)

#define uart_enable_dla_registers(BASE)     out( 0x80, SERIAL_LCR((BASE)) )
#define uart_set_lsb_dla_rate(BASE,x)       out( (x),  SERIAL_DLA_LSB((BASE)) )
#define uart_set_msb_dla_rate(BASE,x)       out( (x),  SERIAL_DLA_MSB((BASE)) )
#define uart_enable_efr_registers(BASE)     out( 0xbf, SERIAL_LCR((BASE)) )

void init_serial_uart(uint16_t port);
void serial_write (char *buffer, uint16_t port);

#endif /* SRC_SERIAL_DRIVER_H_ */
