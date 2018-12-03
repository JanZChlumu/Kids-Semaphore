# Semaphore with IR remote controler
<img src="https://github.com/JanZChlumu/Kids-Semaphore/blob/master/STL%20model/sem_view.png" width="150" height="150"><img src="https://github.com/JanZChlumu/Kids-Semaphore/blob/master/STL%20model/sem_cutted.png" width="150" height="150" alt="cutout">
[Demonstration video (YouTube)](https://youtu.be/u1l54P9FhmE)

**Motivation**: create toy for 2 years child for increasing knowlage about behavior on crossroads :)

## Description:
Semaphore leg is designed to be LEGO Duplo compatible. It could be connected with bricks.

Toy has 2 modes:

- **Manual**: By touching the cover, colours are changed.
- **IR mode**: Semaphore is driven by receiving signals from remote controller. It is used IR Tx joystick from this toy [ **Train with remote IR control**](https://www.lidl-shop.cz/PLAYTIVE-JUNIOR-Vlacek-na-dalkove-ovladani/p100246613)

### Manual control
Is really simple to chang states by touching semaphore head: Stop (red) -> Ready (orange) -> Go (green) -> Prepare for stop (red+orange).

### IR control description
Arduino library was used for deconding IR [IRLremote](https://github.com/NicoHood/IRLremote/blob/master/Readme.md) in mode _CHashIR_. Joystick transmitter has not standard Tx format, that means, _with different libraries, differend results will be obtained_. It's recomended to test receiveing data with your transmitter. For this situation is designed macro for debug
```cpp
#define DEBUG //Comment this for stop debuging
```
Joystick has two modes A or B with different transmited data. Lever has 4 states. 
- UP
  - Tip position (stop the train)
  - Hold long (run the train)
- DOWN
  - Tip position (stop the train)
  - Hold long (run the train)
  
 With given library was received folowing datagrams. 

**Mode A**

|Rx data|UP tip	   |UP long   |	DOWN tip  |	DOWN long |
| ---   | -------- | -------- | --------- | --------- |
|0      |0x90B1027A|**0x90B1027A**	|0x20E9B7A|**0x20E9B7A**|
|1      |**0xFF7A3B7A**|**0x90B1027A**	|**0xFF7A3B7A**|**0x20E9B7A**|
|2      |**0xFF7A3B7A**|**0x90B1027A**	|**0xFF7A3B7A**|**0x20E9B7A**|
|3      |**0xFF7A3B7A**|            |**0xFF7A3B7A**|	


**Mode B**

|Rx data|UP tip	   |UP long   |	DOWN tip  |	DOWN long |
| ---   | -------- | -------- | --------- | --------- |
|0      |0xC4EF317A    |**0xC4EF317A**|	    0x364CCA7A|	**0x364CCA7A**|
|1      |**0x33B86A7A**|**0xC4EF317A**|	**0x33B86A7A**|     **0x364CCA7A**|
|2      |**0x33B86A7A**|**0xC4EF317A**|	**0x33B86A7A**|	**0x364CCA7A**|
|3      |**0x33B86A7A**|              | **0x33B86A7A**|	          |


When we look on data we'll see that the best solution how to decode position is read last 3 data datagrams (bold in tables).
The relevant joystick lever combinations are stored in:
```cpp
/*commands received from joystick*/
const HashIR_command_t IrRxCommnad[2][4] = {  //[command],[transmitter mode A/B]
    //  { mode A UP,  mode A DOWN,    mode B UP,   mode B DOWN}
  	{0xFF7A3B7A,   0xFF7A3B7A,   0x33B86A7A,   0x33B86A7A},   /*Go2Stop (all TIP positions)*/
	{0x90B1027A,    0x20E9B7A,   0xC4EF317A,   0x364CCA7A}};  /*Go2Run (all long hold positions)*/
```
### Battery & discharging
Used charger is little bit stupid and doesn't solve problem with deep discharge. That's why indication function was implemented. When Lion battery reach 3,1V all semaphore functions are deactivate only orange LED is cyclicaly blinking (like in real situation - when traffic lights are out of order) and 15 minutes power off mode is activated.

Other case is when child forgot on the toy for 15 minutes, semaphore goes over sleep mode. 

Minimum power consumption for bought situations is 250uA.
For increasing low power consumption was removed [LDO regulator](http://ww1.microchip.com/downloads/en/DeviceDoc/20005785A.pdf), power on LED from Arduino Mini, IR sensor and capapity button is disconned form power supply ( see [schematic](https://github.com/JanZChlumu/Kids-Semaphore/blob/master/miscellaneous/Semaphore_schematic.pdf)). 
## Compilation
Project was setup for Eclipse with arduino plugin. Last [hex](https://github.com/JanZChlumu/Kids-Semaphore/blob/master/Semaphore/Release/Semaphore.hex) file is stored in project.
## Semaphore parts
* [Lipol charger](https://www.electroschematics.com/10551/tp4056-lipo-battery-charger-rc-toys/)
* [Arduino mini](https://www.aliexpress.com/item/ATMEGA328P-Pro-Mini-328-Mini-ATMEGA328-5V-16MHz-5V-16M-for-arduino/32831029894.html?spm=a2g0s.9042311.0.0.64d94c4dOIaAYn), [schema](https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/Arduino-Pro-Mini-v14.pdf)
* [Capacity button](https://www.aliexpress.com/item/TTP223-Module-Capacitive-Touch-Switch-Button-Self-Lock-Key-Module-2-5-5-5V/32709015595.html?spm=a2g0s.9042311.0.0.27424c4d5RztuN)
* [IR module](https://www.tme.eu/cz/Document/c26cc9aca2ad933c9d4bdc84e43ef900/TSOP2238.pdf)
* [Switch](https://www.tme.eu/cz/details/s1501/posuvne-prepinace/)
* [LEDs](https://www.tme.eu/cz/Document/01421dc8dab8fa585126521a0ba7da49/OSXXXXA1K4A.pdf)
* Battery 18650 (inserted in leg)
* [STL models (see STL model)](https://www.thingiverse.com/thing:3239892)

## Other links
* [Schematic](https://github.com/JanZChlumu/Kids-Semaphore/blob/master/miscellaneous/Semaphore_schematic.pdf)
* [Final product picture](https://github.com/JanZChlumu/Kids-Semaphore/blob/master/miscellaneous/finished_set.jpg)
