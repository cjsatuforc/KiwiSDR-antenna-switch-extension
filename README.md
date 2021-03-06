# KiwiSDR-antenna-switch-extension

This is antenna switch extension to KiwiSDR software defined radio.

Antenna switch can control out-of-stock antenna switches and utilize Beaglebone GPIO-pins. You can also write your own backend for third party antenna switch.

![MS-S7-WEB kit](http://oh1kk.toimii.fi/ant_switch_extension/MS-S7-WEB.jpg)

## Features

* Basic antenna switching
* Antenna mixing. In antenna mix mode multiple antennas can be selected simultaniously.
* Admin can lock/unlock antenna switching from admin panel
* Admin can enable/disable antenna mixing from admin panel
* Admin can deny antenna switching if more than 1 user is online
* Antenna switching can be time scheluded using Beaglebone's crontab
  * look at docs/antenna-schedules-using-crontab.txt
* Switcing back to default antennas when no users are online
  * look at docs/reset-to-default-antennas-when-no-users-online.txt
* <s>Thunderstorm mode. In thunderstorm mode all antennas are grounded.</s> not working right now
  
## Required hardware

You need KiwiSDR is a software-defined radio (SDR) kit

You need antenna switch hardware.

## Available backends for hardware

* ms-s7-web for controlling LZ2RR's MS-S7-WEB antenna switch
* beagle-gpio for controlling Beaglebone green GPIO pins
* snaptekk for controlling Snaptekk Wifi ham radio 8 antenna switch
* arduino-netshield for Arduino Nano V3.0 GPIO pins. ENC28J60 Ethernet Shield needed.
* example-backend is example script for your own backend development

## Version compability

* Tested to work with KiwiSDR v1.194. Does not work on older versions than v1.95 due KiwiSDR API changes.
* Tested to work with MS-S7-WEB firmware v1.01
* Tested to work with Snaptekk WiFi wireless 8 antenna switch

## Installation

open ssh connection to your KiwiSDR as root user

    cd /root
    git clone https://github.com/OH1KK/KiwiSDR-antenna-switch-extension.git
    cd KiwiSDR-antenna-switch-extension
    bash ./ant-switch-extension-installer

Installer copies ant_switch files on place, creates configuration file and recompiles KiwiSDR. This will take several minutes. After compile is finished, KiwiSDR will be restarted. After restart ant_switch extension is installed to KiwiSDR.

## Configuration

Open your KiwiSDR admin panel. Then Extensions -> Antenna Switch.

![ant switch extenstion admin interface](http://oh1kk.toimii.fi/ant_switch_extension/admin_interface_20180123.png)

By default users can switch antennas and select multiple simultanoius antennas.

Describe your antennas 1-8. If you leave antenna description empty, antenna button won't be visible to users.

Antenna switch failure or unknown status decription will be show to users if antenna switch is unreachable or malfunctioning. 

## Usage

Open your KiwiSDR as user. Enable ant_switch extension from extension drop down menu. Antenna switch will show.

![ant switch extension user interface launch](http://oh1kk.toimii.fi/ant_switch_extension/user_interface_launch_20180123.png)
![ant_switch_extension_user_interface](http://oh1kk.toimii.fi/ant_switch_extension/user_interface_20180123.png)

Single antenna mode: Click to select antenna. 

Antenna mixing mode: you can select multiple antennas simultaniously. Click antennas on/off. 

If admin has disable antenna switching, buttons are grey and you cannot click them.

## Uninstalling extension

open ssh connection to your KiwiSDR as root user

    cd /root/KiwiSDR-antenna-switch-extension
    bash ./ant-switch-extension-uninstaller
    cd /root
    rm -rf KiwiSDR-antenna-switch-extension

## Demo site

KiwiSDR Kaustinen http://sdr.vy.fi

## Donate
If you want to support this project, you can [send a donation via PayPal](https://www.paypal.me/oh1kk).

## License

[The MIT License (MIT)](LICENSE)

Copyright (c) 2018 Kari Karvonen
