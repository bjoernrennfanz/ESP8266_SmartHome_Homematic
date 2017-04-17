esp8266-smarthome-bridge
========================

ESP8266 smarthome gateway for SPI or DHT sensors

This firmware for the ESP creates a TCP socket to control it over Telnet via AT commands. With the AT commands you can control SPI, DHT sensors or GPIO pins.

Telnet into the module and issue commands prefixed by AT. The avaiable generic commands are:
```
AT                                    # do nothing, print OK
AT+MODE                               # print current opmode
AT+MODE <mode>                        # set current opmode,
                                      # mode: 1=STA, 2=AP, 3=both
AT+STA                                # print current ssid and password connected to
AT+STA <ssid> <password>              # set ssid and password to connect to
AT+AP                                 # print the current soft ap settings
AT+AP <ssid>                          # set the AP as open with specified ssid
AT+AP <ssid> <pw> [<authmode> [<ch>]] # set the AP ssid and password, 
                                      # authmode:1=WEP,2=WPA,3=WPA2,4=WPA+WPA2,
                                      # channel: 1..13
AT+TCPPORT                            # print current incoming TCP socket port
AT+TCPPORT <port>                     # set current incoming TCP socket port (restarts ESP8266)
AT+RESET                              # software reset the unit
AT+SHOWIP                             # display Station IP Address, gateway and netmask
AT+SHOWMAC                            # display Station MAC.
AT+SCAN                               # display available networks around

The avaiable SPI commands are:
```
AT+SPISTART                           # starts the communication via SPI
AT+SPISEND <byte> <byte> <...>        # sends the data via SPI
AT+SPIEND                             # ends the communication via SPI

The avaiable DHT commands are:
```
AT+DHTREAD                            # read the current temperature and humidity from DHT sensor