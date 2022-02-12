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

#define ASMINST_2(instruction)                                                 \
   ".option push;"                                                             \
   ".option rvc;"                                                              \
   instruction ";"                                                             \
   ".option pop"

#define ASMINST_4(instruction)                                                 \
   ".option push;"                                                             \
   ".option norvc;"                                                            \
   instruction ";"                                                             \
   ".option pop"

#define TESTINST_0_0(length, instruction)                                      \
   {                                                                           \
      __asm__ __volatile__(ASMINST_##length(instruction));                     \
      printf("%s ::\n", instruction);                                          \
   }

#define TESTINST_1_0(length, instruction, rd)                                  \
   {                                                                           \
      unsigned long w[1 /*out*/ + 1 /*spill*/] = {0, 0};                       \
      /* w[0] = output rd value                                                \
         w[1] = spill slot for rd                                              \
       */                                                                      \
      register unsigned long* t1 asm("t1") = w;                                \
      __asm__ __volatile__(                                                    \
         "sd " #rd ", 8(%[w]);"        /* Spill rd. */                         \
         ASMINST_##length(instruction) ";"                                     \
         "sd " #rd ", 0(%[w]);"        /* Save result of the operation. */     \
         "ld " #rd ", 8(%[w]);"        /* Reload rd. */                        \
         :                                                                     \
         : [w] "r"(t1)                                                         \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  output: %s=0x%016lx\n", #rd, w[0]);                            \
   }

#define TESTINST_1_1(length, instruction, rs1_val, rd, rs1)                    \
   {                                                                           \
      unsigned long w[1 /*out*/ + 1 /*in*/ + 2 /*spill*/] = {                  \
         0, (unsigned long)rs1_val, 0, 0};                                     \
      /* w[0] = output rd value                                                \
         w[1] = input rs1 value                                                \
         w[2] = spill slot for rd                                              \
         w[3] = spill slot for rs1                                             \
       */                                                                      \
      register unsigned long* t1 asm("t1") = w;                                \
      __asm__ __volatile__(                                                    \
         "sd " #rd ", 16(%[w]);"       /* Spill rd. */                         \
         "sd " #rs1 ", 24(%[w]);"      /* Spill rs1. */                        \
         "ld " #rs1 ", 8(%[w]);"       /* Load the first input. */             \
         ASMINST_##length(instruction) ";"                                     \
         "sd " #rd ", 0(%[w]);"        /* Save result of the operation. */     \
         "ld " #rd ", 16(%[w]);"       /* Reload rd. */                        \
         "ld " #rs1 ", 24(%[w]);"      /* Reload rs1. */                       \
         :                                                                     \
         : [w] "r"(t1)                                                         \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=0x%016lx\n", #rs1, (unsigned long)rs1_val);         \
      printf("  output: %s=0x%016lx\n", #rd, w[0]);                            \
   }

#define TESTINST_1_2(length, instruction, rs1_val, rs2_val, rd, rs1, rs2)      \
   {                                                                           \
      unsigned long w[1 /*out*/ + 2 /*in*/ + 3 /*spill*/] = {                  \
         0, (unsigned long)rs1_val, (unsigned long)rs2_val, 0, 0, 0};          \
      /* w[0] = output rd value                                                \
         w[1] = input rs1 value                                                \
         w[2] = input rs2 value                                                \
         w[3] = spill slot for rd                                              \
         w[4] = spill slot for rs1                                             \
         w[5] = spill slot for rs2                                             \
       */                                                                      \
      register unsigned long* t1 asm("t1") = w;                                \
      __asm__ __volatile__(                                                    \
         "sd " #rd ", 24(%[w]);"       /* Spill rd. */                         \
         "sd " #rs1 ", 32(%[w]);"      /* Spill rs1. */                        \
         "sd " #rs2 ", 40(%[w]);"      /* Spill rs2. */                        \
         "ld " #rs1 ", 8(%[w]);"       /* Load the first input. */             \
         "ld " #rs2 ", 16(%[w]);"      /* Load the second input. */            \
         ASMINST_##length(instruction) ";"                                     \
         "sd " #rd ", 0(%[w]);"        /* Save result of the operation. */     \
         "ld " #rd ", 24(%[w]);"       /* Reload rd. */                        \
         "ld " #rs1 ", 32(%[w]);"      /* Reload rs1. */                       \
         "ld " #rs2 ", 40(%[w]);"      /* Reload rs2. */                       \
         :                                                                     \
         : [w] "r"(t1)                                                         \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=0x%016lx, %s=0x%016lx\n", #rs1,                     \
             (unsigned long)rs1_val, #rs2, (unsigned long)rs2_val);            \
      printf("  output: %s=0x%016lx\n", #rd, w[0]);                            \
   }

#define TYPED_LOAD(length, instruction, rd, rs1, ipref)                        \
   {                                                                           \
      const size_t   N     = 4096;                                             \
      unsigned char* area  = memalign16(N);                                    \
      unsigned char* area2 = memalign16(N);                                    \
      for (size_t i = 0; i < N; i++)                                           \
         area[i] = area2[i] = rand_uchar();                                    \
      unsigned long w[1 /*out*/ + 1 /*in*/ + 2 /*spill*/] = {                  \
         0, (unsigned long)(area2 + N / 2), 0, 0};                             \
      /* w[0] = output rd value                                                \
         w[1] = input rs1 value                                                \
         w[2] = spill slot for rd                                              \
         w[3] = spill slot for rs1                                             \
       */                                                                      \
      register unsigned long* t1 asm("t1") = w;                                \
      __asm__ __volatile__(                                                    \
         ipref "sd " #rd ", 16(%[w]);" /* Spill rd. */                         \
         "sd " #rs1 ", 24(%[w]);"      /* Spill rs1. */                        \
         "ld " #rs1 ", 8(%[w]);"       /* Load the first input. */             \
         ASMINST_##length(instruction) ";"                                     \
         ipref "sd " #rd ", 0(%[w]);"  /* Save result of the operation. */     \
         ipref "ld " #rd ", 16(%[w]);" /* Reload rd. */                        \
         "ld " #rs1 ", 24(%[w]);"      /* Reload rs1. */                       \
         :                                                                     \
         : [w] "r"(t1)                                                         \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=&area_mid\n", #rs1);                                \
      printf("  output: %s=0x%016lx\n", #rd, w[0]);                            \
      show_block_diff(area, area2, N, N / 2);                                  \
      free(area);                                                              \
   }

#define TESTINST_1_1_LOAD(length, instruction, rd, rs1)                        \
   TYPED_LOAD(length, instruction, rd, rs1, "")

#define TESTINST_1_1_FLOAD(length, instruction, rd, rs1)                       \
   TYPED_LOAD(length, instruction, rd, rs1, "f")

#define TYPED_STORE(length, instruction, rs2_val, rs2, rs1, ipref)             \
   {                                                                           \
      const size_t   N     = 4096;                                             \
      unsigned char* area  = memalign16(N);                                    \
      unsigned char* area2 = memalign16(N);                                    \
      for (size_t i = 0; i < N; i++)                                           \
         area[i] = area2[i] = rand_uchar();                                    \
      unsigned long w[2 /*in*/ + 2 /*spill*/] = {                              \
         (unsigned long)rs2_val, (unsigned long)(area2 + N / 2), 0, 0};        \
      /* w[0] = input rs2 value                                                \
         w[1] = input rs1 value                                                \
         w[2] = spill slot for rs2                                             \
         w[3] = spill slot for rs1                                             \
       */                                                                      \
      register unsigned long* t1 asm("t1") = w;                                \
      __asm__ __volatile__(                                                    \
         ipref "sd " #rs2 ", 16(%[w]);" /* Spill rs2. */                       \
         "sd " #rs1 ", 24(%[w]);"      /* Spill rs1. */                        \
         ipref "ld " #rs2 ", 0(%[w]);" /* Load the first input. */             \
         "ld " #rs1 ", 8(%[w]);"       /* Load the second input. */            \
         ASMINST_##length(instruction) ";"                                     \
         ipref "ld " #rs2 ", 16(%[w]);" /* Reload rs2. */                      \
         "ld " #rs1 ", 24(%[w]);"      /* Reload rs1. */                       \
         :                                                                     \
         : [w] "r"(t1)                                                         \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=0x%016lx, %s=&area_mid\n", #rs2,                    \
             (unsigned long)rs2_val, #rs1);                                    \
      show_block_diff(area, area2, N, N / 2);                                  \
      free(area);                                                              \
   }

#define TESTINST_0_2_STORE(length, instruction, rs2_val, rs2, rs1)             \
   TYPED_STORE(length, instruction, rs2_val, rs2, rs1, "")

#define TESTINST_0_2_FSTORE(length, instruction, rs2_val, rs2, rs1)            \
   TYPED_STORE(length, instruction, rs2_val, rs2, rs1, "f")

#define TESTINST_2_1_LRSC(length, lr_instruction, sc_instruction, lr_rd,       \
                          sc_rd, rs1)                                          \
   {                                                                           \
      const size_t   N     = 32;                                               \
      unsigned char* area  = memalign16(N);                                    \
      unsigned char* area2 = memalign16(N);                                    \
      for (size_t i = 0; i < N; i++)                                           \
         area2[i] = rand_uchar();                                              \
      unsigned long w[4 /*out*/ + 1 /*in*/ + 3 /*spill*/];                     \
      /* w[0] = output lr_rd value                                             \
         w[1] = modded lr_rd value                                             \
         w[2] = output sc_rd value, first instruction                          \
         w[3] = output sc_rd value, second instruction                         \
         w[4] = address of the area midpoint                                   \
         w[5] = spill slot for lr_rd                                           \
         w[6] = spill slot for sc_rd                                           \
         w[7] = spill slot for rs1                                             \
       */                                                                      \
      register unsigned long* t1 asm("t1") = w;                                \
      do {                                                                     \
         w[0] = w[1] = w[2] = w[3] = w[5] = w[6] = w[7] = 0;                   \
         w[4] = (unsigned long)(area2 + N / 2);                                \
         for (size_t i = 0; i < N; i++)                                        \
            area[i] = area2[i];                                                \
         __asm__ __volatile__(                                                 \
            "sd " #lr_rd ", 40(%[w]);" /* Spill lr_rd. */                      \
            "sd " #sc_rd ", 48(%[w]);" /* Spill sc_rd. */                      \
            "sd " #rs1 ", 56(%[w]);"   /* Spill rs1. */                        \
            "ld " #rs1 ", 32(%[w]);"   /* Load the first input. */             \
            /* Perform a load and create a reservation. */                     \
            ASMINST_##length(lr_instruction) ";"                               \
            "mv t2, " #lr_rd ";"       /* Record the loaded value. */          \
            /* Store a negated value which should succeed. */                  \
            "not " #lr_rd ", " #lr_rd ";" /* Modify the loaded value. */       \
            ASMINST_##length(sc_instruction) ";"                               \
            "sd t2, 0(%[w]);"          /* Save result of the lr operation. */  \
            "sd " #lr_rd" , 8(%[w]);"  /* Save result of the not operation. */ \
            "sd " #sc_rd ", 16(%[w]);" /* Save result of the sc operation. */  \
            /* Store back the original value which should now fail. */         \
            "mv " #lr_rd ", t2;"       /* Get the original value. */           \
            ASMINST_##length(sc_instruction) ";"                               \
            "sd " #sc_rd ", 24(%[w]);" /* Save result of the sc operation. */  \
            "ld " #lr_rd ", 40(%[w]);" /* Reload lr_rd. */                     \
            "ld " #sc_rd ", 48(%[w]);" /* Reload sc_rd. */                     \
            "ld " #rs1 ", 56(%[w]);"   /* Reload rs1. */                       \
            :                                                                  \
            : [w] "r"(t1)                                                      \
            : "t2", "memory");                                                 \
         /* Re-run the test in case it happens that the first sc instruction   \
            unexpectedly fails. */                                             \
      } while (w[2] != 0);                                                     \
      printf("%s ::\n", lr_instruction);                                       \
      printf("  inputs: %s=&area_mid\n", #rs1);                                \
      printf("  output: %s=0x%016lx\n", #lr_rd, w[0]);                         \
      printf("%s ::\n", sc_instruction);                                       \
      printf("  inputs: %s=&area_mid, %s=0x%016lx\n", #rs1, #lr_rd, w[1]);     \
      printf("  output: %s=0x%016lx\n", #sc_rd, w[2]);                         \
      show_block_diff(area, area2, N, N / 2);                                  \
      printf("%s ::\n", sc_instruction);                                       \
      printf("  inputs: %s=&area_mid, %s=0x%016lx\n", #rs1, #lr_rd, w[0]);     \
      printf("  output: %s=0x%016lx\n", #sc_rd, w[3]);                         \
      free(area);                                                              \
   }

#define TESTINST_1_2_AMOX(length, instruction, rs2_val, rd, rs2, rs1)          \
   {                                                                           \
      const size_t   N     = 32;                                               \
      unsigned char* area  = memalign16(N);                                    \
      unsigned char* area2 = memalign16(N);                                    \
      for (size_t i = 0; i < N; i++)                                           \
         area[i] = area2[i] = rand_uchar();                                    \
      unsigned long w[1 /*out*/ + 2 /*in*/ + 3 /*spill*/] = {                  \
         0, (unsigned long)rs2_val, (unsigned long)(area2 + N / 2), 0, 0, 0};  \
      /* w[0] = output rd value                                                \
         w[1] = input rs2 value                                                \
         w[2] = address of the area midpoint                                   \
         w[3] = spill slot for rd                                              \
         w[4] = spill slot for rs2                                             \
         w[5] = spill slot for rs1                                             \
       */                                                                      \
      register unsigned long* t1 asm("t1") = w;                                \
      __asm__ __volatile__(                                                    \
         "sd " #rd ", 24(%[w]);"       /* Spill rd. */                         \
         "sd " #rs2 ", 32(%[w]);"      /* Spill rs2. */                        \
         "sd " #rs1 ", 40(%[w]);"      /* Spill rs1. */                        \
         "ld " #rs2 ", 8(%[w]);"       /* Load the first input. */             \
         "ld " #rs1 ", 16(%[w]);"      /* Load the second input. */            \
         ASMINST_##length(instruction) ";"                                     \
         "sd " #rd ", 0(%[w]);"        /* Save result of the operation. */     \
         "ld " #rd ", 24(%[w]);"       /* Reload rd. */                        \
         "ld " #rs2 ", 32(%[w]);"      /* Reload rs2. */                       \
         "ld " #rs1 ", 40(%[w]);"      /* Reload rs1. */                       \
         :                                                                     \
         : [w] "r"(t1)                                                         \
         : "memory");                                                          \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=0x%016lx, %s=&area_mid\n", #rs2,                    \
             (unsigned long)rs2_val, #rs1);                                    \
      printf("  output: %s=0x%016lx\n", #rd, w[0]);                            \
      show_block_diff(area, area2, N, N / 2);                                  \
      free(area);                                                              \
   }

#define TESTINST_1_0_AUIPC(length, instruction, rd)                            \
   {                                                                           \
      unsigned long w[2 /*out*/ + 1 /*spill*/] = {0, 0, 0};                    \
      /* w[0] = output rd value                                                \
         w[1] = address of the test instruction                                \
         w[2] = spill slot for rd                                              \
       */                                                                      \
      register unsigned long* t1 asm("t1") = w;                                \
      __asm__ __volatile__(                                                    \
         "sd " #rd ", 16(%[w]);"       /* Spill rd. */                         \
         "1:;"                                                                 \
         ASMINST_##length(instruction) ";"                                     \
         "sd " #rd ", 0(%[w]);"        /* Save result of the operation. */     \
         "la t2, 1b;"                                                          \
         "sd t2, 8(%[w]);"             /* Store address of the test instr. */  \
         "ld " #rd ", 16(%[w]);"       /* Reload rd. */                        \
         :                                                                     \
         : [w] "r"(t1)                                                         \
         : "t2", "memory");                                                    \
      printf("%s ::\n", instruction);                                          \
      printf("  output: %s=1f%+ld\n", #rd, (long)(w[0] - w[1]));               \
   }

#define JMP_RANGE(length, instruction, rs1_val, rs2_val, offset, rd, rs1, rs2) \
   {                                                                           \
      unsigned long w[5 /*out*/ + 3 /*spill*/] = {0, 0, 0, 0, 0, 0, 0, 0};     \
      /* w[0] = output rd value                                                \
         w[1] = address of the test instruction                                \
         w[2] = flag that rd is valid                                          \
         w[3] = flag that rs1 is valid                                         \
         w[4] = flag that rs2 is valid                                         \
         w[5] = spill slot for rd                                              \
         w[6] = spill slot for rs1                                             \
         w[7] = spill slot for rs2                                             \
       */                                                                      \
      register unsigned long* t1 asm("t1") = w;                                \
      __asm__ __volatile__(                                                    \
         ".if \"" #rd "\" != \"unused\" && \"" #rd "\" != \"zero\";"           \
         "sd " #rd ", 40(%[w]);"       /* Spill rd. */                         \
         ".endif;"                                                             \
         ".if \"" #rs1 "\" != \"unused\" && \"" #rs1 "\" != \"zero\";"         \
         "sd " #rs1 ", 48(%[w]);"      /* Spill rs1. */                        \
         "la " #rs1 ", " rs1_val ";"   /* Load the first input. */             \
         ".endif;"                                                             \
         ".if \"" #rs2 "\" != \"unused\" && \"" #rs2 "\" != \"zero\";"         \
         "sd " #rs2 ", 56(%[w]);"      /* Spill rs2. */                        \
         "la " #rs2 ", " rs2_val ";"   /* Load the second input. */            \
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
         ASMINST_##length(instruction) ";"                                     \
         /* Generate a target area for positive offset. */                     \
         ".if " #length " == 2; .space 2; .endif;"                             \
         ".if " #offset " > 0;"                                                \
         ".if " #offset " - 4 > 0; .space " #offset " - 4; .endif;"            \
         "j 2f;"                                                               \
         ".if 4094 - " #offset " > 0; .space 4094 - " #offset "; .endif;"      \
         ".else;"                                                              \
         ".space 4094;"                                                        \
         ".endif;"                                                             \
         "2:;"                                                                 \
         ".option pop;"                                                        \
         ".if \"" #rd "\" != \"unused\" && \"" #rd "\" != \"zero\";"           \
         "sd " #rd ", 0(%[w]);"        /* Store the output return address. */  \
         "la t2, 1b;"                                                          \
         "sd t2, 8(%[w]);"             /* Store address of the test instr. */  \
         "li t2, 1;"                                                           \
         "sd t2, 16(%[w]);"            /* Flag that rd is valid. */            \
         ".endif;"                                                             \
         ".if \"" #rs1 "\" != \"unused\";"                                     \
         "li t2, 1;"                                                           \
         "sd t2, 24(%[w]);"            /* Flag that rs1 is valid. */           \
         ".endif;"                                                             \
         ".if \"" #rs2 "\" != \"unused\";"                                     \
         "li t2, 1;"                                                           \
         "sd t2, 32(%[w]);"            /* Flag that rs2 is valid. */           \
         ".endif;"                                                             \
         ".if \"" #rd "\" != \"unused\" && \"" #rd "\" != \"zero\";"           \
         "ld " #rd ", 40(%[w]);"       /* Reload rd. */                        \
         ".endif;"                                                             \
         ".if \"" #rs1 "\" != \"unused\" && \"" #rs1 "\" != \"zero\";"         \
         "ld " #rs1 ", 48(%[w]);"      /* Reload rs1. */                       \
         ".endif;"                                                             \
         ".if \"" #rs2 "\" != \"unused\" && \"" #rs2 "\" != \"zero\";"         \
         "ld " #rs2 ", 56(%[w]);"      /* Reload rs2. */                       \
         ".endif;"                                                             \
         :                                                                     \
         : [w] "r"(t1)                                                         \
         : "t2", "memory");                                                    \
      printf("%s ::\n", instruction);                                          \
      if (w[3] != 0) { /* If rs1 is valid. */                                  \
         printf("  inputs: %s=%s", #rs1, rs1_val);                             \
         if (w[4] != 0) /* If rs2 is valid. */                                 \
            printf(", %s=%s", #rs2, rs2_val);                                  \
         printf("\n");                                                         \
      }                                                                        \
      if (w[2] != 0) /* If rd is valid. */                                     \
         printf("  output: %s=1f%+ld\n", #rd, (long)(w[0] - w[1]));            \
      printf("  target: reached\n");                                           \
   }

#define TESTINST_0_0_J_RANGE(length, instruction, offset)                      \
   JMP_RANGE(length, instruction, "0", "0", offset, unused, unused, unused)

#define TESTINST_0_1_JR_RANGE(length, instruction, rs1_val, offset, rs1)       \
   JMP_RANGE(length, instruction, rs1_val, "0", offset, unused, rs1, unused)

#define TESTINST_1_0_JAL_RANGE(length, instruction, offset, rd)                \
   JMP_RANGE(length, instruction, "0", "0", offset, rd, unused, unused)

#define TESTINST_1_1_JALR_RANGE(length, instruction, rs1_val, offset, rd, rs1) \
   JMP_RANGE(length, instruction, rs1_val, "0", offset, rd, rs1, unused)

#define TESTINST_0_1_BxxZ_RANGE(length, instruction, rs1_val, offset, rs1)     \
   JMP_RANGE(length, instruction, #rs1_val, "0", offset, unused, rs1, unused)

#define TESTINST_0_2_Bxx_RANGE(length, instruction, rs1_val, rs2_val, offset,  \
                               rs1, rs2)                                       \
   JMP_RANGE(length, instruction, #rs1_val, #rs2_val, offset, unused, rs1, rs2)

#define JMP_COND(length, instruction, rs1_val, rs2_val, rs1, rs2)              \
   {                                                                           \
      unsigned long w[3 /*out*/ + 2 /*spill*/] = {0, 0, 0, 0, 0};              \
      /* w[0] = flag that the branch was taken                                 \
         w[1] = flag that rs1 is valid                                         \
         w[2] = flag that rs2 is valid                                         \
         w[3] = spill slot for rs1                                             \
         w[4] = spill slot for rs2                                             \
       */                                                                      \
      register unsigned long* t1 asm("t1") = w;                                \
      __asm__ __volatile__(                                                    \
         "li t2, 1;"                                                           \
         "sd t2, 0(%[w]);"             /* Set result to "taken". */            \
         ".if \"" #rs1 "\" != \"unused\" && \"" #rs1 "\" != \"zero\";"         \
         "sd " #rs1 ", 24(%[w]);"      /* Spill rs1. */                        \
         "la " #rs1 ", " rs1_val ";"   /* Load the first input. */             \
         ".endif;"                                                             \
         ".if \"" #rs2 "\" != \"unused\" && \"" #rs2 "\" != \"zero\";"         \
         "sd " #rs2 ", 32(%[w]);"      /* Spill rs2. */                        \
         "la " #rs2 ", " rs2_val ";"   /* Load the second input. */            \
         ".endif;"                                                             \
         ASMINST_##length(instruction) ";"                                     \
         "li t2, 0;"                                                           \
         "sd t2, 0(%[w]);"             /* Set result to "not taken". */        \
         "1:;"                                                                 \
         ".if \"" #rs1 "\" != \"unused\";"                                     \
         "li t2, 1;"                                                           \
         "sd t2, 8(%[w]);"             /* Flag that rs1 is valid. */           \
         ".endif;"                                                             \
         ".if \"" #rs2 "\" != \"unused\";"                                     \
         "li t2, 1;"                                                           \
         "sd t2, 16(%[w]);"            /* Flag that rs2 is valid. */           \
         ".endif;"                                                             \
         ".if \"" #rs1 "\" != \"unused\" && \"" #rs1 "\" != \"zero\";"         \
         "ld " #rs1 ", 24(%[w]);"      /* Reload rs1. */                       \
         ".endif;"                                                             \
         ".if \"" #rs2 "\" != \"unused\" && \"" #rs2 "\" != \"zero\";"         \
         "ld " #rs2 ", 32(%[w]);"      /* Reload rs2. */                       \
         ".endif;"                                                             \
         :                                                                     \
         : [w] "r"(t1)                                                         \
         : "t2", "memory");                                                    \
      printf("%s ::\n", instruction);                                          \
      if (w[1] != 0) { /* If rs1 is valid. */                                  \
         printf("  inputs: %s=%s", #rs1, rs1_val);                             \
         if (w[2] != 0) /* If rs2 is valid. */                                 \
            printf(", %s=%s", #rs2, rs2_val);                                  \
         printf("\n");                                                         \
      }                                                                        \
      printf("  branch: %s\n", w[0] ? "taken" : "not taken");                  \
   }

#define TESTINST_0_1_BxxZ_COND(length, instruction, rs1_val, rs1)              \
   JMP_COND(length, instruction, #rs1_val, "0", rs1, unused)

#define TESTINST_0_2_Bxx_COND(length, instruction, rs1_val, rs2_val, rs1, rs2) \
   JMP_COND(length, instruction, #rs1_val, #rs2_val, rs1, rs2)

/* clang-format on */
