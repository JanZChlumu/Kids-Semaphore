# Semaphore with IR remote controler

**Motivation**: create toy for 2 years baby for increasing knowlage about behavior in crossroads :)

## Description:
Semaphore leg is designed like LEGO Duplo compatible, could be connected with bricks.

Toy has 2 modes:

- **Manual**: By touching the cover, colours are changed.
- **IR mode**: Semaphore is driven by receiving signals from remote controller. Was used IR Tx joystick from this toy [ **Train with remote IR control**](https://www.lidl-shop.cz/PLAYTIVE-JUNIOR-Vlacek-na-dalkove-ovladani/p100246613)

## Semaphore parts
[Lipol charger](https://www.electroschematics.com/10551/tp4056-lipo-battery-charger-rc-toys/)

## IR description
For decoding IR was used arduino library [IRLremote](https://github.com/NicoHood/IRLremote/blob/master/Readme.md) in mode _CHashIR_. Joystick transmitter has not standard Tx format, that means, with different libraries, differend results will be obtained.

Joystick has two modes A or B. With given library was received folowing datagrams. 
**Mode A**

|Rx data|UP tip	   |UP long   |	DOWN tip  |	DOWN long |
| ---   | -------- | -------- | --------- | --------- |
|0      |0x90B1027A	|0x90B1027A	|0x20E9B7A	|0x20E9B7A|
|1      |0xFF7A3B7A	|0x90B1027A	|0xFF7A3B7A	|0x20E9B7A|
|2      |0xFF7A3B7A	|0x90B1027A	|0xFF7A3B7A	|0x20E9B7A|
|3      |0xFF7A3B7A |           |0xFF7A3B7A|	


**Mode B**

|Rx data|UP tip	   |UP long   |	DOWN tip  |	DOWN long |
| ---   | -------- | -------- | --------- | --------- |
|0      |0xC4EF317A|0xC4EF317A|	0x364CCA7A|	0x364CCA7A|
|1      |0x33B86A7A|0xC4EF317A|	0x33B86A7A| 0x364CCA7A|
|2      |0x33B86A7A|0xC4EF317A|	0x33B86A7A|	0x364CCA7A|
|3      |0x33B86A7A|          | 0x33B86A7A|	          |
