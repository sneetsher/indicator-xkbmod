## indicator-xkbmod (Unity XKB Modifier Indicator)

Keyboard modifiers state indicator for Ubuntu Unity desktop. As current state, it's just a prototype.

Unity lacks such accessibility tools which most likely needed when sticky keys is activated.<br>
See [How to add a keyboard modifier state applet to Unity panel?](
https://askubuntu.com/questions/402315/how-to-add-a-keyboard-modifier-state-applet-to-unity-panel/).

![Demo: icon mode in Unity](/screenshots/demo_v0.3_icon_unity.png) ![Demo: icon mode under HUD in Unity](/screenshots/demo_v0.3_icon_unity_under-hud.png)

![Demo: icon mode in Unity (Old release)](/screenshots/demo_icon_unity.png)

![Demo: label mode in Unity](/screenshots/demo_label_unity.png)

## Related Bugs Report

- [lp#773078 Should display the StickyKeys status in some way (a11y)](
https://bugs.launchpad.net/unity/+bug/773078)
- [lp#1306584 No keyboard state applet in Unity](
https://bugs.launchpad.net/ubuntu/+bug/1306584)

## Install

- ### From daily build PPA

 https://code.launchpad.net/~sneetsher/+recipe/indicator-xkbmod-daily

- ### From source

        autoreconf -vif
        ./configure
        make
        sudo make install

- ### From locally built Debian package
 (much cleanner then make install)

        debuild -us -uc
        sudo dpkg -i ../indicator-xkbmod*.deb

## Running Options

- Icon mode (Default):

        indicator-xkbmod

- Label mode:

        indicator-xkbmod -l

- Filter needed modifiers (10110011 binary = 205 decimal, Default: 255)

        indicator-xkbmod -f 205

    shows only shift,control,alt,super,altgr

## Gnome-Shell/Gnome3 Support

**Note:** *May be better if you look for native extention, so try first https://github.com/sneetsher/Keyboard-Modifiers-Status*

Currently, it can be run only in label mode (`-l`) using [gnome-shell-extension-appindicator](https://github.com/rgcjonas/gnome-shell-extension-appindicator) which integrates Ubuntu AppIndicators and KStatusNotifierItems into Gnome Shell.

The extension could be installed easily from https://extensions.gnome.org/extension/615/appindicator-support/ .
