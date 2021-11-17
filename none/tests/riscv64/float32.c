/* Tests for the RV64F standard single-precision floating-point instruction-set
   extension. */

#include "testinst.h"

static void test_float32_shared(void)
{
   printf("RV64F single-precision FP instruction set, shared operations\n");

   /* --------------- flw rd, imm[11:0](rs1) ---------------- */
   TESTINST_1_1_FLOAD(4, "flw fa0, 0(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, 4(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, 8(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, 16(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, 32(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, 64(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, 128(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, 256(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, 512(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, 1024(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, 2040(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, -4(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "flw fa0, -2048(a1)", fa0, a1);

   TESTINST_1_1_FLOAD(4, "flw fa4, 0(a5)", fa4, a5);

   /* --------------- fsw rs2, imm[11:0](rs1) --------------- */
   TESTINST_0_2_FSTORE(4, "fsw fa0, 0(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, 4(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, 8(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, 16(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, 32(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, 64(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, 128(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, 256(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, 512(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, 1024(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, 2040(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, -4(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsw fa0, -2048(a1)", 0xabcdef0123456789, fa0, a1);

   TESTINST_0_2_FSTORE(4, "fsw fa4, 0(a5)", 0xabcdef0123456789, fa4, a5);

   /* ------------ fmadd.s rd, rs1, rs2, rs3, rm ------------ */
   /* TODO Implement. */

   /* ------------ fmsub.s rd, rs1, rs2, rs3, rm ------------ */
   /* TODO Implement. */

   /* ----------- fnmsub.s rd, rs1, rs2, rs3, rm ------------ */
   /* TODO Implement. */

   /* ----------- fnmadd.s rd, rs1, rs2, rs3, rm ------------ */
   /* TODO Implement. */

   /* --------------- fadd.s rd, rs1, rs2, rm --------------- */
   /* TODO Implement. */

   /* --------------- fsub.s rd, rs1, rs2, rm --------------- */
   /* TODO Implement. */

   /* --------------- fmul.s rd, rs1, rs2, rm --------------- */
   /* TODO Implement. */

   /* --------------- fdiv.s rd, rs1, rs2, rm --------------- */
   /* TODO Implement. */

   /* ----------------- fsqrt.s rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fsgnj.s rd, rs1, rs2 ----------------- */
   /* TODO Implement. */

   /* ---------------- fsgnjn.s rd, rs1, rs2 ---------------- */
   /* TODO Implement. */

   /* ---------------- fsgnjx.s rd, rs1, rs2 ---------------- */
   /* TODO Implement. */

   /* ----------------- fmin.s rd, rs1, rs2 ----------------- */
   /* TODO Implement. */

   /* ----------------- fmax.s rd, rs1, rs2 ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.w.s rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.wu.s rd, rs1, rm ---------------- */
   /* TODO Implement. */

   /* ------------------- fmv.x.w rd, rs1 ------------------- */
   /* TODO Implement. */

   /* ----------------- feq.s rd, rs1, rs2 ------------------ */
   /* TODO Implement. */

   /* ----------------- flt.s rd, rs1, rs2 ------------------ */
   /* TODO Implement. */

   /* ----------------- fle.s rd, rs1, rs2 ------------------ */
   /* TODO Implement. */

   /* ------------------ fclass.s rd, rs1 ------------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.s.w rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.s.wu rd, rs1, rm ---------------- */
   /* TODO Implement. */

   /* ------------------- fmv.w.x rd, rs1 ------------------- */
   /* TODO Implement. */

   printf("\n");
}

static void test_float32_additions(void)
{
   printf("RV64F single-precision FP instruction set, additions\n");

   /* ---------------- fcvt.l.s rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.lu.s rd, rs1, rm ---------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.s.l rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.s.lu rd, rs1, rm ---------------- */
   /* TODO Implement. */
}

int main(void)
{
   test_float32_shared();
   test_float32_additions();
   return 0;
}
