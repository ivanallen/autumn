# Autumn

[![Build Status](https://travis-ci.com/ivanallen/autumn.svg?branch=master)](https://travis-ci.com/ivanallen/autumn)

An easy interpreter to learn.

![example](https://github.com/ivanallen/autumn/blob/master/docs/images/example.png)

## Features

- Modern C++ style(Based on C++17)
- Full comments

## Quick Start

### Install GCC8

- Centos

```shell
$ yum install -y devtoolset-8-toolchain
$ scl enable devtoolset-8 bash
```

- Ubuntu

```
$ sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
$ sudo apt-get update -qq
$ sudo apt-get install -qq g++-8
$ export CXX="g++-8"
$ export CC="gcc-8"
```

### Build & Test

```
$ make
```

## Contributor

Allen.

## License

This project maintained under BSD 2.

## Contact

QQ Group: 610441700

## References

- [Write An Interpreter In Go](https://interpreterbook.com)
