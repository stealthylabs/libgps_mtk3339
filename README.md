# libgps\_mtk3339

## INTRODUCTION

This is a C library to perform GPS data parsing for the [Adafruit Ultimate GPS
Board](https://www.adafruit.com/product/746) which uses the GlobalTop MTK3339
chip whose documents and datasheets can be found [here](https://learn.adafruit.com/adafruit-ultimate-gps/downloads).

If your goal is to use Arduino or CircuitPython, this library is **not** for
you.

This library is for the special use case where you have an application running
on the Raspberry Pi, Beaglebone Black or any other Linux single board computer
(SBC) where the GPS breakout board or the chip are directly connected to the
UART of that board. You may also use this with a standard Linux computer using
the USB-TTL cable made by FTDI.

Sometimes you want to integrate reading from the GPS into a larger code base which is
**not** written in Python but needs to run on smaller devices and use low
battery power.

This is why this library is in C, so it can directly execute on the Raspberry Pi
or similar SBCs where it can be integrated into other applications easily.

Another reason for using this C library is to be able to integrate this library
with applications using libraries like `libuv` or `libev` or similar event-based libraries,  and control
the GPS reading or adjustments using events and keeping the power consumption low on such devices.


## BUILD and TEST

```bash
$ git submodule init
$ git submodule update
$ sudo apt-get -y install ragel libuv1-dev libev-dev
$ ./autogen.sh
$ ./configure
$ make
$ make check
```

If you want to install this library in a location other than `/usr/local` you
want to run `configure` with the `--prefix` option.

If you want to build in debug mode, you want to run `configure` with the
`--enable-debug` option.

If you are a developer and want to check if the code compiles, links, installs and runs
after you run `make install` you can run `./checkinstaller.sh` to compile,
install and test.


## COPYRIGHT

&copy; 2015-2020. Stealthy Labs LLC. All Rights Reserved.
