# Autumn

[![Build Status](https://travis-ci.com/ivanallen/autumn.svg?branch=master)](https://travis-ci.com/ivanallen/autumn)
[![codecov](https://codecov.io/gh/ivanallen/autumn/branch/master/graph/badge.svg)](https://codecov.io/gh/ivanallen/autumn)
![license](https://img.shields.io/badge/license-BSD2-green)
![platform](https://img.shields.io/badge/platform-linux%20%7C%20macos-lightgrey)

An easy interpreter to learn.

![example](https://github.com/ivanallen/autumn/blob/master/docs/images/example.png)

## Features

- Modern C++ style(Based on C++17)
- Support Linux and MacOS
- Full comments
- Support debug in repl

![example2](https://github.com/ivanallen/autumn/blob/master/docs/images/example2.png)

- Higher-order functions supported

![example3](https://github.com/ivanallen/autumn/blob/master/docs/images/example3.png)

- Closure supported

![example4](https://github.com/ivanallen/autumn/blob/master/docs/images/example4.png)

- ...

## Quick Start

### Install GCC8

- Centos

```shell
$ yum install readline-devel.x86_64
$ yum install -y devtoolset-8-toolchain
$ scl enable devtoolset-8 bash
```

- Ubuntu

```
$ sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
$ sudo apt-get update -qq
$ sudo apt-get install -qq g++-8
$ apt-get install lib64readline-dev
$ export CXX="g++-8"
$ export CC="gcc-8"
```

### Build & Test

```
$ make
```

### Repl

- lexer mode

```
$ ./autumn lexer
```

- parser mode

```
$ ./autumn parser
```

- debug in parser mode

```
$ DEBUG_AUTUMN=1 ./autumn parser
```

- eval mode

```
$ ./autumn eval
```

## Demo

This is a quick sort source.

- You need a filter function

```js
let filter = fn(arr, f) {
    let iter = fn(arr, accumulated) {
        if (len(arr) == 0) {
            accumulated
        } else {
            let e = if (f(first(arr))) {
                push(accumulated, first(arr));
            } else {
                accumulated
            }
            iter(rest(arr), e);
        }
    };
    iter(arr, []);
};
```

- QuickSort

```js
let quickSort = fn(arr) {
    if (len(arr) == 0) {
        return arr;
    } else {
        let head = first(arr);
        let smaller = quickSort(filter(rest(arr), fn(x) { x <= head }));
        let bigger = quickSort(filter(rest(arr), fn(x) { x > head }));
        return quickSort(smaller) + [head] + quickSort(bigger);
    }
}
```

- Test

```js
let a = [4,5,3,4,6,6]
quickSort(a)
```

![example5](https://github.com/ivanallen/autumn/blob/master/docs/images/example5.png)


## Contributor

Allen.

## License

This project maintained under BSD 2.

## Contact

QQ Group: 610441700

## References

- [Write An Interpreter In Go](https://interpreterbook.com)
