
/*
 * created at 2022-07-29 13:48.
 *
 * Company: HashedIn By Deloitte.
 * Copyright (C) 2022 HashedIn By Deloitte
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdbool.h>
#include "serial_interface/serial_config.h"
#include "cloud_server/cloud_server.h"
#include "can_bus/can_bus.h"
#include "global/global.h"
#include "utils/common_utils.h"
#include "bluetooth_can/bluetooth_can.h"

#define DEBUG

#define CLIENT_CONTROLLER "/dev/ttyACM0" /* Used for default path */
#define GPS_MODULE "/dev/ttyUSB0" /* Used for default path */

#define CC_DEVICE_LIST_LENGTH 1
#define CC_MANUFACTURE_NAME "STM" /* Manufacture name */

#define GPS_DEVICE_LIST_LENGTH 1
#define GPS_MANUFACTURE_NAME "Silicon" /* Manufacture name */

#define AWS_CLIENT_ID "testclient"
#define AWS_TOPIC "testclient/example/topic"

/* Update AWS IOT ENDPOINT */
#define AWS_IOT_ENDPOINT "a2b4nodx1n0ee2-ats.iot.us-east-1.amazonaws.com"

/*BLE Device*/
#define BLE_DEVICE_NAME "OBDII"
#define BLE_DEVICE_SERIAL_PROFILE_UUID "00001101-0000-1000-8000-00805f9b34fb"


/* arg_struct struct to holds required thread arguments*/
struct arg_struct
{
	struct uart_device_struct uart_device;
	struct cloud_data_struct *cloud_data;
};

#endif
