# Dailyfile

## Description

This small tool copies the content of its standard input into daily switching files.

This allows the user to split the output of a long-time running command into daily (or other duration) files.

It can be easily used on command line, in a much simpler way than `logrotate`.

## License

License GPLv3+: [GNU GPL version 3 or later](http://gnu.org/licenses/gpl.html).
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

## Author

Copyright 2017 [Christophe Blaess](https://www.blaess.fr/christophe).

## Installation

You can install the project with CMake:

```
$ git clone https://github.com/cpb-/dailyfile
$ mkdir build
$ cd build
$ cmake ../dailyfile
$ make
$ sudo make install

```

or with the provided Makefile:
```
$ git clone https://github.com/cpb-/dailyfile
$ cd dailyfile
$ make clean
$ make
$ sudo make install
```


## Usage

```
$ dailyfile [options]
```

### Options

* `-b, --buffer-size=SIZE`   Use a specific buffer size for the copy.
* `-c, --cycle=SECONDS`      Cycle duration for switching files.
* `-d, --directory=DIR`      Save the files into the given directory.
* `-h, --help`               Display this help screen and quit.
* `-l, --localtime`          Use localtime for filenames instead of G.M.Time.
* `-p, --prefix=STRING`      Use the given prefix for filenames instead of "day-".
* `-s, --suffix=STRING`      Use the given suffix for filenames instead of ".log".
* `-v, --version`            Display version informations.

Default filenames are `day-YYYY-MM-DD.log` if cycle duration is longer than a day
and `day-YYYY-MM-DD-hh-mm-ss.log` if cycle is shorter than a day.

### Typical usage

```
$ some-long-time-running-command | dayfile
```




