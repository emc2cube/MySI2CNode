MySI2CNode
==========

![Node with battery and Si7021](https://raw.githubusercontent.com/emc2cube/MySI2CNode/master/img/MySI2CNode_temphumbatt.jpg)
![Node with micro USB](https://raw.githubusercontent.com/emc2cube/MySI2CNode/master/img/MySI2CNode_usb.jpg)
![KiCad view](https://raw.githubusercontent.com/emc2cube/MySI2CNode/master/img/kicad-pcb.png)


Description
-----------

Simple [MySensors](http://www.mysensors.org) sensor node with support for up to 3 I2C modules (nRF24L01, 2.4GHz).


Ordering
--------

Gerber files are included so you can order these PCB at your favorite PCB fab house and solder the few components yourself.
For an easy ordering process you can directly order these PCB without having to do anything else:
- [PCBs.io](https://PCBs.io/share/8VvDe) 4 for $6.78, black 1.2mm PCB, ENIG finish.
- [DirtyPCBs](http://dirtypcbs.com/store/designer/details/10880/1157/mysi2cnode-zip) ~10 for $12.95, multiple colors and PCB thickness, HASL finish.


Assembly
--------

If you plan to power this node by a battery it is advised to first prepare your arduino pro mini according to [MySensors battery powered sensors specifications](https://www.mysensors.org/build/battery) (removing Vreg and power indicator LED). You may also want to change your BOD fuse settings.
Do not remove the voltage regulator if you are powering the sensors using a USB wall charger, it will be used to power both arduino and radio module.

If you are using a battery holder start by soldering all SMD components except the radio module. Continue with arduino pin headers, trim them as close as you can of the PCB.
Then solder the battery holder and if you need trim the negative pin to be flush with the PCB. Finally you can carefully add the radio module.


Optional components
-------------------

### Dual power mode
You can run this node using batteries, a CR123 battery holder can be mounted on the PCB, using + and - pins of the PCB.
It is also designed to use a micro USB connector as power input, any old USB phone charger will do.

### Battery monitoring
If using a battery to power the sensor current battery level can be sent to the controller if R3, R4 and C5 are installed.

### ATSHA204 module
For security reason you can add a CryptAuthEE SHA256 chip. This will allow you to sign messages and will secure communications between the node and your gateway (this is not encryption, just signing).
If you only control lights, fan, or other non-essential hardware you probably don't need to bother with this chip.
Signing can also be done at the software level, without the chip if you decide to add this function later.

### Eeprom module
This module is only used to perform OTA updates on the node. If you don't plan to use this feature you can also skip this chip.
You will also need to burn a compatible bootloader to your arduino (DualOptiBoot)


Arduino
-------

Included program(s)

### MySI2CNode-SI7021_TempHum
Use "i2c_SI7021.h" library from [I2C-Sensor-Lib (iLib)](https://github.com/orgua/iLib).
Simple program to retrieve temperature and humidity. Will also check battery status and report to controller.


Revision history
----------------

Version 1.0: Initial release.