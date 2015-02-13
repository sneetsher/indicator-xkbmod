#ref: https://developer.gnome.org/gnome-devel-demos/3.7/weatherAutotools.js.html.en

EXTRA_DIST=  \
    $(private_icons)	\
	  $(NULL)

CLEANFILES =

NULL =

private_icons = \
	icons/icon.png \
	$(NULL)

install-icons:
	for icon in $(private_icons); do \
		mkdir -p $(DESTDIR)$(pkgdatadir)/icons/; \
		$(INSTALL_DATA) $(srcdir)/$$icon $(DESTDIR)$(pkgdatadir)/icons/; \
	done

install-desktop:
	mkdir -p $(DESTDIR)$(datarootdir)/icons/;
	mkdir -p $(DESTDIR)$(datarootdir)/applications/;
	$(INSTALL_DATA) $(srcdir)/indicator-xkbmod.png $(DESTDIR)$(datarootdir)/icons/; \
	$(INSTALL_DATA) $(srcdir)/indicator-xkbmod.desktop $(DESTDIR)$(datarootdir)/applications/;

install-data-local: install-icons install-desktop
