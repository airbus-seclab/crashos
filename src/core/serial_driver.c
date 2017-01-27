/*
 * serial_driver.c
 *
 *  Created on: 27 mai 2016
 *      Author: anais
 */
#include <core/serial_driver.h>
#include <core/print.h>

void init_serial_uart(uint16_t port) {
	serial_ier_reg_t  ier;
	serial_lcr_reg_t  lcr;

	// Disable interrupts
	ier.raw = 0;
	out(ier.raw, SERIAL_IER(port));
	// Allow serial UART baud rate configuration
	uart_enable_dla_registers(port);
	// Configure the serial UART baud rate
	uart_set_lsb_dla_rate(port, 0x01);
	uart_set_msb_dla_rate(port, 0x00);
	// Configure the serial line
	lcr.raw = 0;
	lcr.word_len = 3;
	out(lcr.raw, SERIAL_LCR(port));
	// Flush serial controller buffer
	while(__uart_can_recv(port)) {
	     __uart_recv_char(port);
	}
}

void serial_write (char *buffer, uint16_t port) {
	int cpt = 1024;
	while ((*buffer) && cpt > 0) {
		if(__uart_can_send(port)) {
			__uart_send_char(port, *buffer);
		}
		buffer++;
		cpt--;
	}
}
