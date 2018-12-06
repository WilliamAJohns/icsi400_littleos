#include "idt.h"
#include "device.h"

static struct idt_entry idt[NUM_IRQS];

void
init_idt(void)
{
		
}

void
interrupt_handler(struct cpu_state cpu,
	struct stack_state stack,
	unsigned int irq)
{
	unsigned char s1[] = "executing IRQ...";
	dev_write(0, s1, sizeof(s1));

//The C handler should get the state of the registers, the state of the stack and the number of the interrupt as arguments. 
}
