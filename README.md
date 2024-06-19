# Valgrind &ndash; RISCV64/Linux

## Overview

This repository contains [Valgrind][Valgrind] with support for the
[RISCV64][RISC-V]/[Linux][Linux] platform.

The aim of the project is to enable Valgrind for the RV64GC instruction set on
the Linux operating system. It is intended for this port to eventually become
a part of the upstream Valgrind project and to continue further development
there.

For installation, please follow the generic steps how to build Valgrind in [the
main README file](README).

The project was presented at [FOSDEM 2022][FOSDEM talk]. Work on integrating the
code into the official Valgrind project has been tracked in [an upstream
bug][submit bug].

## Current state

Current focus is on functionality and correctness. The following summarizes the
high-level state as of 2024-06-18.

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

    == 737 tests, 3 stderr failures, 0 stdout failures, 0 stderrB failures, 1 stdoutB failure, 0 post failures ==
    gdbserver_tests/hgtls                    (stdoutB)
    none/tests/double_close_range            (stderr)
    none/tests/double_close_range_sup        (stderr)
    none/tests/double_close_range_xml        (stderr)

For limitations and to-do tasks, please refer to
[README.riscv64](README.riscv64).

## License

This project is released under the terms of [the GPLv2 License](COPYING).

[Valgrind]: https://valgrind.org/
[RISC-V]: https://riscv.org/
[Linux]: https://github.com/torvalds/linux
[FOSDEM talk]: https://archive.fosdem.org/2022/schedule/event/valgrind_riscv/
[submit bug]: https://bugs.kde.org/show_bug.cgi?id=468575
