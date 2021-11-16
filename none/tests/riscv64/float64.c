/* Tests for the RV64D standard double-precision floating-point instruction-set
   extension. */

#include "testinst.h"

static void test_float64_shared(void)
{
   printf("RV64D double-precision FP instruction set, shared operations\n");

   /* --------------- fld rd, imm[11:0](rs1) ---------------- */
   /* TODO Implement. */

   /* --------------- fsd rs2, imm[11:0](rs1) --------------- */
   /* TODO Implement. */

   /* ------------ fmadd.d rd, rs1, rs2, rs3, rm ------------ */
   /* TODO Implement. */

   /* ------------ fmsub.d rd, rs1, rs2, rs3, rm ------------ */
   /* TODO Implement. */

   /* ----------- fnmsub.d rd, rs1, rs2, rs3, rm ------------ */
   /* TODO Implement. */

   /* ----------- fnmadd.d rd, rs1, rs2, rs3, rm ------------ */
   /* TODO Implement. */

   /* --------------- fadd.d rd, rs1, rs2, rm --------------- */
   /* TODO Implement. */

   /* --------------- fsub.d rd, rs1, rs2, rm --------------- */
   /* TODO Implement. */

   /* --------------- fmul.d rd, rs1, rs2, rm --------------- */
   /* TODO Implement. */

   /* --------------- fdiv.d rd, rs1, rs2, rm --------------- */
   /* TODO Implement. */

   /* ----------------- fsqrt.d rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fsgnj.d rd, rs1, rs2 ----------------- */
   /* TODO Implement. */

   /* ---------------- fsgnjn.d rd, rs1, rs2 ---------------- */
   /* TODO Implement. */

   /* ---------------- fsgnjx.d rd, rs1, rs2 ---------------- */
   /* TODO Implement. */

   /* ----------------- fmin.d rd, rs1, rs2 ----------------- */
   /* TODO Implement. */

   /* ----------------- fmax.d rd, rs1, rs2 ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.s.d rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.d.s rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ----------------- feq.d rd, rs1, rs2 ------------------ */
   /* TODO Implement. */

   /* ----------------- flt.d rd, rs1, rs2 ------------------ */
   /* TODO Implement. */

   /* ----------------- fle.d rd, rs1, rs2 ------------------ */
   /* TODO Implement. */

   /* ------------------ fclass.d rd, rs1 ------------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.w.d rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.wu.d rd, rs1, rm ---------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.d.w rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.d.wu rd, rs1, rm ---------------- */
   /* TODO Implement. */

   printf("\n");
}

static void test_float64_additions(void)
{
   printf("RV64D double-precision FP instruction set, additions\n");

   /* ---------------- fcvt.l.d rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.lu.d rd, rs1, rm ---------------- */
   /* TODO Implement. */

   /* ------------------- fmv.x.d rd, rs1 ------------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.d.l rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.d.lu rd, rs1, rm ---------------- */
   /* TODO Implement. */

   /* ------------------- fmv.d.x rd, rs1 ------------------- */
   /* TODO Implement. */
}

int main(void)
{
   test_float64_shared();
   test_float64_additions();
   return 0;
}
