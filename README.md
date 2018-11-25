# Semaphore with IR remote controler

**Motivation**: create toy for 2 years baby for increasing knowlage about behavior in crossroads :)

Only for test, how to use GitHub
[ E-shop toy - **Train with remote IR control**](https://www.lidl-shop.cz/PLAYTIVE-JUNIOR-Vlacek-na-dalkove-ovladani/p100246613)

|  Mode | Lever UP (hold)| Lever UP (Tip.)| Lever DOWN (hold) | Lever UP (tip)|
| :----:  | -------------  |:-------------: | ------:           |  -----        |
| A       | 0x       | 0x       |   $12 |||
| B       | 0x  | 0x       |    $1 |||

## Parts
[Lipol charger](https://www.electroschematics.com/10551/tp4056-lipo-battery-charger-rc-toys/)


Mode B

|Rx data|UP tip	|UP long |	DOWN tip |	DOWN long |
| --- | ------- | -------- | ---------- | ------------ |
|0|0xC4EF317A|0xC4EF317A|	0x364CCA7A|	0x364CCA7A|
|1|0x33B86A7A|0xC4EF317A|	0x33B86A7A| 0x364CCA7A|
|2|0x33B86A7A|0xC4EF317A|	0x33B86A7A|	0x364CCA7A|
|3|0x33B86A7A|          | 0x33B86A7A|	          |
