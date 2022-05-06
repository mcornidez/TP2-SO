// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>
#include <keyboard.h>
#include <stdint.h>
#include <naiveConsole.h>

void int_20() {
	timer_handler();
}

void int_21() {
	keyboard_handler();
}

void irqDispatcher(uint64_t irq) {
	/*
	PInterruption interruption = interruptions[irq];
	if (interruption != 0)
		interruption();
	return;
	*/
	switch(irq){
		case 0: 
			int_20();
			break;
        case 1:
            int_21();
            break;
	}
}