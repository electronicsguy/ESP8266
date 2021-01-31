# ESP8266

@peril99 added a post method to this fork for usa in the arduino projects which need an https / post method. My use case was IOT projects based on arduino clones like esp8266 / esp32. These are the key sensors / affectors which sit outside and controls pumps teeny pumps (affectors) and ph sensors (atlas ezo). 

These have all been tied together in a rudimentary dashboard for visualization @ https://admin.tago.io/public/dashboard/5f35d9868411cc00274b6a8a/e155a708-01a8-4fbc-8227-74e2b4e79855


to use this library with the arduino ide

1) clone a copy of this repo in a spot where you arduino ide can reference the libaries folder for me that was C:\Users\xxx\Documents\Arduino\libraries\HTTPSRedirect
2) include this library in the project you are including with a "#include <HTTPSRedirect.h>"

