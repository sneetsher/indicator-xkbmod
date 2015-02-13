# indicator-xkbmod

Keyboard modifiers state indicator for Ubuntu Unity desktop. As current state, it's just a prototype.

Unity lacks such accessibility tools which most likely needed when sticky keys is activated.<br>
See [How to add a keyboard modifier state applet to Unity panel?](
https://askubuntu.com/questions/402315/how-to-add-a-keyboard-modifier-state-applet-to-unity-panel/).

![Demo: icon mode in Unity](/screenshots/demo_icon_unity.png)

![Demo: label mode in Unity](/screenshots/demo_label_unity.png)

### Related Bugs Report

- [lp#773078 Should display the StickyKeys status in some way (a11y)](
https://bugs.launchpad.net/unity/+bug/773078)
- [lp#1306584 No keyboard state applet in Unity](
https://bugs.launchpad.net/ubuntu/+bug/1306584)

### Daily build PPA

https://code.launchpad.net/~sneetsher/+recipe/indicator-xkbmod-daily

### Build from source

    autoreconf -vif
    ./configure
    make
    sudo make install

### Build Debian package

    debuild -us -uc
    sudo dpkg -i indicator-xkbmod*.deb
