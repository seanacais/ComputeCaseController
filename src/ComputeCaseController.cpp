/*
 * ComputeCaseController.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: Kevin C. Castner
 */

static char const copyright[] =
		"Copyright (c) Kevin C. Castner, 2015; All Rights Reserved";

// Include definitions for the current part defined in Makefile
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>

#include "Button.h"
#include "Scheduler.h"
#include "eventBuffer.h"
#include "PSURemote.h"
#include "DiagnosticLED.h"
#include "VoltMeter.h"

// Function Prototypes
void initialize(void);
void sleepWhenIdle();

// Global for timer overflow
extern uint8_t TCNT0_VALUE;

// Need defines and variables for the state machine
#define EVAR_NUN 0xFF
#define EVAR_PSU 0x00
#define EVAR_VMS 0x01

#define EEPROM_BOOT_OFF 0x00
#define EEPROM_BOOT_ON 0x01

uint8_t evarVal = EVAR_NUN;
bool cfgMode = false;
bool btn_prs_3_dcvm0_state_save;

//uint8_t EEMEM boot_vms_state_eeloc = EEPROM_BOOT_ON;
//uint8_t EEMEM boot_psu_state_eeloc = EEPROM_BOOT_OFF;

uint8_t boot_psu_state;
uint8_t boot_psu_state_new;
uint8_t boot_vms_state;
uint8_t boot_vms_state_new;

PSURemote p0;   // PSU with remote control
Button sw0;      // input momentary switch
Scheduler sch0;   // scheduler object
eventBuffer eb0; // global input event buffer
DiagnosticLED dled0;
VoltMeter dcvm0;

#define DCVM_OFF_TIME 8192

void initialize(void) {
//	TCCR0 |= _BV(CS02);	            // set timer 0 prescalar to CK/256
//	TCCR0 |= _BV(CS01) | _BV(CS00);	// set timer 0 prescalar to CK/64
	TCCR0 |= _BV(CS01);          	// set timer 0 prescalar to CK/8
	TCNT0 = TCNT0_VALUE; // Load the Timer Value
	TIMSK |= _BV(TOIE0); // enable Timer Counter 0 overflow interrupt

	boot_psu_state = eeprom_read_byte((uint8_t *)0x20);
	//boot_psu_state = eeprom_read_byte(&boot_psu_state_eeloc);
	boot_psu_state_new = boot_psu_state;
	boot_vms_state = eeprom_read_byte((uint8_t *)0x21);
	//boot_vms_state = eeprom_read_byte(&boot_vms_state_eeloc);
	boot_vms_state_new = boot_vms_state;

	if(boot_psu_state == EEPROM_BOOT_ON) {
		eb0.addEvent(PSU_RMT_ON);
	} else {
		eb0.addEvent(PSU_RMT_OFF);
	}
	if(boot_vms_state == EEPROM_BOOT_ON) {
		eb0.addEvent(DCVM_ON);
	} else {
		eb0.addEvent(DCVM_OFF);
	}
}

void sleepWhenIdle(void) {
	set_sleep_mode(SLEEP_MODE_IDLE);
	cli();
	sleep_enable();
	sei();
	sleep_cpu();
	sleep_disable();
	sei();
}

/*
 * ComputeCaseController is the controller code for the front panel push button on the
 * Mini-itx Compute Case.
 *
 * The PowerSupply has a remote on-off control for the main power terminals but this
 * processor and circuit is powered from the 5VDC always on
 *
 * There is a single switch and a VoltMeter which is the only display.  The diagnostic
 * LED is not present in the final deployed system.
 *
 * Allowable input events are
 * BUTTON_PRESS - A single press and release of the switch
 * BUTTON_PRESS_2 - a double click of the switch inside the multipress timeout period
 * BUTTON_PRESS_3 - a triple click of the switch inside the multipress timeout period
 * BOTTON_P_AND_H - Press down and hold the switch for the PANDH timeout
 *
 * States used in the decision tree are
 * MODE - RUN or CFG - System boots in RUN Mode
 * EVAR - EEPROM Variable.  Used in CFG Mode Only, Values are NUN, VMS (Volt Meter State), and PSU
 * DCVM_STATE - ON or OFF  -- Boot mode depends on EEPROM settings
 * PSU_STATE - ON or OFF  -- Boot mode depends on EEPROM settings
 *
 * State/Event Response Matrix
 * | DCVM_STATE |  PSU_STATE | MODE | EVAR |    EVENT       | RESPONSE                                                                        |
 * |    OFF     |    *       | RUN  |  *   | BUTTON_PRESS   | Execute DCVM_ON, remove scheduled DCVM_OFF events, schedule DCVM_OFF in 8s      |
 * |    ON      |    OFF     | RUN  |  *   | BUTTON_PRESS   | Turn on PSU, remove any scheduled DCVM_OFF events, schedule DCVM_OFF in 8s      |
 * |    ON      |    ON      | RUN  |  *   | BUTTON_PRESS   | Remove scheduled DCVM_OFF events, schedule DCVM_OFF in 8s                       |
 * |    *       |    *       | RUN  |  *   | BUTTON_PRESS_2 | Remove scheduled DCVM_OFF events, execute DCVM_BLINK_2, execute DCVM_ON         |
 * |    *       |    *       | RUN  |  *   | BUTTON_PRESS_3 | Remove scheduled events, store DCVM_STATE, execute DCVM_BLINK3,                 |
 * |                                                            execute DCVM_ON, enter CFG Mode                                               |
 * |    *       |    *       | RUN  |  *   | BUTTON_P_AND_H | Turn off PSU, remove DCVM_OFF events, schedule DCVM_OFF for 8s                  |
 * |    *       |    *       | CFG  | NUN  | BUTTON_PRESS   | execute DCVM_BLINK, Set EVAR = PSU                                              |
 * |    *       |    *       | CFG  | PSU  | BUTTON_PRESS   | execute DCVM_BLINK, Set EVAR = NUN, prep to store PSU_ON in EEPROM for boot     |
 * |    *       |    *       | CFG  | PSU  | BUTTON_PRESS_2 | execute DCVM_BLINK_2 , Set EVAR = NUN, prep to store PSU_OFF in EEPROM for boot |
 * |    *       |    *       | CFG  | NUN  | BUTTON_PRESS_2 | execute DCVM_BLINK_2, Set EVAR = VMS                                            |
 * |    *       |    *       | CFG  | VMS  | BUTTON_PRESS   | execute DCVM_BLINK, Set EVAR = NUN, prep to store VMS_ON in EEPROM for boot     |
 * |    *       |    *       | CFG  | VMS  | BUTTON_PRESS_2 | execute DCVM_BLINK_2 , Set EVAR = NUN, prep to store VMS_OFF in EEPROM for boot |
 * |    *       |    *       | CFG  |  *   | BUTTON_PRESS_3 | Save EEPROM variables (only if needed), enter RUN mode, restore DCVM_STATE      |
 * |    *       |    *       | CFG  |  *   | BUTTON_P_AND_H | Enter RUN mode, restore DCVM_STATE (Abort EEPROM Config Change)                 |
 *
 */

int main(void) {
	initialize();

	sei();
	while (1) {
		// don't exit this loop until the event buffer is empty.
		// If the isMore() loop exits, the processor will go into
		// an idle sleep.  The timer interrupt will ensure it comes
		// back out of sleep mode.
		while (eb0.isMore()) {
			switch (eb0.getNextEvent()) {
			case BUTTON_DOWN:
				sw0.accept_down();
				break;
			case BUTTON_UP:
				sw0.accept_up();
				break;
			case BUTTON_ASSESS:
				sw0.assess();
				break;
			case SCH_EVENT_READY:
				eb0.addEvent(sch0.getEvent());
				break;
			case PSU_RMT_OFF:
				p0.off();
				break;
			case PSU_RMT_ON:
				p0.on();
				break;
			case DCVM_ON:
				dcvm0.on();
				break;
			case DCVM_OFF:
				dcvm0.off();
				break;
			case DCVM_TOGGLE:
				dcvm0.toggle();
				break;
			case DCVM_BLINK:
				dcvm0.blink();
				break;
			case DCVM_BLINK_2:
				dcvm0.blink_2();
				break;
			case DCVM_BLINK_3:
				dcvm0.blink_3();
				break;
			case DIAG_LED_ON:
				dled0.on();
				break;
			case DIAG_LED_OFF:
				dled0.off();
				break;
			case DIAG_LED_TOGGLE:
				dled0.toggle();
				break;
			case DIAG_LED_BLINK:
				dled0.blink();
				break;
			case DIAG_LED_BLINK_2:
				dled0.blink_2();
				break;
			case DIAG_LED_BLINK_3:
				dled0.blink_3();
				break;


			/*
			 * Buttons presses drive the state machine.
			 */
			case BUTTON_PRESS:
				if (!cfgMode) {
					sch0.removeEventType(DCVM_OFF);
					sch0.scheduleEvent(DCVM_OFF_TIME, DCVM_OFF);
					if (dcvm0.isOff()) {
						sch0.scheduleEvent((DCVM_BLINK_TIME*3),DCVM_ON);
					} else {  // dcvm is on
						if(p0.isOff()){
							eb0.addEvent(PSU_RMT_ON);
						}
					}
				} else {  // in cfgMode
					switch (evarVal) {
					case EVAR_NUN:
						evarVal = EVAR_PSU;
						break;
					case EVAR_PSU:
						boot_psu_state_new = EEPROM_BOOT_ON;
						evarVal = EVAR_NUN;
						break;
					case EVAR_VMS:
						boot_vms_state_new = EEPROM_BOOT_ON;
						evarVal = EVAR_NUN;
						break;
					default:
						dled0.blink();
					}
				}
				sch0.scheduleEvent(DCVM_BLINK_TIME, DCVM_BLINK);
				break;
			case BUTTON_PRESS_2:
				if (!cfgMode) {
					sch0.removeEventType(DCVM_OFF);
					sch0.scheduleEvent((DCVM_BLINK_2_TIME + (DCVM_BLINK_TIME*3)), DCVM_ON);
				} else { // in cfgMode
					switch (evarVal) {
					case EVAR_NUN:
						evarVal = EVAR_VMS;
						break;
					case EVAR_PSU:
						boot_psu_state_new = EEPROM_BOOT_OFF;
						evarVal = EVAR_NUN;
						break;
					case EVAR_VMS:
						boot_vms_state_new = EEPROM_BOOT_OFF;
						evarVal = EVAR_NUN;
						break;
					default:
						dled0.blink();
					}
				}
				sch0.scheduleEvent(DCVM_BLINK_TIME, DCVM_BLINK_2);
				break;
			case BUTTON_PRESS_3:
				if (!cfgMode) {
					sch0.removeEvents();
					btn_prs_3_dcvm0_state_save = dcvm0.isOn();
					sch0.scheduleEvent((DCVM_BLINK_3_TIME + (DCVM_BLINK_TIME*3)),DCVM_ON);
					cfgMode = true;
					evarVal = EVAR_NUN;
				} else {
					evarVal = EVAR_NUN;
					cfgMode = false;
					if (btn_prs_3_dcvm0_state_save) {
						sch0.scheduleEvent((DCVM_BLINK_3_TIME + (DCVM_BLINK_TIME*3)),DCVM_ON);
					} else {
						sch0.scheduleEvent((DCVM_BLINK_3_TIME + (DCVM_BLINK_TIME*3)),DCVM_OFF);
					}
					// save EEPROM Settings;
					if (boot_psu_state_new != boot_psu_state) {
						boot_psu_state = boot_psu_state_new;
						eeprom_update_byte((uint8_t *)0x20, boot_psu_state);
						//eeprom_update_byte((uint8_t *)boot_psu_state_eeloc, boot_psu_state);
					}
					if (boot_vms_state_new != boot_vms_state) {
						boot_vms_state = boot_vms_state_new;
						eeprom_update_byte((uint8_t *)0x21, boot_vms_state);
						//eeprom_update_byte((uint8_t *)boot_vms_state_eeloc, boot_vms_state);
					}
				}
				sch0.scheduleEvent(DCVM_BLINK_TIME, DCVM_BLINK_3);
				break;
			case BUTTON_P_AND_H:
				if (!cfgMode) {
					eb0.addEvent(PSU_RMT_OFF);
					sch0.removeEventType(DCVM_OFF);
					eb0.addEvent(DCVM_BLINK);
					sch0.scheduleEvent(DCVM_BLINK_TIME*2, DCVM_ON);
					sch0.scheduleEvent(DCVM_OFF_TIME, DCVM_OFF);
				} else {
					sch0.scheduleEvent(DCVM_BLINK_TIME, DCVM_BLINK_2);
					sch0.scheduleEvent(DCVM_BLINK_TIME + DCVM_BLINK_3_TIME, DCVM_BLINK_2);
					sch0.scheduleEvent(DCVM_BLINK_TIME + (DCVM_BLINK_3_TIME*2), DCVM_BLINK_2);
					evarVal = EVAR_NUN;
					boot_psu_state_new = boot_psu_state;
					boot_vms_state_new = boot_vms_state;
					cfgMode = false;
					if (btn_prs_3_dcvm0_state_save) {
						eb0.addEvent(DIAG_LED_BLINK_2);
						sch0.scheduleEvent((DCVM_BLINK_TIME) + (DCVM_BLINK_3_TIME*3), DCVM_ON);
					} else {
						eb0.addEvent(DIAG_LED_BLINK_3);
						sch0.scheduleEvent((DCVM_BLINK_TIME) + (DCVM_BLINK_3_TIME*3), DCVM_OFF);
					}
				}
				break;
			}

		}
		sleepWhenIdle();
	} //  endwhile
} // end main()
