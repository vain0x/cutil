# cutil

WIP

Opinionated C utility library.

## Concepts

- Public domain ([CC0-1.0](./LICENSE))
- Use [Spans](https://en.cppreference.com/w/cpp/container/span) (also known as [slices](https://doc.rust-lang.org/std/slice/index.html))
- Platform-agnostic: written in standard C11. No conditional compilation.
- Allocator-aware: allocating functions take `Allocator` as parameter

## Features

- [ ] string
- [ ] vector
- [ ] map
- [ ] integer

----
----

## Development

```sh
make
```

## Motivation

I'm writing a runtime for my own language in C.
Because the language uses spans, C standard functions for null-terminated strings are inconvenient.
Also, since runtime code is depended by compiler-generated code, license is problematic; even MIT is too strict for this use case.
