dreamled
========

Control the 3 panel LED's on the GlobalScale DreamPlug (plug computer) development platform.


The led's on the front panel of a dreamplug are controlled by a few bytes of memory mapped IO.

This little program actives them and then allows the user read their current states
and turn them on, off, and blinking from the command line.

That blinking blue BT led that blinks by default was super annoying.


Use it like : $# dreamled ( ap | wlan | bt | all ) [ on | off | blink ]

I included and "all" switch mostly so I could do all "all off" by single command.

I put the binary, compiled on a dreamplug, in this repo because a great many DreamPlug users
don't have dev enviroments set up to compile an arm binary from source.
