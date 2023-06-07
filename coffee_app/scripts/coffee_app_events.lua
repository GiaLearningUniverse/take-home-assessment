-- Incoming events
local brewstatus_event = "brewStatus"
local brewstatus_fmt = "1s0 status"
local brewstatus_event_table = {}
brewstatus_event_table["status"] = ""

-- Outgoing events
local timer__rest_event = "timer.rest"

local startbrewing_event = "startBrewing"
local startbrewing_fmt = "1u1 volume 1s0 sizename"
local startbrewing_event_table = {}
startbrewing_event_table["volume"] = 0
startbrewing_event_table["sizename"] = ""

