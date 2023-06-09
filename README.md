# Home Environment Monitor (HEM)

Monitor Ruuvi sensors and display the average current temperature and humidity for indoor and outdoor sensors.
Additionally, makes weather forecasts based on pressure and the Zambretti forecaster algorithm.

## Primary target and build tools

This project is targeted primarily at Raspberry Pi Pico W and as such has a lot of code regarding WiFi and Bluetooth that are specific to Raspberry Pi Pico W and will assume that the [Raspberry Pi Pico Arduino core][arduino-pico] by [Earle F. Philhower][earlephilhower] is being used to build the source.

### PlatformIO

I'm using the [PlatformIO] tools to maintain and build the source code for this project. Initially, I used the Arduino IDE and/or Arduino CLI but had issues getting Bluetooth working even though the earlephilhower/arduino-pico toolchain has it in the latest version.

Can't say what made it work using PlatformIO instead but I got used to it rather quickly and now feel like I get better control over libraries and build toolchain using it.

## The included 7-segment font

There is a custom 7-segment looking font included in this package, it was made by me in Fony and only contains the necessary symbols for displaying a string such as "20.0°C" or "28.3%" in something looking like a 7 segment display.

The font files are stored in the [fonts/](fonts/) directory and can be edited in a font editor such as [Fony][fony].

The font is converted from BDF to C source using `bdfconv` [included in the U8glib sources][bdfconv] and is not built during the PlatformIO build process.

Here's how I convert the file:

```pwsh
$ ..\u8g2\tools\font\bdfconv\bdfconv.exe -v -b 0 -f 1 -m '32,37,43-46,48-58,67,70,176' .\fonts\Segments_12x17.bdf -o font_segments_12x17.c -n font_segments_12x17
```

This outputs the file `font_segments_12x17.c` from which I copied everything into a file that can be used when building the project: [`include/font_segments_12x17.h`](include/font_segments_12x17.h). I then delete the file generated by `bdfconv.exe`. The reason I have this separate header file is that I wanted to make sure it would build for different target architectures so I have a few sections copied from the u8g2 sources, namely a couple of defines you can find in the [`csrc/u8g2.h`][u8g2.h] and [`csrc/u8x8.h`][u8x8.h] in the [U8glib project][u8glib].

## TODO

- [ ] Log pressure every X minutes to an array such that we always have the last 3 hours in it, to make proper forecast trends.
- [ ] Figure out why the Pico hangs every once in a while, is there a memory issue or something?

## Pictures

Earlier version of the code running on a Raspberry Pi Pico W connected to a SH1107 64x128 OLED display with output rotated 90 degrees:

![HEM presenting on an SH1107 OLED](doc/images/SH1107_OLED_64x128.png)

## License

Copyright © 2023 Jan Lindblom.

Released under the MIT license except for any included external libraries, files or parts of files which retain their respective licenses.

[arduino-pico]: https://github.com/earlephilhower/arduino-pico
[earlephilhower]: https://github.com/earlephilhower
[PlatformIO]: https://platformio.org/
[fony]: http://hukka.ncn.fi/?fony
[u8glib]: https://github.com/olikraus/u8g2
[bdfconv]: https://github.com/olikraus/u8g2/tree/master/tools/font/bdfconv
[u8g2.h]: https://github.com/olikraus/u8g2/blob/master/csrc/u8g2.h#L188
[u8x8.h]: https://github.com/olikraus/u8g2/blob/master/csrc/u8x8.h#L143
