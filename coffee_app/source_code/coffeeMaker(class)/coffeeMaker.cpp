#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include <memory>
#include <functional>
#include <gre/greio.h>

#include "coffee_app_events.h"

#define COFFEE_SEND_CHANNEL "coffeeApp_frontend"
#define COFFEE_RECEIVE_CHANNEL "coffeeMaker"
#define SNOOZE_TIME 80

using namespace std;

class CoffeeMaker 
{
private:
    mutex mtx;
    condition_variable cv;
    float waterRemain = 10;
    uint8_t sendMessage = 0;
    bool sendFlag = false;
    gre_io_t* send_handle;
    gre_io_t* rcv_handle;

public:
    CoffeeMaker() 
    {
        cout << "Trying to open the connection to the frontend\n" << endl;

        while (true) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(SNOOZE_TIME));
            send_handle = gre_io_open(COFFEE_SEND_CHANNEL, GRE_IO_TYPE_WRONLY);
            if (send_handle) 
            {
                printf("Send channel: %s successfully opened\n", COFFEE_SEND_CHANNEL);
                break;
            }
        }
        rcv_handle = gre_io_open(COFFEE_RECEIVE_CHANNEL, GRE_IO_TYPE_RDONLY);
        if (!rcv_handle) 
        {
            fprintf(stderr, "Can't open receive channel\n");
            throw std::runtime_error("Can't open receive channel");
        }
        cout << "Receive channel opened" << endl;
    }

    void receive_thread() 
    {
        gre_io_serialized_data_t* nbuffer = NULL;
        char* event_addr;
        char* event_name;
        char* event_format;
        void* event_data;
        int ret;
        int nbytes;

        nbuffer = gre_io_size_buffer(NULL, 100);

        while (true) 
        {
            ret = gre_io_receive(rcv_handle, &nbuffer);

            if (ret < 0) 
            {
                cout << "no message" << endl;
                continue;
            }

            event_name = NULL;
            nbytes = gre_io_unserialize(nbuffer, &event_addr, &event_name, &event_format, &event_data);

            if (!event_name) 
            {
                printf("Missing event name\n");
                return;
            }

            if (strcmp(event_name, STARTBREWING_EVENT) == 0) 
            {
                startbrewing_event_t* rcvdata = (startbrewing_event_t*)event_data;
                waterRemain -= rcvdata->volume;
                if (waterRemain >= 0) 
                {
                    std::this_thread::sleep_for(std::chrono::seconds(8));
                    cout << "make "<< rcvdata->sizename << " size coffee" << endl;
                    cout << "water remain: "<< waterRemain << endl;
                    uint8_t size = rcvdata->sizename;
                    unique_lock<mutex> lock(mtx);
                    sendMessage = 1;//"Your " + size + " size coffee is ready";
                    sendFlag = true;
                    cv.notify_one();
                }
                else 
                {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    unique_lock<mutex> lock(mtx);
                    sendMessage = 2;//"Oops..Don't have enough water!";
                    sendFlag = true;
                    cv.notify_one();
                }
            }
        }

        gre_io_free_buffer(nbuffer);
        gre_io_close(rcv_handle);
    }

    void send_thread() 
    {
        gre_io_serialized_data_t* nbuffer = NULL;
        brewstatus_event_t event_data;

        while (true) 
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [this] { return sendFlag; });
            event_data.status = sendMessage;
            nbuffer = gre_io_serialize(nbuffer, NULL, BREWSTATUS_EVENT, BREWSTATUS_FMT, &event_data, sizeof(event_data));
            int ret = gre_io_send(send_handle, nbuffer);
            if (ret < 0) 
            {
                fprintf(stderr, "Send failed, exiting\n");
            }
            sendFlag = false;
        }

        gre_io_free_buffer(nbuffer);
        gre_io_close(send_handle);
    }

    void run() 
    {
        try 
        {
            cout << "enter run" << endl;
            thread rcvthread(&CoffeeMaker::receive_thread, this);
            thread sendthread(&CoffeeMaker::send_thread, this);
            rcvthread.join();
            sendthread.join();
        }
        catch (const system_error& e) 
        {
            cout << "Failed to create the thread: " << e.what() << endl;
        }
    }
};

int main(int argc, char** argv) 
{
    CoffeeMaker coffeeMaker;
    coffeeMaker.run();
    return 0;
}

