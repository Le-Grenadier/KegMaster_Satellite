[EasyEDA Link to order PCB]|(https://easyeda.com/GrenardJ/kegmaster_satellitesmt)

Satellite PIC Application

Each PIC MCU manages hardware interface for a different keg. This includes GPIO to actuate valves, interrupt on change pins to data from the flow meter, logging and initiating ADC readings for the pressure sensor, reading data from the external HX711 24-bit ADC for the load-cell within the scale, and control output for the WS2812b LED driver. 

To improve overall system safety and security, a configurable dwell time is used to control how long a liquid or gas valve will remain open. This was implemented to help make the system resilient to loss of connection with the main controller and maintain security against attempts at physical intrusion. 
**Interface and Control Documentation**
* Each PIC shall have a base address of 0x08.
* Each PIC has two resistors, R20 and R22 on the PCB, to set the address offset. These shall be read as a two bit number (0-3) with R20 as the LSB.
* The following messages shall be defined for communication. If a response is indicated, the same format shall be used for both Tx and Rx. Pin Id’s may be found on the schematic within the Satellite PIC repository.
||Description||Message ID||(8-bit)Data Payload||Response||Notes||
|GPIO Read|0|Output Id (8-bit)\\State (8-bit)\\Hold Time (16-bit)||Yes||State and Hold time ignored on Rx.|
|GPIO Set|1|Output Id (8-bit)\\State (8-bit)\\Hold Time (16-bit)|No|
|GPIO Set Default|2|Output Id (8-bit)\\State (8-bit)\\Hold Time (16-bit)|No|Hold time is ignored.|
|Interrupt Read|3|Interrupt Id (8-bit)\\Count (16-bit)|Yes|Count ignored on Rx.|
|Interrupt Reset|4|Interrupt Id (8-bit)\\Count (16-bit)|No|Count ignored.|
|ADC Read|5|ADC Id (8-bit)\\Value (32-bit)|Yes|Value ignored on Rx.|
|LED Set Data|6|Led Cnt (8-bit)\\Led State[0..n]{\\   Red (8-bit)\\   Green (8-bit)\\   Blue (8-bit)\\   White (8-bit)\\}|No|LED State is a variable size array and can contain info for up to 20 LEDs. \\‘White’ ignored.|
|LED Set Breathe|7|State (8-bit)|No||
* Each message shall be terminated with the following 16-bit sequence 0x04FF. This was intended for the ability to send multiple messages in one transmission, however, this feature is untested. Messages without this terminator will be ignored. 

