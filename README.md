# Semaphore with IR remote controler
<img src="https://github.com/JanZChlumu/Kids-Semaphore/blob/master/STL%20model/sem_view.png" width="150" height="150">

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
|0      |0x90B1027A|0x90B1027A	|0x20E9B7A|0x20E9B7A|
|1      |0xFF7A3B7A|0x90B1027A	|0xFF7A3B7A|0x20E9B7A|
|2      |0xFF7A3B7A|0x90B1027A	|0xFF7A3B7A|0x20E9B7A|
|3      |0xFF7A3B7A|            |0xFF7A3B7A|	


**Mode B**

|Rx data|UP tip	   |UP long   |	DOWN tip  |	DOWN long |
| ---   | -------- | -------- | --------- | --------- |
|0      |0xC4EF317A|0xC4EF317A|	0x364CCA7A|	0x364CCA7A|
|1      |0x33B86A7A|0xC4EF317A|	0x33B86A7A| 0x364CCA7A|
|2      |0x33B86A7A|0xC4EF317A|	0x33B86A7A|	0x364CCA7A|
|3      |0x33B86A7A|          | 0x33B86A7A|	          |


The longerst datagram (with 4 Rx data) takes approx 500ms. Folder _Semafor_IR_datagram_ contains Tx datagrams from aplication Salae Logic. 
I solved lot of problems with IRLremote library. Best solution was create big buffer, after receiving first edge timeout function is triggered. We stop receiveing after 650ms (condition ```if(IRLremote.timeout() > 650000u)``` ). Next step is tring to decode buffered data. 

For easy implementation are read and decoded only 2 first rx data. Relevant joystick lever combinations are stored in
```cpp
/*commands received from train joystick*/
const HashIR_command_t IrRxCommnad[4][2][2] = {  //[command],[transmitter mode A/B], [two first commnads ]
		{{0xC4EF317A,0x33B86A7A},{0x90B1027A,0xFF7A3B7A}} ,       /*Forward move TIP  -> Go2Stop*/
		{{0xC4EF317A,0xC4EF317A},{0x90B1027A,0x90B1027A}} ,       /*Forward move LONG -> Go2Run */
		{{0x364CCA7A,0x33B86A7A},{0x20E9B7A, 0xFF7A3B7A}} ,       /*Reverse move TIP  -> Go2Stop*/
                {{0x364CCA7A,0x364CCA7A},{0x20E9B7A, 0x20E9B7A}}};        /*Reverse move LONG -> Go2Run */
```
### Battery & discharging
Used charger is little bit stupid and doesn't solve problem with deep discharge. That's why indication function was implemented. When Lion battery reach 3,1V all semaphore functions are deactivate only orange LED is cyclicaly blinking (like in real situation - when traffic lights are out of order).
## Compilation
Project was setup for Eclipse with arduino plugin. Last [hex](Kids-Semaphore/Semaphore/Release/Semaphore.hex) file is stored in project.
## Semaphore parts
* [Lipol charger](https://www.electroschematics.com/10551/tp4056-lipo-battery-charger-rc-toys/)
* [Arduino nano](https://www.aliexpress.com/item/ATMEGA328P-Pro-Mini-328-Mini-ATMEGA328-5V-16MHz-5V-16M-for-arduino/32831029894.html?spm=a2g0s.9042311.0.0.64d94c4dOIaAYn)
* [Capacity button](https://www.aliexpress.com/item/TTP223-Module-Capacitive-Touch-Switch-Button-Self-Lock-Key-Module-2-5-5-5V/32709015595.html?spm=a2g0s.9042311.0.0.27424c4d5RztuN)
* [IR module](https://www.tme.eu/cz/Document/c26cc9aca2ad933c9d4bdc84e43ef900/TSOP2238.pdf)
* [Switch](https://www.tme.eu/cz/details/s1501/posuvne-prepinace/)
* [LEDs](https://www.tme.eu/cz/Document/01421dc8dab8fa585126521a0ba7da49/OSXXXXA1K4A.pdf)
* Battery 18650
* [STL models (see STL model)](https://www.thingiverse.com/thing:3239892)
