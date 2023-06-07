#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <gre/greio.h>

#include "coffee_app_events.h"

#define COFFEE_SEND_CHANNEL "coffeeApp_frontend"
#define COFFEE_RECEIVE_CHANNEL "coffeeMaker"

#define SNOOZE_TIME 80

using namespace std;

mutex mtx;
float waterRemain = 20;
uint8_t sendMessage;
int sendFlag;

int receive_thread()
{
	gre_io_t* rcv_handle;
	gre_io_serialized_data_t* nbuffer = NULL;
	char* event_addr;
	char* event_name;
	char* event_format;
	void* event_data;
	int	ret;
	int nbytes;

	rcv_handle = gre_io_open(COFFEE_RECEIVE_CHANNEL, GRE_IO_TYPE_RDONLY);
	if (rcv_handle == NULL) 
	{
		fprintf(stderr, "Can't open receive channel\n");
		return 0;
	}
	cout << "Receive channel opened" << endl;

	nbuffer = gre_io_size_buffer(NULL, 100);

	while (true)
	{
		ret = gre_io_receive(rcv_handle, &nbuffer);

		if (ret < 0) {
			cout << "no message" << endl;
			continue;
		}

		event_name = NULL;
		nbytes = gre_io_unserialize(nbuffer, &event_addr, &event_name, &event_format, &event_data);

		if (!event_name) 
		{
			printf("Missing event name\n");
			return 0;
		}
		
		if (strcmp(event_name, STARTBREWING_EVENT) == 0)
		{
			startbrewing_event_t *rcvdata = (startbrewing_event_t *)event_data;
			waterRemain -= rcvdata->volume;
			if (waterRemain >= 0)
			{
				std::this_thread::sleep_for(std::chrono::seconds(8));
				cout << "make " << rcvdata->sizename << " size coffee" << endl;
				uint8_t size = rcvdata->sizename;
				mtx.lock();
				sendMessage = 1;//"Your " + size + " size coffee is ready";
				sendFlag = 1;
				mtx.unlock();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				mtx.lock();
				sendMessage = 2;//"Oops..Don't have enough water!";
				sendFlag = 1;
				mtx.unlock();
				cout << "unlocked message: 2" << endl;
			}
		}
		
	}

	gre_io_free_buffer(nbuffer);
	gre_io_close(rcv_handle);
}

void send_thread(gre_io_t* send_handle)
{
	gre_io_serialized_data_t* nbuffer = NULL;
	brewstatus_event_t event_data;
	
	while (true)
	{
		if (sendFlag == 1)
		{
			mtx.lock();
			event_data.status = sendMessage;
			mtx.unlock();
			cout << event_data.status << endl;
			nbuffer = gre_io_serialize(nbuffer, NULL, BREWSTATUS_EVENT, BREWSTATUS_FMT, &event_data, sizeof(event_data));

			int ret = gre_io_send(send_handle, nbuffer);
			if (ret < 0) {
				fprintf(stderr, "Send failed, exiting\n");
			}
			cout << "send message: " << to_string(sendMessage) << endl;
			mtx.lock();
			sendFlag = 0;
			mtx.unlock();
		}
	}

	gre_io_free_buffer(nbuffer);
	gre_io_close(send_handle);
}

int main(int argc, char** argv)
{
	gre_io_t* send_handle;
	
	cout << "Trying to open the connection to the frontend\n" << endl;
	while (true) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(SNOOZE_TIME));
		send_handle = gre_io_open(COFFEE_SEND_CHANNEL, GRE_IO_TYPE_WRONLY);
		if (send_handle != NULL) 
		{
			printf("Send channel: %s successfully opened\n", COFFEE_SEND_CHANNEL);
			break;
		}
	}

	try
	{
		thread rcvthread(receive_thread);
		thread sendthread(send_thread, send_handle);
		rcvthread.join();
		sendthread.join();
	}
	catch (const system_error& e)
	{
		cout << "Failed to create the thread: " << e.what() << endl;
	}

	return 0;
}