#include "tests/malloc.h"
#include <stdbool.h>
#include <stdio.h>

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

#define TESTINST1_SP(instruction, sp_val, rd)                                  \
   {                                                                           \
      unsigned long out, scratch;                                              \
      __asm__ __volatile__("mv %[scratch], sp;"                                \
                           "mv sp, %[sp_in];" instruction ";"                  \
                           "mv sp, %[scratch];"                                \
                           "mv %[out], " #rd ";"                               \
                           : [out] "=r"(out), [scratch] "=&r"(scratch)         \
                           : [sp_in] "r"(sp_val)                               \
                           : #rd, "memory");                                   \
      printf("%s\n", instruction);                                             \
      printf("  inputs: sp=%#016lx\n", (unsigned long)sp_val);                 \
      printf("  output: %s=%#016lx\n", #rd, out);                              \
   }

#define TESTINST2_LOAD(instruction, rd, rs1)                                   \
   {                                                                           \
      unsigned long  out;                                                      \
      const size_t   N    = 512;                                               \
      unsigned char* area = memalign16(N);                                     \
      unsigned char  area2[N];                                                 \
      for (size_t i = 0; i < N; i++)                                           \
         area[i] = area2[i] = rand_uchar();                                    \
      __asm__ __volatile__("mv " #rs1 ", %[area_mid];" instruction ";"         \
                           "mv %[out], " #rd ";"                               \
                           : [out] "=r"(out)                                   \
                           : [area_mid] "r"(area + N / 2)                      \
                           : #rd, #rs1, "memory");                             \
      printf("%s\n", instruction);                                             \
      printf("  inputs: %s=&area_mid\n", #rs1);                                \
      printf("  output: %s=%#016lx\n", #rd, out);                              \
      show_block_diff(area2, area, N, N / 2);                                  \
      free(area);                                                              \
   }

#define TESTINST2_STORE(instruction, rs2_val, rs2, rs1)                        \
   {                                                                           \
      const size_t   N    = 512;                                               \
      unsigned char* area = memalign16(N);                                     \
      unsigned char  area2[N];                                                 \
      for (size_t i = 0; i < N; i++)                                           \
         area[i] = area2[i] = rand_uchar();                                    \
      __asm__ __volatile__(                                                    \
         "mv " #rs2 ", %[rs2_in];"                                             \
         "mv " #rs1 ", %[area_mid];" instruction ";"                           \
         :                                                                     \
         : [rs2_in] "r"(rs2_val), [area_mid] "r"(area + N / 2)                 \
         : #rs2, #rs1, "memory");                                              \
      printf("%s\n", instruction);                                             \
      printf("  inputs: %s=%#016lx, %s=&area_mid\n", #rs2,                     \
             (unsigned long)rs2_val, #rs1);                                    \
      printf("  output: none\n");                                              \
      show_block_diff(area2, area, N, N / 2);                                  \
      free(area);                                                              \
   }

static __attribute__((noinline)) void test_compressed_00(void)
{
   printf("Compressed Instructions, Quadrant 0\n");

   /* ------------- c.addi4spn rd, nzuimm[9:2] -------------- */
   TESTINST1_SP("c.addi4spn a0, sp, 4", 0xdb432311d1e3a1d5, a0);
   TESTINST1_SP("c.addi4spn a5, sp, 4", 0xfd79baaee550b488, a5);
   TESTINST1_SP("c.addi4spn a0, sp, 1020", 0xc58586ea2c6954df, a0);
   TESTINST1_SP("c.addi4spn a5, sp, 1020", 0xfb834ed5b21de6b5, a5);

   /* -------------- c.fld rd, uimm[7:3](rs1) --------------- */
   /* TODO */

   /* --------------- c.lw rd, uimm[6:2](rs1) --------------- */
   TESTINST2_LOAD("c.lw a0, 0(a1)", a0, a1);
   TESTINST2_LOAD("c.lw a0, 124(a1)", a0, a1);

   /* --------------- c.ld rd, uimm[7:3](rs1) --------------- */
   TESTINST2_LOAD("c.ld a0, 0(a1)", a0, a1);
   TESTINST2_LOAD("c.ld a0, 248(a1)", a0, a1);

   /* -------------- c.fsd rs2, uimm[7:3](rs1) -------------- */
   /* TODO */

   /* -------------- c.sw rs2, uimm[6:2](rs1) --------------- */
   TESTINST2_STORE("c.sw a0, 0(a1)", 0xdb432311d1e3a1d5, a0, a1);
   TESTINST2_STORE("c.sw a0, 124(a1)", 0xfd79baaee550b488, a0, a1);

   /* -------------- c.sd rs2, uimm[7:3](rs1) --------------- */
   TESTINST2_STORE("c.sd a0, 0(a1)", 0xdb432311d1e3a1d5, a0, a1);
   TESTINST2_STORE("c.sd a0, 248(a1)", 0xfd79baaee550b488, a0, a1);
}

int main(void)
{
   test_compressed_00();
   return 0;
}
