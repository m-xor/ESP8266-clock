# ESP8266 Clock

## Outline

### Functionality

* Just clock. Maybe alarm clock (in the future).
* 7 segment LED display
* Configuration thru http activated on demand
  - SSID, password
  - NTP server
  - time zone
* Clock updates every hour or something like that

* optional 
  - alarm function
  - alarm(s) set/unset thru http. Alarm activation/deactivation by pushbuttons
  - some kind, limited, interface by LED display/pushbuttons

### Implementation

* SDK IDF 
* WiFi, httpd, sntp - according to IDF [WiFi programming model](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/wifi.html) (blocking mode, events send to application, application calls API functions) 
* application in RTC, non blocking model [QPC framework](https://www.state-machine.com/qpc/freertos.html)