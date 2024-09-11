#pragma once
#include <furi_hal_serial_types.h>

typedef struct SubGhz SubGhz;

typedef struct {
    FuriHalSerialId  uart_nmea_channel;
    FuriHalSerialId  uart_general_channel;
    FuriHalSerialId  uart_esp_channel;
} Gps_nm;