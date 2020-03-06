/*$file${../project/compo~::clock.c} #######################################*/
/*
* Model: ntp_clock.qm
* File:  ${../project/compo~::clock.c}
*
* This code has been generated by QM 4.3.0 (https://www.state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*/
/*$endhead${../project/compo~::clock.c} ####################################*/
#include "qep_port.h"
#include "qassert.h"
#include "display.h"
#include  "app_apsta.h"
#include "project.h"

Q_DEFINE_THIS_FILE

/* local objects -----------------------------------------------------------*/

/*$declare${AOs::Clock} ####################################################*/

/*Application */
/*${AOs::Clock} ............................................................*/
typedef struct {
/* protected: */
    QTask super;
} Clock;

/* protected: */
static QState Clock_initial(Clock * const me, QEvt const * const e);

/* Application is runnig normally */
static QState Clock_running(Clock * const me, QEvt const * const e);

/* Clock configuration phase: httpd is active, wifi in AP+STA  mode */
static QState Clock_configuration(Clock * const me, QEvt const * const e);
/*$enddecl${AOs::Clock} ####################################################*/

static Clock l_clock;  /* the instance of the Clock State Machine */

/* Public-scope objects ----------------------------------------------------*/

/* Check for the minimum required QP version */
#if (QP_VERSION < 630U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 6.3.0 or higher required
#endif

/*$define${AOs::theClock} ##################################################*/

/*pointer to static instance of the Clock */
/*${AOs::theClock} .........................................................*/
QTask * const theClock = &l_clock.super;
/*$enddef${AOs::theClock} ##################################################*/

/*$define${AOs::Clock_ctor} ################################################*/

/*Clock constructor */
/*${AOs::Clock_ctor} .......................................................*/
void Clock_ctor(void * queue) {
    QHsm_ctor(&l_clock.super.super, Q_STATE_CAST(&Clock_initial));
    Q_ASSERT( queue != (void*)0 );
    l_clock.super.queue = queue;
}
/*$enddef${AOs::Clock_ctor} ################################################*/

/* Active object definition ================================================*/

/*$define${AOs::Clock} #####################################################*/

/*Application */
/*${AOs::Clock} ............................................................*/
/*${AOs::Clock::SM} ........................................................*/
static QState Clock_initial(Clock * const me, QEvt const * const e) {
    /*${AOs::Clock::SM::initial} */
    return Q_TRAN(&Clock_running);
}

/* Application is runnig normally */
/*${AOs::Clock::SM::running} ...............................................*/
static QState Clock_running(Clock * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Clock::SM::running::BUTTON} */
        case BUTTON_SIG: {
            status_ = Q_TRAN(&Clock_configuration);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/* Clock configuration phase: httpd is active, wifi in AP+STA  mode */
/*${AOs::Clock::SM::configuration} .........................................*/
static QState Clock_configuration(Clock * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Clock::SM::configuration} */
        case Q_ENTRY_SIG: {
            wifi_init_softap();
            display_led(DISPLAY_HTTP_ACTIVE, DISPLAY_LED_ON);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock::SM::configuration} */
        case Q_EXIT_SIG: {
            wifi_stop_softap();
            display_led(DISPLAY_HTTP_ACTIVE, DISPLAY_LED_OFF);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock::SM::configuration::TIMEOUT} */
        case TIMEOUT_SIG: {
            status_ = Q_TRAN(&Clock_running);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::Clock} #####################################################*/
