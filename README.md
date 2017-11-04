# M5Stack
Project repo related to the M5Stack dev platform.

----------

**M5Stack_Jeedom_Infos_Screen**
------

Infos screen that query a Jeedom server (used for home automation) through wifi and retrieve the temperature, humidity and lamp status then display it. The LCD is set to shut down after a specific time. It will turn on by pushing the middle button or if a value have changed. Then shut off to prevent LCD burn-in.
In case of one value have changed and waked up the LCD, the difference is displayed. (e.g: the temperature changed from 24.00 to 24.42, it will display the + symbol and 0.42 value).
