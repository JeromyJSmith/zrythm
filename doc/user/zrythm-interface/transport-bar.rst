.. This is part of the Zrythm Manual.
   Copyright (C) 2019 Alexandros Theodotou <alex at zrythm dot org>
   See the file index.rst for copying conditions.

.. _transport-bar:

Transport Bar
=============

The transport bar contains information about the audio engine
and transport controls.

.. image:: /_static/img/transport-bar.png
   :align: center

Transport Controls
------------------
Transport controls change the behavior of playback.
They include the following items.

.. image:: /_static/img/transport-controls.png
   :align: center

Metronome toggle
++++++++++++++++
Toggles the metronome on/off.

Metronome options
+++++++++++++++++
Sets the volume of the metronome.

Return to cue point on stop
+++++++++++++++++++++++++++
Toggles whether to return to the cue point when
playback stops.

BPM
+++
Song tempo (beats per minute).

Playhead
++++++++
Current playhead position, in bars.beats.sixteenths.ticks.

Transport buttons
+++++++++++++++++
Controls for stopping, playback, recording, etc.

.. tip:: Widgets like the BPM meter can be changed
  by clicking and dragging, or by hovering over them with the cursor
  and scrolling

More information about working with the transport
bar is covered in :ref:`transport-controls`.

Backend Information
-------------------
Information about the currently selected backend and options
is visible in the bottom left corner.

.. image:: /_static/img/backend-info.png
   :align: center
