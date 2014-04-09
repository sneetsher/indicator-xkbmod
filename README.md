indicator-xkbmod
================

Keyboard modifiers state indicator for Ubuntu Unity desktop. As courent state is just a prototype.

Unity lacks such accessibility tools whick most likely needed when sticky is activated. See `How to add a keyboard modifier state applet to Unity panel? <https://askubuntu.com/questions/402315/how-to-add-a-keyboard-modifier-state-applet-to-unity-panel/>`_


Building
--------

```sh
cd ./indicator-xkbmod/
gcc -Wall indicator-xkbmod.c -o indicator-xkbmod `pkg-config --cflags --libs appindicator-0.1` -lX11
```
