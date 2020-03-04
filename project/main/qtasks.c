/*
 * qtasks.c
 *
 *  Created on: 3 mar 2020
 *      Author: slawek
 */

#include "esp_err.h"
#include "esp_log.h"
#include "qtasks.h"



Q_DEFINE_THIS_FILE

static const char *TAG = "QTASK";

void qTaskFunction(void *pvParameters) { /* FreeRTOS task signature */

	QTask *act = (QTask *)pvParameters;
	ProjectEvents pvBuffer;
    QEvt *e = (QEvt *)&pvBuffer;

    ESP_LOGI(TAG, "task started");

	QHSM_INIT(&act->super, (QEvt *)0); /* trigger initial transition */

	ESP_LOGI(TAG, "qhsm initialized");

	ESP_LOGI(TAG, "queue: %p", act->queue);
	ESP_LOGI(TAG, "sizeof pvbuffer: %d", sizeof(ProjectEvents));

    /* event-loop */
    for (;;) { /* for-ever */
    	xQueueReceive( 	act->queue,
    					&pvBuffer,
						portMAX_DELAY ); /* wait for ever */
    	ESP_LOGI(TAG, "event dispatched");
        QHSM_DISPATCH(&act->super, e);
    }
}


void Q_onAssert(char const *module, int loc) {
	ESP_LOGE(TAG, "Error asserted in file %s, loc %d", module, loc);
	for(;;);
}

//void qTaskPostEvent(QTask *task, QEvt *e)
//{
//	BaseType_t queue = xQueueSendToBack( &task->queue, e, 0);
//	Q_ASSERT(queue==pdPASS);
//}
