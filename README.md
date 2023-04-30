# CarCluster

**Control car instrument clusters from your computer for gaming or other purposes using an ESP32.**

![Main image](https://github.com/r00li/CarCluster/blob/main/Misc/main_display.jpg?raw=true)

## Tell me more?

Ever wondered if you could use a car instrument cluster outside of your car? Well... the answer to that question is yes. The aim of this project is to give you the ability to run a real instrument cluster from a real car on your desk. The main purpose of this is gaming, but the possibilities are endless. You might want to use it as a clock. Or maybe you want to show the amount of unread emails - possibilities are endless.

Here is a little demo video of me playing forza Horizon 4:
[![](https://github.com/r00li/CarCluster/blob/main/Misc/video_thumb.png?raw=true)](https://youtu.be/-aPPZKZ644M)

## What do I need?
The hardware for this is pretty straightforward. You will need:

 - ESP32 board (I am using a generic Devkit V1 board - available from ebay for around 10€)
 - MCP2515 CAN bus module (I am using a generic one from ebay - available for around 5€)
 - 12V power supply (doesn't have to be too powerful. But 1A or more is probably what you want)
 - Car instrument cluster supported by this project
 - Some wires, pin headers and other stuff to wire everything together

### Supported instrument clusters
Currently fully tested and supported are the following instrument clusters:

- VW Polo 6R (Also known as Polo mark 5)

Note that some other VW instrument clusters from a similar vintage car will probably work as well. Generally we are talking about PQ platform cars. However not all functionality might work without some modifications. Newer MQB platform cars will sadly need a lot more modifications.

You can get these from ebay, your local scrapyard, or if you are in EU: [rrr.lt](rrr.lt/) . They should cost you around 50€,

### Supported games
If you want to use the instrument cluster for gaming then currently supported are the following games:

- Forza Horizon 4 (Horizon 5 should also work, as should Forza Motorsport 7)
  
## Set it up
### Wiring it all together
Below is the connector pinout for the Polo 6R dashboard:
![Pinout indication](https://github.com/r00li/CarCluster/blob/main/Misc/pinout.jpg?raw=true)You can also see my improvised connector. Just two rows of standard 2.54mm female pin headers glued together. If you can I would suggest you get a cluster that comes with it's original connector.

Wire everything according to the tables below:

| Cluster pin | Connect to | Comment |
|--|--|--|
| 31, 32 | +12V |
| 16 | GND (12V power supply) |
| 20 | GND (ESP) |
| 28 | CAN H (connect to your can bus interface) |
| 29 | CAN L (connect to your can bus interface) |
| 27 | ESP pin D15 | Oil pressure switch |
| 17 | ESP pin D4 | Sprinkler water level sensor |
| 18 | ESP pin D16 | Coolant shortage indicator |
| 1 | - not connected for now - | Fuel level indicator pin 1 |
| 2 | - not connected for now - | Fuel level indicator pin 2 |
| 21 | Through a push button to ESP GND | Optional: Trip computer menu up |
| 22 | Through a push button to ESP GND | Optional: Trip computer menu set/reset |
| 23 | Through a push button to ESP GND | Optional: Trip computer menu down |
| 19 | Through a NTC (10k?) | Optional: outside temperature sensor |

Connect the CAN bus interface to ESP32 according to this:
| Can bus interface pin | Connect to | Comment |
|--|--|--|
| INT | ESP Pin D2 |
| CS | ESP Pin D5 |
| SCK | ESP Pin D18 |
| MISO | ESP Pin D19 |
| MOSI | ESP Pin 23 |
| GND | ESP GND |
| VCC | ESP VIN | This is the 5V line when ESP is powered from USB |

### Install the arduino sketch to the ESP32
Navigate to the Dashboard Sketches folder of this project and select the sketch for the dashboard that you are using. Download the sketch folder and open it using Arduino IDE (I am using version 2.1.0).

If you haven't yet installed ESP32 support for Arduino IDE then do so now. Go to Boards Manager and search for esp32 by Espressif Systems. I am using version 2.0.7.

Finally you will need to install a few libraries:

 - [mcp_can by coryjfowler](https://github.com/coryjfowler/MCP_CAN_lib) - install through library manager - tested using 1.5.0
 - [ArduinoJson](https://arduinojson.org/) - install through library manager - tested using 6.20.1
 - [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) - install manually by downloading from github and placing it into your arduino IDE libraries folder
 - [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - install manually by downloading from github and placing it into your arduino IDE libraries folder
 - [ESPDash](https://docs.espdash.pro/) - install through library manager - tested using 4.0.1

Now that you have done that you just need to enter your wifi network name and wifi password into the code (search for `User configuration`), select your board from the arduino menu and upload it to your board. 

*Note that you might have some trouble uploading the code to the arduino while the CAN interface is connected. If you do, disconnect the CAN interface and try uploading again.*  

### Use the web interface to test the basic functionality

![Web dashboard](https://github.com/r00li/CarCluster/blob/main/Misc/screenshot_dashboard.png?raw=true)
This project contains a small web server that you can use to control some basic functionality of the dashboard. You can set the RPM, speed and some of the other parameters using it. Simply open `http://<ip-of-your-ESP32>/` in your web browser and you should be able to see the above interface. Move the sliders and click the buttons to control the dashboard display.

### Use telemetry data from your game
*For forza Horizon 4/5*:
1.  Launch the game and proceed through the menus until you can drive your car
2.  Pause the game and navigate to the Settings menu
3.  Navigate to HUD and Gameplay
4.  Set `Data Out` to `ON`
5.  Set `Data Out IP Address` to `ip-of-your-ESP32`
6.  Set `Data Out IP Port` to `1101` (or other if you changed the IP in the code)

Whenever you now run the game you should see the instrument cluster showing the data from the game. 

### Known issues and limitations
For the most part everything should work without issues. However there are a few small things still being worked on:

- Speed needle sometimes drops for a second or two. This is fairly infrequent, but it does sometimes happen.
- Fuel indicator doesn't work. Currently you can wire a 200 ohm potentiometer between pins 1 and 2 (middle pin of the pot should be connected to ESP GND) to set a fixed value. Working on a solution with a digital pot to fix this.
- Traveled trip distance shown on the display is incorrect (this is connected to the speed needle issue, however not exactly sure how)
- Handbrake indicator causes issues when being show for a long time (keeping this indicator lit for more than a few seconds at a time causes the dashboard to go crazy). Code currently handles this by turning the indicator off after a few seconds, so you should not have any issues.

## Help and support

If you need help with anything contact me through github (open an issue here). You can also check my website for other contact information: http://www.r00li.com .

If you want me to add support for a game or a specific instrument cluster you can also [donate to this project using Paypal](https://www.paypal.com/donate/?hosted_button_id=NVTTFT7Z6KG3E). This will not guarantee that I will add support, but it will certainly give me more incentive to do so.

## License

This work is licensed under GNU GPL version 3. See LICENSE file for more details