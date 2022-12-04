# Valgrind &ndash; RISCV64/Linux

## Overview

This repository contains [Valgrind][Valgrind] with support for the
[RISCV64][RISC-V]/[Linux][Linux] platform.

The aim of the project is to enable Valgrind for the RV64GC instruction set on
the Linux operating system. Once this support is implemented and has sufficient
quality, it is intended for this port to become a part of the upstream Valgrind
project and to continue further development there.

For installation, please follow the generic steps how to build Valgrind in [the
main README file](README).

In case you are interested in helping with the port then the best option is to
analyze remaining failing tests in the Valgrind test suite, or just try the port
with your application and report any discovered bugs.

The project was presented at [FOSDEM 2022][FOSDEM talk].

## Current state

Current focus is on functionality and correctness.

The following tables summarize the basic state as of 2022-12-04.

Enabled RV64GC instructions:

| Name         | Description                       | #Enabled/#Instrs |
| ------------ | --------------------------------- | ---------------: |
| RV64I        | Base instruction set              |            52/52 |
| RV64M        | Integer multiplication & division |            12/13 |
| RV64A        | Atomic                            |            22/22 |
| RV64F        | Single-precision floating-point   |            30/30 |
| RV64D        | Double-precision floating-point   |            32/32 |
| RV64Zicsr    | Control & status register         |            2/6   |
| RV64Zifencei | Instruction-fetch fence           |            0/1   |
| RV64C        | Compressed                        |            36/37 |

Test results:

| Test set   | #Failed/#Total |
| ---------- | -------------: |
| Memcheck   |         10/219 |
| Nulgrind   |          1/140 |
| Cachegrind |            0/7 |
| Callgrind  |           0/15 |
| DHAT       |            0/8 |
| DRD        |          4/130 |
| Helgrind   |           4/55 |
| Massif     |           2/37 |
| GDBserver  |           2/25 |

## License

This project is released under the terms of [the GPLv2 License](COPYING).

[Valgrind]: https://valgrind.org/
[RISC-V]: https://riscv.org/
[Linux]: https://github.com/torvalds/linux
[FOSDEM talk]: https://archive.fosdem.org/2022/schedule/event/valgrind_riscv/
