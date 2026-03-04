# SoundX

![GitHub release (latest by date)](https://img.shields.io/github/v/release/SukkoPera/SoundX)
![GitHub Release Date](https://img.shields.io/github/release-date/SukkoPera/SoundX?color=blue&label=last%20release)
![GitHub commits since latest release (by date)](https://img.shields.io/github/commits-since/SukkoPera/SoundX/latest?color=orange)

SoundX is an Open Hardware sound card for the Commodore 16, 116 and Plus/4 home computers based on the Yamaha OPL/OPL-II chip.

![Board](https://raw.githubusercontent.com/SukkoPera/SoundX/master/img/render-top.png)

## Summary
SoundX is basically an adaptation of the [C64 SFX Sound Expander](https://www.c64-wiki.com/wiki/Commodore_Sound_Expander) for the x264 series of Commodore computers.

While it didn't enjoy much success when it was released on the C64, since it was not that much superior to the built-in SID (if superior at all...), it would have certainly been much more successful if it had been targeted at the Commodore 16, 116 and Plus/4. In fact, it is basically the equivalent of an [AdLib](https://en.wikipedia.org/wiki/Ad_Lib,_Inc.#AdLib_Music_Synthesizer_Card_(1987)), an audio card for the PCs of the time, based on the YM3526 chip from Yamaha (also known as OPL), which is *exponentially* better than the TED as a sound chip, as it sports the following features (thanks [Wikipedia](https://en.wikipedia.org/wiki/Yamaha_OPL)):

- 9 channels of sound, each made of two oscillators or 6 channels with 5 percussion instruments

For every channel:
- Main frequency (10 bits)
- Octave (3 bits)
- Note on/off
- Synthesis mode (FM or just additive)
- Feedback (0–7, the modulator modulating itself)

For each one of the two oscillators:
- Frequency multiply (can be set to 1⁄2, 1 to 10, 12 or 15)
- Waveform (Sine)
- Volume (0–63, logarithmic)
- Attack, decay, sustain, release (4 bits each, logarithmic)
- Tremolo (on or off)
- Vibrato (on or off)
- Sustain (on or off)
- Envelope scaling per key (on or off)
- Volume scaling per key (0–3)

There are also a few parameters that can be set for the whole chip:
- Vibrato depth
- Tremolo depth
- Percussion mode (uses 3 channels to provide 5 percussion sounds)
- Composite sine mode (see Sinewave synthesis)

The board can even use the slightly more modern YM3812 (OPL-II): *among its newly-added features is the ability to pick between four waveforms for each individual oscillator. In addition to the original sine wave, three modified waveforms can be produced: half-sine waves (where the negative part of the sine is muted), absolute-sine waves (where the negative part is inverted), and pseudo-sawtooth waves (quarter sine waves upward only with silent sections in between)*.

### MIDI I/O
The original SFX Sound Expander could be bought [together with a piano-like keyboard](http://www.mssiah-forum.com/viewtopic.php?pid=4598#p4598) and some demo software. Such keyboard is not easy to find these days, so it was pointless to replicate the interface for it and I rather decided to build upon my experience with another project and add a real MIDI I/O interface. This means the C16/116/+4 will be able to receive data from a MIDI keyboard or to play it, through a **standard** DIN-5 MIDI interface.

## Versions
There are currently two versions of SoundX. Both have the same features and differ only in a few aspects, see below for more details.


## Design and Assembly Notes
This project makes sense because the YM3812 chip, its companion DAC (YM3014B) and an ACIA chip (for the MIDI part) can all be bought supercheap on AliExpress & similar sites, making this board very affordable to build for everyone. Let's say 15-20€? So get all this stuff second-hand and be happy :).

Remember to solder the JP5 jumper on the back of the board: I suggest the ON position unless you know what you are doing (check the schematics).

The audio output is automatically fed back into the computer through the EXT_AUDIO pin, so that you will hear it mixed with the sounds produced by the TED. There is also a 3.5" jack connector on the board, which allows bringing the sound output to external equipment. Note that when a jack is plugged in, the sound will no longer be redirected to the EXT_AUDIO pin.

Note that the OPL and MIDI circuits of the board are completely independent from each other, so the board can also be assembled partially if only one of the features is desired. The IBOM points out which feature every components belongs to.

### Version 1
Version 1 uses a standard ACIA 6551 chip for the MIDI interface. This is available from different manufacturers (MOS, Rockwell, and even [new from the Western Design Center](https://www.westerndesigncenter.com/wdc/w65c51s-chip.php)!) and requires a 3 MHz crystal to reach the MIDI baudrate.

The YM3812 requires an active oscillator either in DIP-8 or DIP-14 size.

There are some solder jumpers on the board:
- JP1: Close if your oscillator requires pin 1 to be grounded (uncommon).
- JP2: Close if you want the same signal sent to both channels of the jack connector.
- JP3/JP4: Close these to provide +5V on the normally-unused pins of the MIDI output connector: **this is non-standard and might actually damage the board or the connected device**, so keep these open unless you REALLY know what you are doing.
- JP5: The YM3812 can either go through a 15 kHz low-pass filter (ON) or not (BYPass): normally set to ON.

### Version 2
Version 2 switched the ACIA to a Motorola MC6850 chip that requires an active oscillator in DIP-8 size, running at either 500 kHz or 2 MHz. When using the 500 kHz oscillator, U5 and C12 are not required and can be skipped altogether.

In order to make space for the MC6850 oscillator, the YM3812 oscillator was also restricted to DIP-8 size.

Version 2 features additional jumpers:
- JP6: Same as JP1 for the MC6850 oscillator.
- JP7: Set in accordance with the oscillator frequency.
- JP8: Set to R/W and don't ask questions! If you want to know more, see [this](https://github.com/mist-devel/c64/issues/1).

## Programming
### OPL/OPL-II
Due to the impressive array of features, the OPL/OPL-II is not easy to program: the chip has 244 registers, so it would take a while to get acquainted with it and there is really not much documentation about how to program the SFX Sound Expander. This is one of the reasons why I decided to diverge a bit from the SFX Sound Expander on the programming side and rather followed the AdLib style: the board only uses two addresses for the audio part: $FDE4 and $FDE5. The former is for writing the number of the YM register to be modified while the latter is for the value. The former address can also be read and it will return the OPL status register, which is only useful for the detection of the board or if you want to make use of the OPL internal timers.

This means that you should be able to follow any AdLib programming tutorials around (like [this one](https://bochs.sourceforge.io/techspec/adlib_sb.txt)), as they should be 100% applicable to SoundX just as well (except for the different addresses, of course). The chip detection from that document also works!

> [!IMPORTANT]
> When programming the OPL chip, always keep in mind is that **you must wait at least 3.3 microseconds after you wrote the address, before you write the data, and then at least 23 microseconds before the next write**. This stands for AdLib sound cards as well, since it is an inherent limitation of the OPL chip. Nevertheless, [some clever programming](https://c64.xentax.com/index.php/15-testing-ym3812-register-write-timing) appears to be able to mitigate the issue.

### MIDI I/O - Version 1
The classic ACIA (6551) chip running the MIDI section uses addresses $FDE0/1/2/3 (I chose these addresses because they were already partly used by Solder's MIDI interface).

Configure the chip for:
- No Parity, No Echo, No TX Interrupt, /RTS Low (Unneeded), No RX Interrupt, /DTR Low (Unneeded) &rarr; $0b to Command Register
- 0 Stop Bits, 8 Data Bits, Internal Baud Rate Generator, 19200 bps (which will actually result in 31250 due to the use of the non-standard crystal) &rarr; $1f to Control Register

### MIDI I/O - Version 2
The MC6850 only has two register, which get mapped at $FDE0/1, which makes me think that this is exactly the same design used by Solder's interface. The software configuration is the same, regardless of what oscillator is installed: configure the chip for No interrupt, /RTS Low (Unneeded), 8 Data bits + 1 Stop bit, No Parity, Clock Divider = 16 (i.e.: $15)

> [!NOTE]
> Please always support both board versions in your projects. It is easy and you can find a driver that does so on the [Wiki](https://github.com/SukkoPera/SoundX/wiki).

### Examples
If you want some code to start from, you can have a look at the [Tech Demo that Master Csabo from the Plus/4 World Forum quickly hacked together](https://plus4world.powweb.com/software/YM3812_Tech_Demo): it does OPL detection and then plays a few songs from the original [AdLib Card Demo](https://vgmrips.net/packs/pack/adlib-music-synthesizer-card-demo-songs-ibm-pc-xt-at). It comes with full source code, so it will definitely be helpful!

Csabo has also made a [Simple MIDI Decoder](https://plus4world.powweb.com/software/Simple_MIDI_Decoder) utility that listens for NoteOn and NoteOff messages coming in through the MIDI port and plays the corresponding notes through the TED. Again, it comes with full source code and thus it should help you have a jump start.

## Next Steps
Of course, to make complete sense, this project needs support from the actual programmers! So people, please make games for the board! Or programs, demos, whatever! I think a nice first step would be to port [the original SFX Sounds Expander software](https://csdb.dk/release/?id=155181): I can't say for sure, but I'm guessing this shouldn't be too much of an effort, plus I can help with the tech details and can explain how to make the MIDI decoder, just ask! :)

## Releases
If you want to get this board produced, you are recommended to get [the latest release](https://github.com/SukkoPera/SoundX/releases) rather than the current git version, as the latter might be under development and is not guaranteed to be working.

Every release is accompanied by its Bill Of Materials (BOM) file and any relevant notes about it, which you are recommended to read carefully.

## License
The SoundX documentation, including the design itself, is copyright &copy; SukkoPera 2023-2025 and is licensed under the [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License](https://creativecommons.org/licenses/by-nc-sa/4.0/).

This documentation is distributed *as is* and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES whatsoever with respect to its functionality, operability or use, including, without limitation, any implied warranties OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A PARTICULAR PURPOSE or infringement. We expressly disclaim any liability whatsoever for any direct, indirect, consequential, incidental or special damages, including, without limitation, lost revenues, lost profits, losses resulting from business interruption or loss of data, regardless of the form of action or legal theory under which the liability may be asserted, even if advised of the possibility or likelihood of such damages.

## Support the Project
If you want to get some boards manufactured, you can get them from PCBWay through this link:

[![PCB from PCBWay](https://www.pcbway.com/project/img/images/frompcbway.png)](https://www.pcbway.com/project/shareproject/SoundX_V2_An_AdLib_card_for_your_Commodore_16_116_4_7110494a.html)

You get my gratitude and cheap, professionally-made and good quality PCBs, I get some credit that will help with this and [other projects](https://www.pcbway.com/project/member/?bmbno=72D33927-5EF6-42). You won't even have to worry about the various PCB options, it's all pre-configured for you!

Also, if you still have to register, [you can use this link](https://www.pcbway.com/setinvite.aspx?inviteid=41100) to get some bonus initial credit (and yield me some more).

You can also buy me a coffee if you want, all the money collected this way will actually go to charity:

<a href='https://ko-fi.com/L3L0U18L' target='_blank'><img height='36' style='border:0px;height:36px;' src='https://storage.ko-fi.com/cdn/kofi6.png?v=6' border='0' alt='Buy Me a Coffee at ko-fi.com' /></a>

## Thanks
* Master Csabo and Chizman (unbeknownst) for their help with the testing software.
* kinmami for his usual precious hints about the hardware design.
* Iconian Fonts (Daniel Zadorozny) for the [Universal Jack Font](https://www.fontspace.com/universal-jack-font-f101650) used for the logo.
