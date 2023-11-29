#ifndef LORAWAN
#define LORAWAN
#include "mbed.h"
#include "lorawan/LoRaWANInterface.h"
#include "lorawan/system/lorawan_data_structures.h"
//#include "events/EventQueue.h"
//#include "DummySensor.h"
#include "trace_helper.h"

#include "mbed_events.h"

using namespace events;

#define TX_TIMER 10s
#define MAX_NUMBER_OF_EVENTS 10
#define CONFIRMED_MSG_RETRY_COUNTER 3
//#define PC_9 0



class Lora{
    public:

        uint16_t message;
        lorawan_event_t event;

        void init();
        void connect();
        void send_message(uint16_t message);
        void receive_message();
        void lora_event_handler(lorawan_event_t event);

        Lora();

    private: 
        uint8_t tx_buffer[30];
        uint8_t rx_buffer[30];
        //EventQueue *ev_queue1;
        LoRaWANInterface *lorawan;
        lorawan_app_callbacks_t callbacks;
        EventQueue ev_queue;

};

#endif