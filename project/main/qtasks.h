/*
 * qtasks.h
 *
 *  Created on: 3 mar 2020
 *      Author: slawek
 */

#ifndef MAIN_QTASKS_H_
#define MAIN_QTASKS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "qep.h"
#include "qassert.h"
#include "project.h"


//#define POST_EVENT(task,e) do {Q_ASSERT(pdPASS==xQueueSendToBack( ((QTask*)task)->queue, e, 0));} while(0)
#define POST_EVENT(task,e) do {Q_ASSERT(pdPASS==xQueueSendToBack( (task)->queue, e, 0));} while(0)
//extern void qTaskPostEvent(QTask *task, QEvt *e);
extern void qTaskFunction(void *pvParameters);
extern void Q_onAssert(char const *module, int loc);

#endif /* MAIN_QTASKS_H_ */
