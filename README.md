libkern library
===============

liblern is a simple generic collection library for C99 largely based on
Linux kernel sources, algoritms and data structures.

For more information, please consider visiting [project
website](http://github.com/petrh/libkern).

Features
--------

libibase provides the following data structures and algorithms:

* single- and double-linked lists
* red-black trees
* leftist heaps
* bitmaps

Building libibase
-----------------
		   
It is now possible to automatically configure and build a variety of
tools with one command. To build all of the tools contained herein, run
the `configure` script here, e.g.:

```shell
./configure 
make
```

To install them (by default in /usr/local/bin, /usr/local/lib, etc),
then do:

```shell
make install
```

(If the configure script can't determine your type of computer, give it
the name as an argument, for instance `./configure sun4`.  You can use
the script `config.sub` to test whether a name is recognized; if it is,
config.sub translates it to a triplet specifying CPU, vendor, and OS.)

If you have more than one compiler on your system, it is often best to
explicitly set CC in the environment before running configure, and to
also set CC when running make.  For example (assuming sh/bash/ksh):

```shell
CC=gcc ./configure
make
```

A similar example using csh:

```shell
setenv CC gcc
./configure
make
```

Much of the code and documentation enclosed is copyright by the Free
Software Foundation, Inc.  See the file COPYING or COPYING.LIB in the
various directories, for a description of the GNU General Public License
terms under which you can copy the files.

Authors
-------

* [Petr Hosek](mailto:p.hosek@imperial.ac.uk)

Credits
-------

This project implementation either uses some parts or is partially
inspired by the following projects.

The Linux kernel released under the GNU General Public License (GPL)
version 2.0.

The glibc library released under the GNU Lesser General Public License
(LGPL) version 2.0.

License
-------

libibase is licensed under the GNU General Public License (GPL) version
2.0 with linking exemption. This means you can link to the library with
any program, commercial, open source or other. However, you cannot
modify libgit2 and distribute it without supplying the source.
