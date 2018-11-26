# Semaphore with IR remote controler
<img src="https://github.com/JanZChlumu/Kids-Semaphore/blob/master/STL%20model/sem_view.png" width="150" height="150">

**Motivation**: create toy for 2 years baby for increasing knowlage about behavior in crossroads :)

## Description:
Semaphore leg is designed like LEGO Duplo compatible, could be connected with bricks.

Toy has 2 modes:

- **Manual**: By touching the cover, colours are changed.
- **IR mode**: Semaphore is driven by receiving signals from remote controller. Was used IR Tx joystick from this toy [ **Train with remote IR control**](https://www.lidl-shop.cz/PLAYTIVE-JUNIOR-Vlacek-na-dalkove-ovladani/p100246613)

## Semaphore parts
* [Lipol charger](https://www.electroschematics.com/10551/tp4056-lipo-battery-charger-rc-toys/)
* [Arduino nano](https://www.aliexpress.com/item/ATMEGA328P-Pro-Mini-328-Mini-ATMEGA328-5V-16MHz-5V-16M-for-arduino/32831029894.html?spm=a2g0s.9042311.0.0.64d94c4dOIaAYn)
* [Capacity button](https://www.aliexpress.com/item/TTP223-Module-Capacitive-Touch-Switch-Button-Self-Lock-Key-Module-2-5-5-5V/32709015595.html?spm=a2g0s.9042311.0.0.27424c4d5RztuN)
* [IR module] (https://www.tme.eu/cz/Document/c26cc9aca2ad933c9d4bdc84e43ef900/TSOP2238.pdf)
* [Switch](https://www.tme.eu/cz/details/s1501/posuvne-prepinace/)

### Manual control
Is really simple by touching semaphore head are cyclicaly changed states: Stop (red), Ready (orange), Go (green), Prepare for stop (red+orange).

### IR control description
For decoding IR was used arduino library [IRLremote](https://github.com/NicoHood/IRLremote/blob/master/Readme.md) in mode _CHashIR_. Joystick transmitter has not standard Tx format, that means, _with different libraries, differend results will be obtained_. It's recomended to test receiveing data with your transmitter. For this situation is designed macro for debug
```cpp
#define DEBUG //Comment this for stop debuging
```
Joystick has two modes A or B. Lever has 4 states. 
- UP
  - Tip position (stop the train)
  - Long hold (run the train)
- DOWN
  - Tip position (stop the train)
  - Long hold (run the train)
  
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

For easy implementation are read and decoded only 2 first rx data. Relevant combinatis are stored in
```cpp
/*commands received from train joystick*/
const HashIR_command_t IrRxCommnad[4][2][2] = {  //[command],[transmitter mode A/B], [two first commnads ]
		{{0xC4EF317A,0x33B86A7A},{0x90B1027A,0xFF7A3B7A}} ,       /*Forward move TIP  -> Go2Stop*/
		{{0xC4EF317A,0xC4EF317A},{0x90B1027A,0x90B1027A}} ,       /*Forward move LONG -> Go2Run */
		{{0x364CCA7A,0x33B86A7A},{0x20E9B7A, 0xFF7A3B7A}} ,       /*Reverse move TIP  -> Go2Stop*/
                {{0x364CCA7A,0x364CCA7A},{0x20E9B7A, 0x20E9B7A}}};        /*Reverse move LONG -> Go2Run */
```
