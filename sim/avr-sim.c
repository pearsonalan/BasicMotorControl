#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>

#include "sim_avr.h"
#include "avr_ioport.h"
#include "avr_spi.h"
#include "avr_timer.h"
#include "sim_elf.h"
#include "sim_gdb.h"
#include "sim_vcd_file.h"

avr_t *avr = NULL;
avr_vcd_t vcd_file;

void pin_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param)
{
	printf("Pin change: %s => %d\n", irq->name, value);
}

void sig_int(int sign)
{
	printf("signal caught, simavr terminating\n");
	if (avr)
		avr_terminate(avr);
	exit(0);
}

void show_ports(avr_t *avr)
{
	avr_io_t * port = avr->io_port;
	while (port)
	{
		printf("%s %s %08x %d\n", port->kind, port->irq_names ? *port->irq_names : NULL, port->irq_ioctl_get, port->irq_count);
		port = port->next;
	}
}

int main(int argc, char *argv[])
{
	int state;
	elf_firmware_t f;
	const char *fname = "../BasicMotorControl.elf";
	const char *mmcu = "atmega328p";

	if (elf_read_firmware(fname, &f) != 0)
	{
		exit(1);
	}

	f.frequency = 16000000;

	printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, mmcu);

	avr = avr_make_mcu_by_name(mmcu);
	if (!avr)
	{
		fprintf(stderr, "%s: AVR '%s' not known\n", argv[0], mmcu);
		exit(1);
	}

	avr_init(avr);
	avr_load_firmware(avr, &f);

#if 0
	/* even if not setup at startup, activate gdb if crashing */
	avr->gdb_port = 1234;
	avr->state = cpu_Stopped;
	avr_gdb_init(avr);
#endif

	/* VCD file initialization */
	avr_vcd_init(avr, "wave.vcd", &vcd_file, 10000 /* usec */);
	avr_vcd_add_signal(&vcd_file, avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0), 1, "B0" ); 
	avr_vcd_add_signal(&vcd_file, avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 1), 1, "B1" ); 
	avr_vcd_add_signal(&vcd_file, avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 2), 1, "B2" ); 
	avr_vcd_add_signal(&vcd_file, avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 3), 1, "B3" ); 
	avr_vcd_start(&vcd_file);

	/* IRQ callback hooks */
	avr_irq_register_notify( avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0), pin_changed_hook, NULL); 
	avr_irq_register_notify( avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 1), pin_changed_hook, NULL); 
	avr_irq_register_notify( avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 2), pin_changed_hook, NULL); 
	avr_irq_register_notify( avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 3), pin_changed_hook, NULL); 

	/* show some info */
	show_ports(avr);

	/* install signal handlers */
	signal(SIGINT, sig_int);
	signal(SIGTERM, sig_int);

	/* main loop */
	printf("*** Entering main loop ***\n");
	while (1) 
	{
		state = avr_run(avr);
		if (state == cpu_Done || state == cpu_Crashed)
		{
			printf("CPU State %d\n", state);
			break;
		}
	}

	avr_vcd_stop(&vcd_file);
	avr_terminate(avr);

	return 0;
}

