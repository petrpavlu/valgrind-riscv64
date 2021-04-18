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

#define TESTINST_1_SP(instruction, sp_val, rd)                                 \
   {                                                                           \
      unsigned long out, scratch;                                              \
      __asm__ __volatile__(                                                    \
         "mv %[scratch], sp;"                                                  \
         "mv sp, %[sp_in];"                                                    \
         instruction ";"                                                       \
         "mv %[out], " #rd ";"                                                 \
         "mv sp, %[scratch];"                                                  \
         : [out] "=r"(out), [scratch] "=&r"(scratch)                           \
         : [sp_in] "r"((unsigned long)sp_val)                                  \
         : #rd);                                                               \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: sp=0x%016lx\n", (unsigned long)sp_val);                \
      printf("  output: %s=0x%016lx\n", #rd, out);                             \
   }

#define TESTINST_1_1_LOAD(instruction, rd, rs1)                                \
   {                                                                           \
      unsigned long  out;                                                      \
      const size_t   N    = 512;                                               \
      unsigned char* area = memalign16(N);                                     \
      unsigned char  area2[N];                                                 \
      for (size_t i = 0; i < N; i++)                                           \
         area[i] = area2[i] = rand_uchar();                                    \
      __asm__ __volatile__(                                                    \
         "mv " #rs1 ", %[area_mid];"                                           \
         instruction ";"                                                       \
         "mv %[out], " #rd ";"                                                 \
         : [out] "=r"(out)                                                     \
         : [area_mid] "r"(area + N / 2)                                        \
         : #rd, #rs1);                                                         \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=&area_mid\n", #rs1);                                \
      printf("  output: %s=0x%016lx\n", #rd, out);                             \
      show_block_diff(area2, area, N, N / 2);                                  \
      free(area);                                                              \
   }

#define TESTINST_0_2_STORE(instruction, rs2_val, rs2, rs1)                     \
   {                                                                           \
      const size_t   N    = 512;                                               \
      unsigned char* area = memalign16(N);                                     \
      unsigned char  area2[N];                                                 \
      for (size_t i = 0; i < N; i++)                                           \
         area[i] = area2[i] = rand_uchar();                                    \
      __asm__ __volatile__(                                                    \
         "mv " #rs2 ", %[rs2_in];"                                             \
         "mv " #rs1 ", %[area_mid];"                                           \
         instruction ";"                                                       \
         :                                                                     \
         : [rs2_in] "r"((unsigned long)rs2_val), [area_mid] "r"(area + N / 2)  \
         : #rs2, #rs1, "memory");                                              \
      printf("%s ::\n", instruction);                                          \
      printf("  inputs: %s=0x%016lx, %s=&area_mid\n", #rs2,                    \
             (unsigned long)rs2_val, #rs1);                                    \
      show_block_diff(area2, area, N, N / 2);                                  \
      free(area);                                                              \
   }

static __attribute__((noinline)) void test_compressed_00(void)
{
   printf("Compressed Instructions, Quadrant 0\n");

   /* ------------- c.addi4spn rd, nzuimm[9:2] -------------- */
   TESTINST_1_SP("c.addi4spn a0, sp, 4", 0x0000000000001000, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 8", 0x0000000000001000, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 16", 0x0000000000001000, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 32", 0x0000000000001000, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 64", 0x0000000000001000, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 128", 0x0000000000001000, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 256", 0x0000000000001000, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 512", 0x0000000000001000, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 1020", 0x0000000000001000, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 4", 0x000000007ffffffc, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 4", 0x00000000fffffffb, a0);
   TESTINST_1_SP("c.addi4spn a0, sp, 4", 0x00000000fffffffc, a0);
   TESTINST_1_SP("c.addi4spn a5, sp, 4", 0x0000000000001000, a0);

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

int main(void)
{
   test_compressed_00();
   return 0;
}
