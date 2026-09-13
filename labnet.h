#ifndef LABNET_H
#define LABNET_H

#include <Arduino.h>

void initNetwork();
void updateNetwork();

bool isWifiConnected();
bool isTimeSynced();

void getFormattedTime(char *buffer, size_t maxLen);  // "HH:MM:SS", "--:--:--" until synced
void getFormattedDate(char *buffer, size_t maxLen);  // "YYYY-MM-DD", "----/--/--" until synced

#endif // LABNET_H
