/*
 * created at 2022-07-29 14:30.
 *
 * Company: HashedIn By Deloitte.
 * Copyright (C) 2022 HashedIn By Deloitte
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "can_bus.h"
#include "../serial_interface/serial_config.h"
#include "../main.h"
#include "../logger/logger.h"

/* mutex to lock can_module_lock */
pthread_mutex_t can_module_lock = PTHREAD_MUTEX_INITIALIZER;

#define EQUALS_SIGN 0x3D

int sockfd = 0;

/*
 * Name : update_can_error_code
 * Descriptoin: The update_can_error_code function is for updating erro codes for can struct member
 * Input parameters: struct cloud_data_struct * : clout struct to update can data member
 *                   int error_code : error code to update
 * Output parameters: void
 */
void update_can_error_code(struct cloud_data_struct *cloud_data, int error_code)
{
    struct can_data_struct can_data;

    sprintf((char *)can_data.vin, "%d", error_code);
    can_data.speed = (uint16_t)error_code;
    can_data.rpm = (float)error_code;
    sprintf(can_data.vehicle_type, "%d", error_code);
    sprintf((char *)can_data.supported_pids, "%d", error_code);

    /* update gps_data to cloud_data struct */
    pthread_mutex_lock(&can_module_lock);
    cloud_data->can_data = can_data;
    pthread_mutex_unlock(&can_module_lock);
}

/*
 * Name : get_manufacturer_detail
 * Descriptoin: The get_manufacturer_detail function is for fetching manufaturer detail & vehicle type.
 * Input parameters:
 *                  uint8_t wmi : World manufaturer Identifier
 * Output parameters: char * : manufaturer detail & vehicle type
 */
char *get_manufacturer_detail(uint8_t *wmi)
{
    for (size_t i = 0; i < WMI_LIST_LEN; i++)
    {
        char wmi_key[CAN_FRAME_LENGTH];
        memset(wmi_key, '\0', sizeof(wmi_key));

        char *manufacturer_detail = strchr(manufacturers[i], EQUALS_SIGN);

        for (size_t j = 0; j < WMI_LEN; j++)
        {
            wmi_key[j] = manufacturers[i][j];
        }

        if (strcmp((char *)wmi, wmi_key) == 0)
        {
            return manufacturer_detail + 1;
        }
    }

    return NULL;
}

/*
 * Name : validate_vin
 * Descriptoin: The validate_vin function is for validating VIN.
 * Input parameters:
 *                  char *vin : Vehicle Identification Number
 *                              Sample VIN = 5YJSA3DG9HFP14703
 * Output parameters: bool : true/false
 */
bool validate_vin(char *vin)
{
    if (strlen(vin) != VIN_LEN)
    {
        return false;
    }

    size_t sum = 0;
    for (size_t i = 0; i < VIN_LEN; i++)
    {
        size_t value;
        char vin_character = vin[i];

        /* Numerical counterparts for VIN characters */
        if (vin_character > ('A' - 1) && vin_character < ('Z' + 1))
        {
            value = values[vin_character - 'A'];
            // don't allow 0
            if (!value)
            {
                return false;
            }
        }
        /* Numerical counterparts for VIN numeric */
        else if (vin_character > ('0' - 1) && vin_character < ('9' + 1))
        {
            value = vin_character - '0';
        }
        /* return false for bad characters */
        else
        {
            return false;
        }

        /*
         * Multiply Numerical counterparts new number with the assigned weight.
         * Sum the resulting products.
         */
        sum = sum + weights[i] * value;
    }

    /* Modulus the sum of the products by 11, to find the remainder. */
    sum = sum % 11;

    /* The check digit is the character on position 9 and can be a number from 0 to 9 and X (for 10) */
    char check_digit = vin[CAN_FRAME_LENGTH];
    size_t check_digit_value = (size_t)check_digit - 48;

    if ((sum == 10 && check_digit == 'X') || (sum == check_digit_value))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Name : can_request_response
 * Descriptoin: The can_request_response function is for requesting PID data to can module and get response from the CAN module.
 * Input parameters:
 *                  struct can_frame * : response frame, updating from can response frame
 *                  size_t frame_length : number of can response frames - 3 frames for VIN, 1 frame for speed and supported pid
 *                  struct can_frame : can request frame which contains requets PID details
 * Output parameters: int retun code 0 for success and >0 value for error
 */
int can_request_response(struct can_frame *frame, size_t frame_length, struct can_frame request_frame)
{
    struct can_frame response_frame;

    /* locking read write for can data synchronization */
    pthread_mutex_lock(&can_module_lock);

    int rc = transmit_can_data(sockfd, request_frame);

    if (rc <= 0)
    {
        pthread_mutex_unlock(&can_module_lock);
        return CAN_WRITE_ERROR;
    }

    log_can_data(request_frame, CAN_REQUEST);

    for (size_t i = 0; i < frame_length; i++)
    {
        rc = receive_can_data(sockfd, &response_frame);
        if (rc <= 0)
        {
            pthread_mutex_unlock(&can_module_lock);
            return CAN_READ_ERROR;
        }
        log_can_data(response_frame, CAN_RESPONSE);
        frame[i] = response_frame;
        if (frame_length > 1)
        {
            struct can_frame vin_ack_frame;
            get_control_flow_frame(&vin_ack_frame);
            int rc = transmit_can_data(sockfd, vin_ack_frame);

            if (rc <= 0)
            {
                return CAN_WRITE_ERROR;
            }
        }
    }

    pthread_mutex_unlock(&can_module_lock);
    return CAN_SUCESS;
}

/*
 * Name : read_can_id_number
 *
 * Descriptoin: The read_can_id_number function is for fetching VIN PID data from the CAN module
 *
 * Input parameters:
 *                  void *arg : cloud_data_struct to update VIN data
 *
 * Output parameters: void
 */
void *read_can_id_number(void *arg)
{
    struct can_frame vin_frame[VIN_DATA_FRAME], request_frame;
    struct cloud_data_struct *cloud_data = (struct cloud_data_struct *)arg;

    char read_data[VIN_LEN];

    /* prepare CAN request frame */
    get_request_frame(&request_frame, VIN_PID, VIN_MODE);

    do
    {
        /* Send Request and get response for PID 0x02 */
        int rc = can_request_response(vin_frame, VIN_DATA_FRAME, request_frame);

        if (rc != CAN_SUCESS)
        {
            sprintf((char *)cloud_data->can_data.vin, "%d", rc);
            sprintf(cloud_data->can_data.vehicle_type, "%d", rc);
        }
        else if (vin_frame[0].data[3] == VIN_PID)
        {
            vin_from_can_frame_data(vin_frame, read_data);

            bool is_valid = validate_vin(read_data);

            if (is_valid)
            {
                uint8_t wmi[WMI_LEN];

                /* Extracting 1st 3 character from VIN for WMI details and assigning to wmi */
                for (size_t i = 0; i < WMI_LEN; i++)
                {
                    wmi[i] = read_data[i];
                }

                char *vehicle_detail = get_manufacturer_detail(wmi);

                if (vehicle_detail != NULL)
                {
                    strncpy(cloud_data->can_data.vehicle_type, vehicle_detail, WMI_STRING_LEN - 1);

                    /* Copy 17 byte VIN data to cloud struct member for displaying on screen from deiplay thread */
                    strncpy((char *)cloud_data->can_data.vin, read_data, MAX_LEN_VIN);

                    logger_info(CAN_LOG_MODULE_ID, "VALID CAN VIN: %s", cloud_data->can_data.vin);
                    logger_info(CAN_LOG_MODULE_ID, "VEHICLE TYPE: %s", cloud_data->can_data.vehicle_type);
                }
                else
                {
                    logger_info(CAN_LOG_MODULE_ID, "INVALID CAN VIN: %s", read_data);
                    sprintf((char *)cloud_data->can_data.vin, "%d", INVALID_VIN_ERROR);
                    sprintf(cloud_data->can_data.vehicle_type, "%d", INVALID_VIN_ERROR);
                }
            }
            else
            {
                logger_info(CAN_LOG_MODULE_ID, "INVALID CAN VIN: %s", read_data);
                sprintf((char *)cloud_data->can_data.vin, "%d", INVALID_VIN_ERROR);
                sprintf(cloud_data->can_data.vehicle_type, "%d", INVALID_VIN_ERROR);
            }
            break;
        }
        /* Retrying after 3 seconds if vin pid request faild */
        sleep(3);
    } while (cloud_data->can_data.mode == BUIL_MODE); /* mode=1 for infinite loop - build mode  || mode=0 for test mode used to test infinte loops and other cases */

    /* Retuning null to avoid control reaches end of non-void function warning */
    return NULL;
}

/*
 * Name : read_can_rpm_pid
 *
 * Description: The read_can_rpm_pid function is for fetching Vehicle rpm PID data from the CAN module
 *
 * Input parameters:
 *                  void *arg : cloud_data_struct to update vehicle rpm data
 *
 * Output parameters: void
 */
void *read_can_rpm_pid(void *arg)
{
    struct can_frame rpm_frame[RPM_DATA_FRAME], request_frame;
    struct cloud_data_struct *cloud_data = (struct cloud_data_struct *)arg;
    uint8_t read_rpm[CAN_FRAME_LENGTH];
    /* prepare CAN request frame */
    get_request_frame(&request_frame, RPM_PID, LIVE_DATA_MODE);

    do
    {
        /* Copy 2 bytes of Vehicle RPM data to cloud struct member for displaying on screen from display thread */

        if (cloud_data->can_data.supported_pids[RPM_PID_POSITION] == PID_SUPPORTED)
        {
            /* Send Request and get response for PID 0x0C */
            int rc = can_request_response(rpm_frame, RPM_DATA_FRAME, request_frame);

            if (rc != CAN_SUCESS)
            {
                cloud_data->can_data.rpm = (float)rc;
            }
            else if (rpm_frame[0].data[2] == RPM_PID)
            {
                sprintf((char *)read_rpm, "%.2X%.2X", rpm_frame[0].data[3], rpm_frame[0].data[4]);
                /* Engine speed Formula: (256 A + B)/4 */
                cloud_data->can_data.rpm = (float)(hex_to_decimal(read_rpm) * 0.25);
            }
        }
        else
        {
            cloud_data->can_data.rpm = (float)CAN_PID_NOT_SUPPORTED;
        }

        /* request next data each 1sec */
        sleep(1);
    } while (cloud_data->can_data.mode == BUIL_MODE); /* mode=1 for infinite loop - build mode  || mode=0 for test mode used to test infinte loops and other cases */
    close_socket(&sockfd);
    update_can_error_code(cloud_data, CAN_SOCKET_CLOSED);

    return NULL;
}

/*
 * Name : read_can_speed_pid
 *
 * Descriptoin: The read_can_speed_pid function is for fetching Vehicle speed PID data from the CAN module
 *
 * Input parameters:
 *                  void *arg : cloud_data_struct to update vehicle speed data
 *
 * Output parameters: void
 */
void *read_can_speed_pid(void *arg)
{
    struct can_frame speed_frame[SPEED_DATA_FRAME], request_frame;
    struct cloud_data_struct *cloud_data = (struct cloud_data_struct *)arg;

    /* prepare CAN request frame */
    get_request_frame(&request_frame, SPEED_PID, LIVE_DATA_MODE);

    do
    {
        /* Copy 1 byte (0-255) Vehicle speed data to cloud struct member for displaying on screen from deiplay thread */

        if (cloud_data->can_data.supported_pids[SPEED_PID_POSITION] == PID_SUPPORTED)
        {
            /* Send Request and get response for PID 0x0D */
            int rc = can_request_response(speed_frame, SPEED_DATA_FRAME, request_frame);

            if (rc != CAN_SUCESS)
            {
                cloud_data->can_data.speed = (uint16_t)rc;
            }
            else if (speed_frame[0].data[2] == SPEED_PID)
            {
                cloud_data->can_data.speed = (uint16_t)speed_frame[0].data[3];

                logger_info(CAN_LOG_MODULE_ID, "CAN VEHICLE SPEED: %d", cloud_data->can_data.speed);
            }
        }
        else
        {
            cloud_data->can_data.speed = (uint16_t)CAN_PID_NOT_SUPPORTED;
        }

        /* request next data each 1sec */
        sleep(1);
    } while (cloud_data->can_data.mode == BUIL_MODE); /* mode=1 for infinite loop - build mode  || mode=0 for test mode used to test infinte loops and other cases */
    close_socket(&sockfd);
    update_can_error_code(cloud_data, CAN_SOCKET_CLOSED);

    return NULL;
}

/*
 * Name : read_can_supported_pid
 *
 * Descriptoin: The read_can_supported_pid function is for fetching supported PID data from the CAN module
 *
 * Input parameters:
 *                  void *arg : cloud_data_struct to update supported pid data
 *
 * Output parameters: void
 */
void *read_can_supported_pid(void *arg)
{
    struct can_frame supported_frame[SUPPORTED_DATA_FRAME], request_frame;
    struct cloud_data_struct *cloud_data = (struct cloud_data_struct *)arg;

    /* prepare CAN request frame */
    get_request_frame(&request_frame, SUPPORTED_PID, LIVE_DATA_MODE);

    do
    {
        /* Send Request and get response for PID 0x00 */
        int rc = can_request_response(supported_frame, SUPPORTED_DATA_FRAME, request_frame);

        if (rc != CAN_SUCESS)
        {
            memset(cloud_data->can_data.supported_pids, 0, CAN_PID_LENGTH * sizeof(uint8_t));
        }
        else if (supported_frame[0].data[2] == SUPPORTED_PID)
        {
            uint8_t supported_binary_value[CAN_PID_LENGTH];

            hex_to_binary(supported_frame[0], supported_binary_value);

            for (size_t i = 0; i < CAN_PID_LENGTH; i++)
            {
                cloud_data->can_data.supported_pids[i] = supported_binary_value[i];
            }

            log_can_supported_data(supported_binary_value);
        }
        /* request next data after 30sec */
        if (cloud_data->can_data.mode == BUIL_MODE)
        {
            sleep(30);
        }
    } while (cloud_data->can_data.mode == BUIL_MODE); /* mode=1 for infinite loop - build mode  || mode=0 for test mode used to test infinte loops and other cases */
    close_socket(&sockfd);
    update_can_error_code(cloud_data, CAN_SOCKET_CLOSED);

    return NULL;
}

/*
 * Name : read_can_temperature_pid
 *
 * Descriptoin: The read_can_temperature_pid function is for fetching Vehicle temperature PID data from the CAN module
 *
 * Input parameters:
 *                  void *arg : cloud_data_struct to update vehicle temperature data
 *
 * Output parameters: void
 */
void *read_can_temperature_pid(void *arg)
{
    struct can_frame temperature_frame[TEMPERATURE_DATA_FRAME], request_frame;
    struct cloud_data_struct *cloud_data = (struct cloud_data_struct *)arg;

    /* prepare CAN request frame */
    get_request_frame(&request_frame, TEMPERATURE_PID, LIVE_DATA_MODE);

    do
    {
        /* Copy 1 byte (0-255) Vehicle speed data to cloud struct member for displaying on screen from deiplay thread */

        if (cloud_data->can_data.supported_pids[TEMPERATURE_PID_POSITION] == PID_SUPPORTED)
        {
            /* Send Request and get response for PID 0x0D */
            int rc = can_request_response(temperature_frame, TEMPERATURE_DATA_FRAME, request_frame);

            if (rc != CAN_SUCESS)
            {
                cloud_data->can_data.temperature = rc;
            }
            else if (temperature_frame[0].data[2] == TEMPERATURE_PID)
            {
                cloud_data->can_data.temperature = temperature_frame[0].data[3];

                logger_info(CAN_LOG_MODULE_ID, "CAN VEHICLE TEMPERATURE: %d", cloud_data->can_data.temperature);
            }
        }
        else
        {
            cloud_data->can_data.temperature = CAN_PID_NOT_SUPPORTED;
        }

        /* request next data each 1sec */
        sleep(1);
    } while (cloud_data->can_data.mode == BUIL_MODE); /* mode=1 for infinite loop - build mode  || mode=0 for test mode used to test infinte loops and other cases */
    close_socket(&sockfd);

    return NULL;
}

/*
 * Name : read_from_can
 *
 * Description: The read_from_can function is for fetching CAN data which contains VIN, SPEED, and supported PID data
 *
 * Input parameters:
 *                  void *arg : cloud_data_struct to update CAN data
 *                  pthread_t *read_can_supported_thread
 *                  pthread_t *read_can_speed_thread
 *                  pthread_t *read_can_vin_thread
 *                  pthread_t *read_can_rpm_thread
 *
 * Output parameters: void
 * Note: TBD optimize the function to have only one thread for all the CAN PID requests
 */
void read_from_can(void *arg, pthread_t *read_can_supported_thread, pthread_t *read_can_speed_thread, pthread_t *read_can_vin_thread, pthread_t *read_can_rpm_thread, pthread_t *read_can_temperature_thread)
{
    struct cloud_data_struct *cloud_data = (struct cloud_data_struct *)arg;
    do{
        /* setup socket can */
       if (setup_can_socket(&sockfd) == 0)
       {
            /* Thread to fetch VIN. */
            pthread_create(read_can_vin_thread, NULL, &read_can_id_number, arg);
            /* Thread to fetch supported pid data. */
            pthread_create(read_can_supported_thread, NULL, &read_can_supported_pid, arg);
            /* Thread to fetch speed pid data. */
            pthread_create(read_can_speed_thread, NULL, &read_can_speed_pid, arg);
            /* Thread to fetch rpm pid data. */
            pthread_create(read_can_rpm_thread, NULL, &read_can_rpm_pid, arg);
            /* Thread to fetch temperature pid data. */
            pthread_create(read_can_temperature_thread, NULL, &read_can_temperature_pid, arg);
       }
       else
       {
            update_can_error_code(cloud_data, CAN_SOCKET_ERROR);
       }
       sleep(3);
    }while(sockfd == 0);
}
