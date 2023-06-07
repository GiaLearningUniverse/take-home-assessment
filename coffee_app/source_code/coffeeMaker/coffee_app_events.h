// Incoming events
#define BREWSTATUS_EVENT "brewStatus"
#define BREWSTATUS_FMT "1u1 status"
typedef struct {
	uint8_t status;
} brewstatus_event_t;

// Outgoing events
#define TIMER__REST_EVENT "timer.rest"

#define TIMER__WAIT_EVENT "timer.wait"

#define STARTBREWING_EVENT "startBrewing"
#define STARTBREWING_FMT "1u1 volume 1u1 sizename"
typedef struct {
	uint8_t volume;
	uint8_t sizename;
} startbrewing_event_t;

