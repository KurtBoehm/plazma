# Plazma 🌋: Multithreading-friendly XZ Decoding and Encoding

Plazma is a header-only C++20 library based on `liblzma` from [XZ Utils](https://github.com/tukaani-project/xz) which supports loading a segment of a (blocked) XZ file efficiently, which enables multithreaded decoding, as well as multithreaded encoding of a (blocked) XZ files.
While any XZ file can be decoded, segmented loading is only efficient if the file is split into a sufficient number of blocks internally, which the `xz` command-line utility does in its multithreaded mode, i.e. with `-T` set to something other than 1, and Plazma’s writer does in any case.
The source files in the `test` subdirectory give a reasonably good overview of how Plazma can be used in conjunction with [Thesauros](https://github.com/KurtBoehm/thesauros).

## Building

Plazma uses the Meson build system and includes two tests which decode/encode Alice in Wonderland.
These can be run by executing `meson setup -C <build directory>` followed by `meson test -C <build directory>`.

## Dependencies

Dependencies are handled using Meson subprojects, which make it possible to use Plazma without installing any packages.
The Meson subprojects are managed by [Tlaxcaltin](https://github.com/KurtBoehm/tlaxcaltin) and are only [XZ Utils](https://github.com/tukaani-project/xz) and [Thesauros](https://github.com/KurtBoehm/thesauros), in addition to the compiler options defined in Tlaxcaltin.

## Licences

Plazma is licenced under the terms of the Mozilla Public Licence 2.0, which is provided in [`License`](License).
