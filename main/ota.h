#ifndef OTA_H
#define OTA_H

#include "iterations.h"

void ota_validation_checker(void);

void ota_start(TaskHandle_t *task, bool *state_connecting);

#endif