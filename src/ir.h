#ifndef _IR_H
#define _IR_H

void ir_init();

void ir_tick();

/**
 * Is the ball present ?
 */
bool ir_present();

/**
 * Is something present right now ?
 */
bool ir_present_now();

void ir_diagnostic();

#endif
