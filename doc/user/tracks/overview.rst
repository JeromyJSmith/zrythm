.. This is part of the Zrythm Manual.
   Copyright (C) 2019-2020 Alexandros Theodotou <alex at zrythm dot org>
   See the file index.rst for copying conditions.

Overview
========

Tracks are the main building blocks of projects.
Tracks appear in the tracklist and contain various
events such as regions (see :term:`Region`).

.. image:: /_static/img/track.png
   :align: center

Most types of tracks have a corresponding channel
that appears in the :ref:`mixer`. See
:ref:`channels` for more info.

.. image:: /_static/img/channel.png
   :align: center

There are various kinds of Tracks suited for
different purposes, explained in the following
sections. Some tracks are special, like the
:ref:`chord-track` and the :ref:`marker-track`,
which contain chords and markers respectively.

.. important:: In Zrythm, group tracks are used for
  grouping signals (direct routing), FX/bus tracks
  are used for effects/sends, and folder tracks
  (coming soon) are  used for
  putting tracks under the same folder and
  performing common operations.

  Moreover, Zrythm uses Instrument tracks for
  instrument plugins, such as synthesizers, and
  MIDI tracks for MIDI plugins, such as a
  MIDI arpegiator.

  This is different from what many
  other DAWs (see :term:`DAW`) do, so please keep
  this in mind.

Track Interface
---------------

.. image:: /_static/img/track-interface.png
   :align: center

Each track has a color, an icon (corresponding to its type)
a name and various buttons. Tracks that can have
lanes, like instrument tracks, will also have an option to
display each lane as above. Tracks that can have automation
will have an option to display automation tracks as above.

If the track produces output, it will have a meter on its
right-hand side showing the current level.

Track Inspector
----------------

Each track will display its page in the inspector when
selected. See :ref:`track-inspector` for more details.

Context Menu
------------

Each tracks has a context menu with additional options
depending on its type.

.. image:: /_static/img/track-context-menu-duplicate-track.png
   :align: center

The section :ref:`track-operations` explains the
various track operations available.
