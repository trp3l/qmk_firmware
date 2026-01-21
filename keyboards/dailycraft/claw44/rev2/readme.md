# ÆLITH

![ÆLITH](https://i.imgur.com/wG1OWLol.png)

A through-hole Alice-layout keyboard.

<<<<<<<< HEAD:keyboards/rmi_kb/aelith/readme.md
* Keyboard Maintainer: [Ramon Imbao](https://github.com/ramonimbao)
* Hardware Supported: ATmega32A

Make example for this keyboard (after setting up your build environment):

    make rmi_kb/aelith:default
========
* Keyboard Maintainer: [yfuku](https://github.com/yfuku)
* Hardware Supported: Claw44 PCB, ProMicro
* Hardware Availability: https://shop.dailycraft.jp/

Make example for this keyboard (after setting up your build environment):

    make dailycraft/claw44:default

Flashing example for this keyboard:

    make dailycraft/claw44:default:flash
>>>>>>>> topic-claw44-rev2:keyboards/dailycraft/claw44/rev2/readme.md

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 2 ways:

* **Physical reset button**: Briefly press the button on the PCB
* **Keycode in layout**: Press the key mapped to `RESET` if it is available
