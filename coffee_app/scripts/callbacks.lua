local csv = require("csv")

local lang = "eng"
local lang_status = {}
lang_status["start"] = 0;
lang_status["confirmation"] = 0;
lang_status["brewing"] = 0;
lang_status["size"] = 0;

local status = "brewing_layer.status.status"
local BackendChannel = "coffeeMaker"

function LoadCSV(fname, col_num)
  local data = {}
  local column
  local k 
  
  if(col_num == nil)then
    column = 2
  else
    column = col_num
  end
  
  local f = csv.open(gre.APP_ROOT.."/csv/"..fname..".csv")
  for fields in f:lines() do
    for i, v in ipairs(fields) do 
      if(i == 1)then
        k = v
      elseif(i == column)then
        data[k]=v
      end
    end
  end
  
  return data
end

function setSizes(mapargs)
  local size_data = {}
  size_data = LoadCSV(mapargs.sizes)
  
  gre.set_data(size_data)
end


--- @param gre#context mapargs
function sendBrewingCmd(mapargs)
  local data = {}
  data["volume"] = mapargs.volume
  data["sizename"] = mapargs.sizeChoice
  
  gre.send_event_data(
    "startBrewing",
    "1u1 volume 1s0 sizename",
    data,
    BackendChannel
  )
end

--- @param gre#context mapargs
function showStatus(mapargs)
  local data = {}
  local ev_name = mapargs.context_event
  
  if (ev_name == "timer.wait") then
    if (lang == "eng") then
      data[status] = "Warning: miss connection with the coffee maker!"
    elseif (lang == "fre") then
      data[status] = "Attention: connexion manquante avec la cafetière!"
    end
  elseif (ev_name == "brewStatus") then
    local message = mapargs.context_event_data.status
    if (lang == "eng") then
      if (message == 1) then
        data[status] = "Accomplished! Hope you enjoy it!:)"
      elseif (message == 2) then
        data[status] = "Oops..Don't have enough water!"
      end
    elseif (lang == "fre") then
      if (message == 1) then
        data[status] = "Accompli! J'espère que vous apprécierez!"
      elseif (message == 2) then
        data[status] = "Oups...Il n'y a pas assez d'eau!"
      end
    end
  else
  end
  
  gre.set_data(data)
--  gre.animation_trigger("showStatus")
end

function setLangAlpha()
  local data = {}

  if (lang == "eng") then
    data["start.engshow"] = 255
    data["start.freshow"] = 128
  elseif (lang == "fre") then
    data["start.engshow"] = 128
    data["start.freshow"] = 255
  end
  
  gre.set_data(data)
end

--- @param gre#context mapargs
function setLang(mapargs)
  local data = {}
  local screen = mapargs.context_screen

  if (lang_status[screen] == 0) then 
    setLangAlpha()
    gre.animation_trigger("showlangopt")
    lang_status[screen] = 1
  else
    gre.animation_trigger("hidelangopt")
    lang_status[screen] = 0
  end
end

--- @param gre#context mapargs
function loadLang(mapargs)
  local lang_data = {}
  lang = mapargs.langchoice
  
  size_data = LoadCSV(lang)
  gre.set_data(size_data)

--  for i, v in pairs(data) do 
--    print(i.."  "..v)
--  end
end
