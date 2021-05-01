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

/* Macros for testing individual instructions

   Naming is in form TESTINST_<#outputs>_<#inputs>_<suffix-id>.

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

#define JMP_RANGE(instruction, rs1_val, offset, rd, rs1)                       \
   {                                                                           \
      unsigned long work[4 /*out*/ + 2 /*spill*/] = {0, 0, 0, 0, 0, 0};        \
      /* work[0] = output rd value                                             \
         work[1] = address of the test instruction                             \
         work[2] = flag that rd is not the zero reg                            \
         work[3] = flag that rs1 is not the zero reg                           \
         work[4] = spill slot for rd                                           \
         work[5] = spill slot for rs1                                          \
       */                                                                      \
      register unsigned long* t1 asm("t1") = work;                             \
      __asm__ __volatile__(                                                    \
         ".if \"" #rd "\" != \"zero\";"                                        \
         "sd " #rd ", 32(%[work]);"    /* Spill rd. */                         \
         ".endif;"                                                             \
         ".if \"" #rs1 "\" != \"zero\";"                                       \
         "sd " #rs1 ", 40(%[work]);"   /* Spill rs1. */                        \
         "la " #rs1 ", " rs1_val ";"   /* Load the first input. */             \
         ".endif;"                                                             \
         "j 1f;"                                                               \
         ".option push;"                                                       \
         ".option norvc;"                                                      \
         /* Generate a target area for negative offset. */                     \
         ".if " #offset " < 0;"                                                \
         ".if 4096 + " #offset " > 0; .space 4096 + " #offset "; .endif;"      \
         "j 2f;"                                                               \
         ".if -" #offset " - 4 > 0; .space -" #offset " - 4; .endif;"          \
         ".else;"                                                              \
         ".space 4096;"                                                        \
         ".endif;"                                                             \
         "1:;"                                                                 \
         ".option rvc;" instruction "; .space 2; .option norvc;"               \
         /* Generate a target area for positive offset. */                     \
         ".if " #offset " > 0;"                                                \
         ".if " #offset " - 4 > 0; .space " #offset " - 4; .endif;"            \
         "j 2f;"                                                               \
         ".if 4094 - " #offset " > 0; .space 4094 - " #offset "; .endif;"      \
         ".else;"                                                              \
         ".space 4094;"                                                        \
         ".endif;"                                                             \
         "2:;"                                                                 \
         ".option pop;"                                                        \
         ".if \"" #rd "\" != \"zero\";"                                        \
         /* Calculate offset of the return address from the instruction        \
            address. */                                                        \
         "sd " #rd ", 0(%[work]);"     /* Store the output return address. */  \
         "la " #rd ", 1b;"                                                     \
         "sd " #rd ", 8(%[work]);"     /* Store address of the test instr. */  \
         "li " #rd ", 1;"                                                      \
         "sd " #rd ", 16(%[work]);"    /* Flag rd is not the zero reg. */      \
         "ld " #rd ", 32(%[work]);"    /* Reload rd. */                        \
         ".endif;"                                                             \
         ".if \"" #rs1 "\" != \"zero\";"                                       \
         "li " #rs1 ", 1;"                                                     \
         "sd " #rs1 ", 24(%[work]);"   /* Flag rs1 is not the zero reg. */     \
         "ld " #rs1 ", 40(%[work]);"   /* Reload rs1. */                       \
         ".endif;"                                                             \
         :                                                                     \
         : [work] "r"(t1)                                                      \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      if (work[3] != 0) /* If rs1 is not the zero register. */                 \
         printf("  inputs: %s=%s\n", #rs1, rs1_val);                           \
      if (work[2] != 0) /* If rd is not the zero register. */                  \
         printf("  output: %s=1f%+ld\n", #rd, (long)(work[0] - work[1]));      \
      printf("  target: reached\n");                                           \
   }

#define TESTINST_0_0_J_RANGE(instruction, offset)                              \
   JMP_RANGE(instruction, "0", offset, zero, zero)

#define TESTINST_0_1_BxxZ_RANGE(instruction, rs1_val, offset, rs1)             \
   JMP_RANGE(instruction, #rs1_val, offset, zero, rs1)

#define TESTINST_0_1_JR_RANGE(instruction, rs1_val, offset, rs1)               \
   JMP_RANGE(instruction, rs1_val, offset, zero, rs1)

#define TESTINST_1_1_JALR_RANGE(instruction, rs1_val, offset, rd, rs1)         \
   JMP_RANGE(instruction, rs1_val, offset, rd, rs1)

#define TESTINST_1_0_JAL_RANGE(instruction, offset, rd)                        \
   JMP_RANGE(instruction, "0", offset, rd, zero)

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
