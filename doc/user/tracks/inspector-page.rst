.. SPDX-FileCopyrightText: © 2019-2022 Alexandros Theodotou <alex@zrythm.org>
   SPDX-License-Identifier: GFDL-1.3-invariants-or-later
.. This is part of the Zrythm Manual.
   See the file index.rst for copying conditions.

.. _track-inspector:

Track Inspector
===============

Each track will display its page in the inspector when
selected. Depending on the track, the properties shown
will be different.

.. image:: /_static/img/track-inspector.png
   :align: center

Track Properties
----------------

Track properties are basic properties that the track
has.

.. image:: /_static/img/track-properties.png
   :align: center

Track Name
  Name of the track. Can be changed by double
  clicking.
Direct Out
  The track that this track routes its output to.

  .. figure:: /_static/img/routing-to-groups.png
     :align: center

     Routing an instrument to an audio group

Instrument
  The instrument plugin from this track. This is
  only applicable to
  :ref:`Instrument tracks <tracks/track-types:Instrument Track>`.

.. _track-inputs:

Inputs
------

If the track takes input, there will be an input
selection section. This is mainly used for
:ref:`playback-and-recording/recording:Recording`.

MIDI Inputs
~~~~~~~~~~~

.. image:: /_static/img/track-inputs.png
   :align: center

Input Device
  Device to read MIDI input from
MIDI Channels
  MIDI channels to listen to (other channels will be
  ignored)

Audio Inputs
~~~~~~~~~~~~

.. image:: /_static/img/audio-track-inputs.png
   :align: center

Left Input
  Left audio input port to listen to
Right Input
  Right audio input port to listen to
Mono toggle
  Duplicate the left signal on both channels
Gain knob
  Adjust input gain

MIDI FX/Inserts
---------------

These are slots for dropping audio or MIDI effects that will
be applied to the signal as it passes through the track.

.. image:: /_static/img/midi-fx-inserts.png
   :align: center

MIDI FX are processed after the input and piano roll events
and before the instruments in instrument tracks, or
before the inserts in MIDI tracks.

.. note:: Only MIDI and Instrument tracks support MIDI FX.

Inserts are processed in order. For instrument tracks,
the inserts will be added onto the signal coming from
the instrument, and for other tracks they will be added
on the incoming signal.

The signal flow is summarized below.

``[track input] -> [MIDI FX, if any] -> [instrument, if any] -> [inserts, if any] -> [track output]``

.. _track-sends:

Aux Sends
---------

These are
`aux sends <https://en.wikipedia.org/wiki/Aux-send>`_ to
other tracks or plugin
side-chain inputs. These are generally useful for
side-chaining or applying additional effects to
channels, such as reverb.

.. image:: /_static/img/track-sends.png
   :align: center

The first 6 slots are for pre-fader sends and the
last 3 slots are for post-fader sends.
The pre-fader slots will send the signal before
the fader is processed, and the post-fader slots
will send the signal after the fader is applied.

Fader
-----
Fader section to control the volume and stereo balance.

.. image:: /_static/img/track-fader.png
   :align: center

To change the fader or stereo balance amount, click and
drag their respective widgets. You can reset them to their
default positions with
:menuselection:`Right click --> Reset`.

The meter displays the amplitude of the signal in dBFS as
it is processed live. The following values are displayed
next to the meter for additional reference.

Peak
  Peak signal value
RMS
  Root Mean Square of the signal value

The controls available for controlling the signal
flow are covered in
:ref:`tracks/track-controls:Signal Flow`.

.. tip:: MIDI faders use MIDI volume CC.

.. note:: The MIDI fader is a TODO feature -
   currently, it lets the signal pass through
   unchanged.

Channel Settings
~~~~~~~~~~~~~~~~

.. todo:: Implement.

Comments
--------
User comments. This feature is useful for keeping
notes about tracks.

.. image:: /_static/img/track-comment.png
   :align: center

Clicking the pencil button will bring up a popup to
edit the comment.
