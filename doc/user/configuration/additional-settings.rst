.. This is part of the Zrythm Manual.
   Copyright (C) 2019 Alexandros Theodotou <alex at zrythm dot org>
   See the file index.rst for copying conditions.

Additional Settings
===================

GSettings
---------
Zrythm stores most of its configuration using
the GSettings mechanism, which comes with the
`gsettings <https://developer.gnome.org/gio/stable/gsettings-tool.html>`_ command for changing settings
from the command line, or the optional GUI tool
`dconf-editor <https://wiki.gnome.org/Apps/DconfEditor>`_.

Normally, you shouldn't need to access any of
these settings as most of them are found inside
Zrythm's UI, and it is not recommended to
edit them as Zrythm validates some settings
before it saves them or uses some settings
internally, but in some cases you
may want to change them manually for some
reason.

Viewing the Current Settings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

See :option:`zrythm --print-settings`.

Reset to Factory Settings
~~~~~~~~~~~~~~~~~~~~~~~~~

See :option:`zrythm --reset-to-factory`.

.. todo:: This will be added to the UI soon.

Plugin Settings
---------------
Located at :file:`plugin-settings.yaml` under the
:term:`Zrythm user path`, this is a collection of
per-plugin settings, such as whether to open the
plugin with :term:`Carla`, the
:ref:`bridge mode <plugins-files/plugins/plugin-window:Opening Plugins in Bridged Mode>`
to use and whether to use a
:ref:`generic UI <plugins-files/plugins/plugin-window:Generic UIs>`.

Zrythm will remember the last setting used for each
plugin and automatically apply it when you choose to
instantiate that plugin from the
:doc:`plugin browser <../plugins-files/plugins/plugin-browser>`.

.. note:: This file is generated and maintained
   automatically by Zrythm and users are not
   expected to edit it.
