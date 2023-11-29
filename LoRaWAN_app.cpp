#include "LoRaWAN_app.h"
#include "lora_radio_helper.h"
using namespace events;

Lora::Lora()
{
    lorawan = new LoRaWANInterface(radio);

    //ev_queue(MAX_NUMBER_OF_EVENTS *EVENTS_EVENT_SIZE);
}

void Lora::init()
{
    // setup tracing
    setup_trace();

    // stores the status of a call to LoRaWAN protocol
    //lorawan_status_t retcode;

    // Initialize LoRaWAN stack
    if (lorawan->initialize(&ev_queue) != LORAWAN_STATUS_OK) {
        printf("\r\n LoRa initialization failed! \r\n");
        //return -1;
    }

    printf("\r\n Mbed LoRaWANStack initialized \r\n");

    
}

void Lora::connect()
{
    // stores the status of a call to LoRaWAN protocol
    lorawan_status_t retcode;

    retcode = lorawan->connect();

    // prepare application callbacks
    callbacks.events = mbed::callback(this, &Lora::lora_event_handler);
    lorawan->add_app_callbacks(&callbacks);

    // Set number of retries in case of CONFIRMED messages
    if (lorawan->set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER)
            != LORAWAN_STATUS_OK) {
        printf("\r\n set_confirmed_msg_retries failed! \r\n\r\n");
        //return -1;
    }

    printf("\r\n CONFIRMED message retries : %d \r\n",
           CONFIRMED_MSG_RETRY_COUNTER);

    // Enable adaptive data rate
    if (lorawan->enable_adaptive_datarate() != LORAWAN_STATUS_OK) {
        printf("\r\n enable_adaptive_datarate failed! \r\n");
        //return -1;
    }

    printf("\r\n Adaptive data  rate (ADR) - Enabled \r\n");

    if (retcode == LORAWAN_STATUS_OK ||
            retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS) {
    } else {
        printf("\r\n Connection error, code = %d \r\n", retcode);
        //return -1;
    }

    printf("\r\n Connection - In Progress ...\r\n");
        
    // make your event queue dispatching events forever
    ev_queue.dispatch_forever();

    //printf("fin connect\n");
}

void Lora::send_message(uint16_t message)
{
    uint16_t packet_len;
    int16_t retcode;
    uint16_t mes = message;
    //int32_t sensor_value;
     retcode = lorawan->connect();
   /* if (ds1820.begin()) {
        ds1820.startConversion();
        sensor_value = ds1820.read();
        printf("\r\n Dummy Sensor Value = %d \r\n", sensor_value);
        ds1820.startConversion();
    } else {
        printf("\r\n No sensor found \r\n");
        return;
    }

    packet_len = sprintf((char *) tx_buffer, "Dummy Sensor Value is %d",
                         sensor_value);

    retcode = lorawan->send(MBED_CONF_LORA_APP_PORT, tx_buffer, packet_len,
                           MSG_UNCONFIRMED_FLAG);*/
    
    //////PRESSION
    //float pression = capt_pression.pressure();
    //packet_len = sprintf((char *) tx_buffer, "{\"pression\": %.2f}", pression);
    //lorawan->send(MBED_CONF_LORA_APP_PORT, tx_buffer, packet_len, MSG_UNCONFIRMED_FLAG);

    //////TEMPERATURE
    //Temp AS(P1_I2C_SDA, P1_I2C_SCL);
    //float temp = AS.read(AS.addr8bit, AS.cmd, AS.temp);
    //packet_len = sprintf((char *) tx_buffer, "{\"temp\": %.2f}", temp);
    packet_len = message;
    lorawan->send(MBED_CONF_LORA_APP_PORT, tx_buffer, packet_len, MSG_UNCONFIRMED_FLAG);

    if (retcode < 0) {
        retcode == LORAWAN_STATUS_WOULD_BLOCK ? printf("send - WOULD BLOCK\r\n")
        : printf("\r\n send() - Error code %d \r\n", retcode);

        if (retcode == LORAWAN_STATUS_WOULD_BLOCK) {
            //retry in 3 seconds
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                ev_queue.call_in(3s, callback(this, &Lora::send_message), mes);
            }
        }
        return;
    }

    printf("\r\n %d bytes scheduled for transmission \r\n", retcode);
    memset(tx_buffer, 0, sizeof(tx_buffer));
}

/**
 * Receive a message from the Network Server
 */
void Lora::receive_message()
{
    uint8_t port;
    int flags;
    int16_t retcode = lorawan->receive(rx_buffer, sizeof(rx_buffer), port, flags);

    if (retcode < 0) {
        printf("\r\n receive() - Error code %d \r\n", retcode);
        return;
    }

    printf(" RX Data on port %u (%d bytes): ", port, retcode);
    for (uint8_t i = 0; i < retcode; i++) {
        printf("%02x ", rx_buffer[i]);
    }
    printf("\r\n");
    
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

/**
 * Event handler
 */
void Lora::lora_event_handler(lorawan_event_t event)
{
    uint16_t mes = sprintf((char *) tx_buffer, "{}");
    switch (event) {
        case CONNECTED:
            printf("\r\n Connection - Successful \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message(message);
            } else {
                ev_queue.call_every(TX_TIMER, callback(this, &Lora::send_message), mes);
            }

            break;
        case DISCONNECTED:
            ev_queue.break_dispatch();
            printf("\r\n Disconnected Successfully \r\n");
            break;
        case TX_DONE:
            printf("\r\n Message Sent to Network Server \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message(message);
            }
            break;
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            printf("\r\n Transmission Error - EventCode = %d \r\n", event);
            // try again
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message(message);
            }
            break;
        case RX_DONE:
            printf("\r\n Received message from Network Server \r\n");
            receive_message();
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
            printf("\r\n Error in reception - Code = %d \r\n", event);
            break;
        case JOIN_FAILURE:
            printf("\r\n OTAA Failed - Check Keys \r\n");
            break;
        case UPLINK_REQUIRED:
            printf("\r\n Uplink required by NS \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message(message);
            }
            break;
        default:
            MBED_ASSERT("Unknown Event");
    }
}