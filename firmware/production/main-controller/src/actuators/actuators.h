#pragma once

void actuators_setup();
void actuators_loop();
void actuators_triggerCycle();
void actuators_setCompressorTime(unsigned long durationMs);
bool actuators_isBusy();
