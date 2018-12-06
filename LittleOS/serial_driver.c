#include "io.h" /* io.h is implement in the section "Moving the cursor" */

    /* The I/O ports */

    /* All the I/O ports are calculated relative to the data port. This is because
     * all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
     * order, but they start at different values.
     */

    #define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

    #define SERIAL_DATA_PORT(base)          (base)
    #define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
    #define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
    #define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
    #define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

    /* The I/O port commands */

    /* SERIAL_LINE_ENABLE_DLAB:
     * Tells the serial port to expect first the highest 8 bits on the data port,
     * then the lowest 8 bits will follow
     */
    #define SERIAL_LINE_ENABLE_DLAB         0x80

	struct com_port
	{
		unsigned short com;
		unsigned short divisor;
	};

    /** serial_configure_baud_rate:
     *  Sets the speed of the data being sent. The default speed of a serial
     *  port is 115200 bits/s. The argument is a divisor of that number, hence
     *  the resulting speed becomes (115200 / divisor) bits/s.
     *
     *  @param com      The COM port to configure
     *  @param divisor  The divisor
     */
    void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
    {
        outb(SERIAL_LINE_COMMAND_PORT(com),
             SERIAL_LINE_ENABLE_DLAB);
        outb(SERIAL_DATA_PORT(com),
             (divisor >> 8) & 0x00FF);
        outb(SERIAL_DATA_PORT(com),
             divisor & 0x00FF);
    }

    /** serial_configure_line:
     *  Configures the line of the given serial port. The port is set to have a
     *  data length of 8 bits, no parity bits, one stop bit and break control
     *  disabled.
     *
     *  @param com  The serial port to configure
     */
    void serial_configure_line(unsigned short com)
    {
        /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
         * Content: | d | b | prty  | s | dl  |
         * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
         */
        outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
    }

void serial_configure_buffer(unsigned short com)
{
/*	Buffer config register layout: 
 *	Bit:     | 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
 *	Content: | lvl | bs | r | dma | clt | clr | e |
 *
 * 	lvl 	How many bytes should be stored in the FIFO buffers
 * 	bs 	If the buffers should be 16 or 64 bytes large
 * 	r 	Reserved for future use
 * 	dma 	How the serial port data should be accessed
 * 	clt 	Clear the transmission FIFO buffer
 * 	clr 	Clear the receiver FIFO buffer
 * 	e 	If the FIFO buffer should be enabled or not
*/

	outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

void serial_configure_modem(unsigned short com)
{
/* 	Modem control register:
 * 	Bit:     | 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
 * 	Content: | r | r | af | lb | ao2 | ao1 | rts | dtr |
 *
 * 	r 	Reserved
 * 	af 	Autoflow control enabled
 * 	lb 	Loopback mode (used for debugging serial ports)
 * 	ao2 	Auxiliary output 2, used for receiving interrupts
 * 	ao1 	Auxiliary output 1
 * 	rts 	Ready To Transmit
 * 	dtr 	Data Terminal Ready
*/

	outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

void serial_configure_port(const struct com_port *p)
{
	serial_configure_baud_rate(p->com, p->divisor);
	serial_configure_line(p->com);
	serial_configure_buffer(p->com);
	serial_configure_modem(p->com);
}

/**	serial_is_transmit_fifo_empty;
 * 	Checks whether the transmit FIFO queue is empty
 * 	or not for the given COM port.
*/
int serial_is_transmit_fifo_empty(unsigned int com)
{
	/* 0x20 = 0110 0000 */
	return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_write(unsigned char *b, unsigned short len)
{
	int i = 0;
	while (!serial_is_transmit_fifo_empty(SERIAL_COM1_BASE));

	while (i < len)
	{
		outb(SERIAL_COM1_BASE, b[i++]);
	}
}

void serial_test()
{
	struct com_port port = {SERIAL_COM1_BASE, 1};
	unsigned char s[] = "abcdefghijklmnopqrstu";
	serial_configure_port(&port);
	serial_write(s, sizeof(s));
}
