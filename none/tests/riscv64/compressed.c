#include "tests/malloc.h"
#include <stdbool.h>
#include <stdio.h>

/* Helper functions. */

static inline unsigned char rand_uchar(void)
{
   static unsigned int seed = 80021;

   seed = 1103515245 * seed + 12345;
   return (seed >> 17) & 0xFF;
}

static void show_block_diff(unsigned char* block1,
                            unsigned char* block2,
                            size_t         n,
                            size_t         offset)
{
   bool block_changed = false;
   for (size_t i = 0; i < n; i += 16) {
      bool line_changed = false;
      for (size_t j = i; j < n && j < i + 16; j++) {
         if (block1[j] != block2[j]) {
            line_changed = true;
            break;
         }
      }
      if (!line_changed)
         continue;

      if (i < offset)
         printf("  [-%03zu] ", offset - i);
      else
         printf("  [+%03zu] ", i - offset);
      for (size_t j = i; j < n && j < i + 16; j++) {
         unsigned char diff = block1[j] - block2[j];
         if (diff == 0)
            printf(" ..");
         else
            printf(" %02x", block2[j]);
      }
      printf("\n");

      block_changed = true;
   }
   if (!block_changed)
      printf("  no memory changes\n");
}

/* Macros to test individual instructions

   Naming is in form TESTINST_<#inputs>_<#outputs>_<suffix-id>.

   Environment to test each instruction is set up by a carefully crafted inline
   assembly. The code implements own handling of input and output operands
   which most importantly allows also use of the sp register as an instruction
   operand. Register t1 is reserved for this purpose and must be avoided in
   instruction tests.
 */

/* Disable clang-format for the test macros because it would mess up the inline
   assembly. */
/* clang-format off */

#define TESTINST_0_0(instruction)                                              \
   {                                                                           \
      __asm__ __volatile__(instruction);                                       \
      printf("%s ::\n", instruction);                                          \
   }

#define TESTINST_1_0(instruction, rd)                                          \
   {                                                                           \
      unsigned long work[1 /*out*/ + 1 /*spill*/] = {0};                       \
      register unsigned long* t1 asm("t1") = work;                             \
      __asm__ __volatile__(                                                    \
         "sd " #rd ", 8(%[work]);"     /* Spill rd. */                         \
         instruction ";"                                                       \
         "sd " #rd ", 0(%[work]);"     /* Save result of the operation. */     \
         "ld " #rd ", 8(%[work]);"     /* Reload rd. */                        \
         :                                                                     \
         : [work] "r"(t1)                                                      \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  output: %s=0x%016lx\n", #rd, work[0]);                         \
   }

#define TESTINST_1_1(instruction, rs1_val, rd, rs1)                            \
   {                                                                           \
      unsigned long work[1 /*out*/ + 1 /*in*/ + 2 /*spill*/] = {               \
         0, (unsigned long)rs1_val, 0, 0};                                     \
      register unsigned long* t1 asm("t1") = work;                             \
      __asm__ __volatile__(                                                    \
         "sd " #rd ", 16(%[work]);"    /* Spill rd. */                         \
         "sd " #rs1 ", 24(%[work]);"   /* Spill rs1. */                        \
         "ld " #rs1 ", 8(%[work]);"    /* Load the first input. */             \
         instruction ";"                                                       \
         "sd " #rd ", 0(%[work]);"     /* Save result of the operation. */     \
         "ld " #rd ", 16(%[work]);"    /* Reload rd. */                        \
         "ld " #rs1 ", 24(%[work]);"   /* Reload rs1. */                       \
         :                                                                     \
         : [work] "r"(t1)                                                      \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=0x%016lx\n", #rs1, (unsigned long)rs1_val);         \
      printf("  output: %s=0x%016lx\n", #rd, work[0]);                         \
   }

#define TESTINST_1_2(instruction, rs1_val, rs2_val, rd, rs1, rs2)              \
   {                                                                           \
      unsigned long work[1 /*out*/ + 2 /*in*/ + 3 /*spill*/] = {               \
         0, (unsigned long)rs1_val, (unsigned long)rs2_val, 0, 0};             \
      register unsigned long* t1 asm("t1") = work;                             \
      __asm__ __volatile__(                                                    \
         "sd " #rd ", 24(%[work]);"    /* Spill rd. */                         \
         "sd " #rs1 ", 32(%[work]);"   /* Spill rs1. */                        \
         "sd " #rs2 ", 40(%[work]);"   /* Spill rs2. */                        \
         "ld " #rs1 ", 8(%[work]);"    /* Load the first input. */             \
         "ld " #rs2 ", 16(%[work]);"   /* Load the second input. */            \
         instruction ";"                                                       \
         "sd " #rd ", 0(%[work]);"     /* Save result of the operation. */     \
         "ld " #rd ", 24(%[work]);"    /* Reload rd. */                        \
         "ld " #rs1 ", 32(%[work]);"   /* Reload rs1. */                       \
         "ld " #rs2 ", 40(%[work]);"   /* Reload rs2. */                       \
         :                                                                     \
         : [work] "r"(t1)                                                      \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=0x%016lx, %s=0x%016lx\n", #rs1,                     \
             (unsigned long)rs1_val, #rs2, (unsigned long)rs2_val);            \
      printf("  output: %s=0x%016lx\n", #rd, work[0]);                         \
   }

#define TESTINST_1_1_LOAD(instruction, rd, rs1)                                \
   {                                                                           \
      const size_t   N    = 1024;                                              \
      unsigned char* area = memalign16(N);                                     \
      unsigned char  area2[N];                                                 \
      for (size_t i = 0; i < N; i++)                                           \
         area[i] = area2[i] = rand_uchar();                                    \
      unsigned long work[1 /*out*/ + 1 /*in*/ + 2 /*spill*/] = {               \
         0, (unsigned long)(area + N / 2), 0, 0};                              \
      register unsigned long* t1 asm("t1") = work;                             \
      __asm__ __volatile__(                                                    \
         "sd " #rd ", 16(%[work]);"    /* Spill rd. */                         \
         "sd " #rs1 ", 24(%[work]);"   /* Spill rs1. */                        \
         "ld " #rs1 ", 8(%[work]);"    /* Load the first input. */             \
         instruction ";"                                                       \
         "sd " #rd ", 0(%[work]);"     /* Save result of the operation. */     \
         "ld " #rd ", 16(%[work]);"    /* Reload rd. */                        \
         "ld " #rs1 ", 24(%[work]);"   /* Reload rs1. */                       \
         :                                                                     \
         : [work] "r"(t1)                                                      \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=&area_mid\n", #rs1);                                \
      printf("  output: %s=0x%016lx\n", #rd, work[0]);                         \
      show_block_diff(area2, area, N, N / 2);                                  \
      free(area);                                                              \
   }

#define TESTINST_0_2_STORE(instruction, rs2_val, rs2, rs1)                     \
   {                                                                           \
      const size_t   N    = 1024;                                              \
      unsigned char* area = memalign16(N);                                     \
      unsigned char  area2[N];                                                 \
      for (size_t i = 0; i < N; i++)                                           \
         area[i] = area2[i] = rand_uchar();                                    \
      unsigned long work[2 /*in*/ + 2 /*spill*/] = {                           \
         (unsigned long)rs2_val, (unsigned long)(area + N / 2), 0, 0};         \
      register unsigned long* t1 asm("t1") = work;                             \
      __asm__ __volatile__(                                                    \
         "sd " #rs2 ", 16(%[work]);"   /* Spill rs2. */                        \
         "sd " #rs1 ", 24(%[work]);"   /* Spill rs1. */                        \
         "ld " #rs2 ", 0(%[work]);"    /* Load the first input. */             \
         "ld " #rs1 ", 8(%[work]);"    /* Load the second input. */            \
         instruction ";"                                                       \
         "ld " #rs2 ", 16(%[work]);"   /* Reload rs2. */                       \
         "ld " #rs1 ", 24(%[work]);"   /* Reload rs1. */                       \
         :                                                                     \
         : [work] "r"(t1)                                                      \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=0x%016lx, %s=&area_mid\n", #rs2,                    \
             (unsigned long)rs2_val, #rs1);                                    \
      show_block_diff(area2, area, N, N / 2);                                  \
      free(area);                                                              \
   }

#define JMP_RANGE_ASM(instruction, offset)                                     \
   "j 1f;"                                                                     \
   ".option push;"                                                             \
   ".option norvc;"                                                            \
   /* Generate a target area for negative offset. */                           \
   ".if " #offset " < 0;"                                                      \
   ".if 4096 + " #offset " > 0; .space 4096 + " #offset "; .endif;"            \
   "j 2f;"                                                                     \
   ".if -" #offset " - 4 > 0; .space -" #offset " - 4; .endif;"                \
   ".else;"                                                                    \
   ".space 4096;"                                                              \
   ".endif;"                                                                   \
   "1:;"                                                                       \
   ".option rvc;" instruction "; .space 2; .option norvc;"                     \
   /* Generate a target area for positive offset. */                           \
   ".if " #offset " > 0;"                                                      \
   ".if " #offset " - 4 > 0; .space " #offset " - 4; .endif;"                  \
   "j 2f;"                                                                     \
   ".if 4094 - " #offset " > 0; .space 4094 - " #offset "; .endif;"            \
   ".else;"                                                                    \
   ".space 4094;"                                                              \
   ".endif;"                                                                   \
   "2:;"                                                                       \
   ".option pop;"

#define TESTINST_0_0_J_RANGE(instruction, offset)                              \
   {                                                                           \
      __asm__ __volatile__(JMP_RANGE_ASM(instruction, offset));                \
      printf("%s ::\n", instruction);                                          \
      printf("  target: reached\n");                                           \
   }

#define TESTINST_0_1_BxxZ_RANGE(instruction, rs1_val, offset, rs1)             \
   {                                                                           \
      unsigned long work[1 /*in*/ + 1 /*spill*/] = {                           \
         (unsigned long)rs1_val, 0};                                           \
      register unsigned long* t1 asm("t1") = work;                             \
      __asm__ __volatile__(                                                    \
         "sd " #rs1 ", 8(%[work]);"    /* Spill rs1. */                        \
         "ld " #rs1 ", 0(%[work]);"    /* Load the first input. */             \
         JMP_RANGE_ASM(instruction, offset)                                    \
         "ld " #rs1 ", 8(%[work]);"    /* Reload rs1. */                       \
         :                                                                     \
         : [work] "r"(t1)                                                      \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=0x%016lx\n", #rs1, (unsigned long)rs1_val);         \
      printf("  target: reached\n");                                           \
   }

#define TESTINST_0_1_BxxZ_COND(instruction, rs1_val, rs1)                      \
   {                                                                           \
      unsigned long work[1 /*out*/ + 1 /*in*/ + 1 /*spill*/] = {               \
         0, (unsigned long)rs1_val, 0};                                        \
      register unsigned long* t1 asm("t1") = work;                             \
      __asm__ __volatile__(                                                    \
         "sd " #rs1 ", 16(%[work]);"   /* Spill rs1. */                        \
         "li " #rs1 ", 1;"                                                     \
         "sd " #rs1 ", 0(%[work]);"    /* Set result to "taken". */            \
         "ld " #rs1 ", 8(%[work]);"    /* Load the first input. */             \
         instruction ";"                                                       \
         "li " #rs1 ", 0;"                                                     \
         "sd " #rs1 ", 0(%[work]);"    /* Set result to "not taken". */        \
         "1:;"                                                                 \
         "ld " #rs1 ", 16(%[work]);"   /* Reload rs1. */                       \
         :                                                                     \
         : [work] "r"(t1)                                                      \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=0x%016lx\n", #rs1, (unsigned long)rs1_val);         \
      printf("  branch: %s\n", work[0] ? "taken" : "not taken");               \
   }

/* clang-format on */

static __attribute__((noinline)) void test_compressed_00(void)
{
   printf("Compressed Instructions, Quadrant 0\n");

   /* ------------- c.addi4spn rd, nzuimm[9:2] -------------- */
   TESTINST_1_1("c.addi4spn a0, sp, 4", 0x0000000000001000, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 8", 0x0000000000001000, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 16", 0x0000000000001000, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 32", 0x0000000000001000, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 64", 0x0000000000001000, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 128", 0x0000000000001000, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 256", 0x0000000000001000, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 512", 0x0000000000001000, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 1020", 0x0000000000001000, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 4", 0x000000007ffffffc, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 4", 0x00000000fffffffb, a0, sp);
   TESTINST_1_1("c.addi4spn a0, sp, 4", 0x00000000fffffffc, a0, sp);
   TESTINST_1_1("c.addi4spn a5, sp, 4", 0x0000000000001000, a0, sp);

   /* -------------- c.fld rd, uimm[7:3](rs1) --------------- */
   /* TODO */

   /* --------------- c.lw rd, uimm[6:2](rs1) --------------- */
   TESTINST_1_1_LOAD("c.lw a0, 0(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.lw a0, 4(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.lw a0, 8(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.lw a0, 16(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.lw a0, 32(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.lw a0, 64(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.lw a0, 124(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.lw a4, 0(a5)", a4, a5);

   /* --------------- c.ld rd, uimm[7:3](rs1) --------------- */
   TESTINST_1_1_LOAD("c.ld a0, 0(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.ld a0, 8(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.ld a0, 16(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.ld a0, 32(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.ld a0, 64(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.ld a0, 128(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.ld a0, 248(a1)", a0, a1);
   TESTINST_1_1_LOAD("c.ld a4, 0(a5)", a4, a5);

   /* -------------- c.fsd rs2, uimm[7:3](rs1) -------------- */
   /* TODO */

   /* -------------- c.sw rs2, uimm[6:2](rs1) --------------- */
   TESTINST_0_2_STORE("c.sw a0, 0(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sw a0, 4(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sw a0, 8(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sw a0, 16(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sw a0, 32(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sw a0, 64(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sw a0, 124(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sw a4, 0(a5)", 0xabcdef0123456789, a4, a5);

   /* -------------- c.sd rs2, uimm[7:3](rs1) --------------- */
   TESTINST_0_2_STORE("c.sd a0, 0(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sd a0, 8(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sd a0, 16(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sd a0, 32(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sd a0, 64(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sd a0, 128(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sd a0, 248(a1)", 0xabcdef0123456789, a0, a1);
   TESTINST_0_2_STORE("c.sd a4, 0(a5)", 0xabcdef0123456789, a4, a5);

   printf("\n");
}

static __attribute__((noinline)) void test_compressed_01(void)
{
   printf("Compressed Instructions, Quadrant 1\n");

   /* ------------------------ c.nop ------------------------ */
   TESTINST_0_0("c.nop");

   /* -------------- c.addi rd_rs1, nzimm[5:0] -------------- */
   TESTINST_1_1("c.addi a0, 1", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addi a0, 2", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addi a0, 4", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addi a0, 8", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addi a0, 16", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addi a0, 31", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addi a0, -32", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addi a0, 1", 0x000000007fffffff, a0, a0);
   TESTINST_1_1("c.addi a0, 1", 0x00000000fffffffe, a0, a0);
   TESTINST_1_1("c.addi a0, 1", 0x00000000ffffffff, a0, a0);
   TESTINST_1_1("c.addi t6, 1", 0x0000000000001000, t6, t6);

   /* -------------- c.addiw rd_rs1, imm[5:0] --------------- */
   TESTINST_1_1("c.addiw a0, 0", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addiw a0, 1", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addiw a0, 2", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addiw a0, 4", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addiw a0, 8", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addiw a0, 16", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addiw a0, 31", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addiw a0, -32", 0x0000000000001000, a0, a0);
   TESTINST_1_1("c.addiw a0, 1", 0x000000007fffffff, a0, a0);
   TESTINST_1_1("c.addiw a0, 1", 0x00000000fffffffe, a0, a0);
   TESTINST_1_1("c.addiw a0, 1", 0x00000000ffffffff, a0, a0);
   TESTINST_1_1("c.addiw t6, 0", 0x0000000000001000, t6, t6);

   /* ------------------ c.li rd, imm[5:0] ------------------ */
   TESTINST_1_0("c.li a0, 0", a0);
   TESTINST_1_0("c.li a0, 1", a0);
   TESTINST_1_0("c.li a0, 2", a0);
   TESTINST_1_0("c.li a0, 4", a0);
   TESTINST_1_0("c.li a0, 8", a0);
   TESTINST_1_0("c.li a0, 15", a0);
   TESTINST_1_0("c.li a0, -16", a0);
   TESTINST_1_0("c.li t6, 0", t6);

   /* ---------------- c.addi16sp nzimm[9:4] ---------------- */
   TESTINST_1_1("c.addi16sp sp, 16", 0x0000000000001000, sp, sp);
   TESTINST_1_1("c.addi16sp sp, 32", 0x0000000000001000, sp, sp);
   TESTINST_1_1("c.addi16sp sp, 64", 0x0000000000001000, sp, sp);
   TESTINST_1_1("c.addi16sp sp, 128", 0x0000000000001000, sp, sp);
   TESTINST_1_1("c.addi16sp sp, 256", 0x0000000000001000, sp, sp);
   TESTINST_1_1("c.addi16sp sp, 496", 0x0000000000001000, sp, sp);
   TESTINST_1_1("c.addi16sp sp, -512", 0x0000000000001000, sp, sp);
   TESTINST_1_1("c.addi16sp sp, 16", 0x000000007ffffff0, sp, sp);
   TESTINST_1_1("c.addi16sp sp, 16", 0x00000000ffffffef, sp, sp);
   TESTINST_1_1("c.addi16sp sp, 16", 0x00000000fffffff0, sp, sp);

   /* --------------- c.lui rd, nzimm[17:12] ---------------- */
   TESTINST_1_0("c.lui a0, 1", a0);
   TESTINST_1_0("c.lui a0, 2", a0);
   TESTINST_1_0("c.lui a0, 4", a0);
   TESTINST_1_0("c.lui a0, 8", a0);
   TESTINST_1_0("c.lui a0, 16", a0);
   TESTINST_1_0("c.lui a0, 31", a0);
   TESTINST_1_0("c.lui a0, 0xfffe0" /* -32 */, a0);
   TESTINST_1_0("c.lui a0, 0xffff0" /* -16 */, a0);
   TESTINST_1_0("c.lui a0, 0xffff8" /* -8 */, a0);
   TESTINST_1_0("c.lui a0, 0xffffc" /* -4 */, a0);
   TESTINST_1_0("c.lui a0, 0xffffe" /* -2 */, a0);
   TESTINST_1_0("c.lui a0, 0xfffff" /* -1 */, a0);
   TESTINST_1_0("c.lui t6, 1", t6);

   /* ------------- c.srli rd_rs1, nzuimm[5:0] -------------- */
   TESTINST_1_1("c.srli a0, 1", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srli a0, 2", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srli a0, 4", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srli a0, 8", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srli a0, 16", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srli a0, 32", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srli a0, 63", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srli a5, 1", 0xabcdef0123456789, a5, a5);

   /* ------------- c.srai rd_rs1, nzuimm[5:0] -------------- */
   TESTINST_1_1("c.srai a0, 1", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srai a0, 2", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srai a0, 4", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srai a0, 8", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srai a0, 16", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srai a0, 32", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srai a0, 63", 0xabcdef0123456789, a0, a0);
   TESTINST_1_1("c.srai a5, 1", 0xabcdef0123456789, a5, a5);

   /* --------------- c.andi rd_rs1, imm[5:0] --------------- */
   TESTINST_1_1("c.andi a0, 0", 0xffffffffffffffff, a0, a0);
   TESTINST_1_1("c.andi a0, 1", 0xffffffffffffffff, a0, a0);
   TESTINST_1_1("c.andi a0, 2", 0xffffffffffffffff, a0, a0);
   TESTINST_1_1("c.andi a0, 4", 0xffffffffffffffff, a0, a0);
   TESTINST_1_1("c.andi a0, 8", 0xffffffffffffffff, a0, a0);
   TESTINST_1_1("c.andi a0, 16", 0xffffffffffffffff, a0, a0);
   TESTINST_1_1("c.andi a0, 31", 0xffffffffffffffff, a0, a0);
   TESTINST_1_1("c.andi a5, 0", 0xffffffffffffffff, a5, a5);

   /* ------------------ c.sub rd_rs1, rs2 ------------------ */
   TESTINST_1_2("c.sub a0, a1", 0x0000000000001000, 0x0000000000000fff, a0, a0,
                a1);
   TESTINST_1_2("c.sub a0, a1", 0x0000000000001000, 0x0000000000001000, a0, a0,
                a1);
   TESTINST_1_2("c.sub a0, a1", 0x0000000000001000, 0x0000000000001001, a0, a0,
                a1);
   TESTINST_1_2("c.sub a0, a1", 0xffffffffffffffff, 0x0000000000000000, a0, a0,
                a1);
   TESTINST_1_2("c.sub a0, a1", 0x0000000100000000, 0x0000000000000001, a0, a0,
                a1);
   TESTINST_1_2("c.sub a4, a5", 0x0000000000001000, 0x0000000000000fff, a4, a4,
                a5);

   /* ------------------ c.xor rd_rs1, rs2 ------------------ */
   TESTINST_1_2("c.xor a0, a1", 0x0000ffff0000ffff, 0x00000000ffffffff, a0, a0,
                a1);
   TESTINST_1_2("c.xor a4, a5", 0x0000ffff0000ffff, 0x00000000ffffffff, a4, a4,
                a5);

   /* ------------------ c.or rd_rs1, rs2 ------------------- */
   TESTINST_1_2("c.or a0, a1", 0x0000ffff0000ffff, 0x00000000ffffffff, a0, a0,
                a1);
   TESTINST_1_2("c.or a4, a5", 0x0000ffff0000ffff, 0x00000000ffffffff, a4, a4,
                a5);

   /* ------------------ c.and rd_rs1, rs2 ------------------ */
   TESTINST_1_2("c.and a0, a1", 0x0000ffff0000ffff, 0x00000000ffffffff, a0, a0,
                a1);
   TESTINST_1_2("c.and a4, a5", 0x0000ffff0000ffff, 0x00000000ffffffff, a4, a4,
                a5);

   /* ----------------- c.subw rd_rs1, rs2 ------------------ */
   TESTINST_1_2("c.subw a0, a1", 0x0000000000001000, 0x0000000000000fff, a0, a0,
                a1);
   TESTINST_1_2("c.subw a0, a1", 0x0000000000001000, 0x0000000000001000, a0, a0,
                a1);
   TESTINST_1_2("c.subw a0, a1", 0x0000000000001000, 0x0000000000001001, a0, a0,
                a1);
   TESTINST_1_2("c.subw a0, a1", 0xffffffffffffffff, 0x0000000000000000, a0, a0,
                a1);
   TESTINST_1_2("c.subw a0, a1", 0x0000000100000000, 0x0000000000000001, a0, a0,
                a1);
   TESTINST_1_2("c.subw a4, a5", 0x0000000000001000, 0x0000000000000fff, a4, a4,
                a5);

   /* ----------------- c.addw rd_rs1, rs2 ------------------ */
   TESTINST_1_2("c.addw a0, a1", 0x0000000000001000, 0x0000000000002000, a0, a0,
                a1);
   TESTINST_1_2("c.addw a0, a1", 0x000000007fffffff, 0x0000000000000001, a0, a0,
                a1);
   TESTINST_1_2("c.addw a0, a1", 0x00000000fffffffe, 0x0000000000000001, a0, a0,
                a1);
   TESTINST_1_2("c.addw a0, a1", 0x00000000ffffffff, 0x0000000000000001, a0, a0,
                a1);
   TESTINST_1_2("c.addw a0, a1", 0xfffffffffffffffe, 0x0000000000000001, a0, a0,
                a1);
   TESTINST_1_2("c.addw a0, a1", 0xffffffffffffffff, 0x0000000000000001, a0, a0,
                a1);
   TESTINST_1_2("c.addw a4, a5", 0x0000000000001000, 0x0000000000002000, a4, a4,
                a5);

   /* -------------------- c.j imm[11:1] -------------------- */
   TESTINST_0_0_J_RANGE("c.j .-2048", -2048);
   TESTINST_0_0_J_RANGE("c.j .-1024", -1024);
   TESTINST_0_0_J_RANGE("c.j .-512", -512);
   TESTINST_0_0_J_RANGE("c.j .-256", -256);
   TESTINST_0_0_J_RANGE("c.j .-128", -128);
   TESTINST_0_0_J_RANGE("c.j .-64", -64);
   TESTINST_0_0_J_RANGE("c.j .-32", -32);
   TESTINST_0_0_J_RANGE("c.j .-16", -16);
   TESTINST_0_0_J_RANGE("c.j .-8", -8);
   TESTINST_0_0_J_RANGE("c.j .-6", -6);
   TESTINST_0_0_J_RANGE("c.j .-4", -4);
   TESTINST_0_0_J_RANGE("c.j .+4", 4);
   TESTINST_0_0_J_RANGE("c.j .+6", 6);
   TESTINST_0_0_J_RANGE("c.j .+8", 8);
   TESTINST_0_0_J_RANGE("c.j .+16", 16);
   TESTINST_0_0_J_RANGE("c.j .+32", 32);
   TESTINST_0_0_J_RANGE("c.j .+64", 64);
   TESTINST_0_0_J_RANGE("c.j .+128", 128);
   TESTINST_0_0_J_RANGE("c.j .+256", 256);
   TESTINST_0_0_J_RANGE("c.j .+512", 512);
   TESTINST_0_0_J_RANGE("c.j .+1024", 1024);
   TESTINST_0_0_J_RANGE("c.j .+2044", 2044);

   /* ---------------- c.beqz rs1, imm[8:1] ----------------- */
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .-256", 0, -256, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .-128", 0, -128, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .-64", 0, -64, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .-32", 0, -32, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .-16", 0, -16, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .-8", 0, -8, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .-6", 0, -6, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .-4", 0, -4, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .+4", 0, 4, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .+6", 0, 6, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .+8", 0, 8, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .+16", 0, 16, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .+32", 0, 32, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .+64", 0, 64, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .+128", 0, 128, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a0, .+252", 0, 252, a0);
   TESTINST_0_1_BxxZ_RANGE("c.beqz a5, .-256", 0, -256, a5);
   TESTINST_0_1_BxxZ_COND("c.beqz a0, 1f", 0, a0);
   TESTINST_0_1_BxxZ_COND("c.beqz a0, 1f", 1, a0);
   TESTINST_0_1_BxxZ_COND("c.beqz a0, 1f", 0x8000000000000000, a0);

   /* ---------------- c.bnez rs1, imm[8:1] ----------------- */
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .-256", 1, -256, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .-128", 1, -128, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .-64", 1, -64, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .-32", 1, -32, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .-16", 1, -16, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .-8", 1, -8, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .-6", 1, -6, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .-4", 1, -4, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .+4", 1, 4, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .+6", 1, 6, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .+8", 1, 8, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .+16", 1, 16, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .+32", 1, 32, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .+64", 1, 64, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .+128", 1, 128, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a0, .+252", 1, 252, a0);
   TESTINST_0_1_BxxZ_RANGE("c.bnez a5, .-256", 1, -256, a5);
   TESTINST_0_1_BxxZ_COND("c.bnez a0, 1f", 0, a0);
   TESTINST_0_1_BxxZ_COND("c.bnez a0, 1f", 1, a0);
   TESTINST_0_1_BxxZ_COND("c.bnez a0, 1f", 0x8000000000000000, a0);

   printf("\n");
}

int main(void)
{
   test_compressed_00();
   test_compressed_01();
   return 0;
}
