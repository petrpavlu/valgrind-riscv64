/* Tests for the RV64A standard atomic instruction-set extension. */

#include "testinst.h"

static void test_atomic_shared(void)
{
   printf("RV64A atomic instruction set, shared operations\n");

   /* ------------------- lr.w rd, (rs1) -------------------- */
   /* ----------------- sc.w rd, rs2, (rs1) ----------------- */
   TESTINST_2_1_LRSC(4, "lr.w a0, (a2)", "sc.w a1, a0, (a2)", a0, a1, a2);
   TESTINST_2_1_LRSC(4, "lr.w t4, (t6)", "sc.w t5, t4, (t6)", t4, t5, t6);

   /* -------------- amoswap.w rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amoswap.w a0, a1, (a2)", 0xabcdef0123456789, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amoswap.w t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* --------------- amoadd.w rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amoadd.w a0, a1, (a2)", 0xabcdef0123456789, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amoadd.w t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* --------------- amoxor.w rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amoxor.w a0, a1, (a2)", 0xabcdef0123456789, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amoxor.w t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* --------------- amoand.w rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amoand.w a0, a1, (a2)", 0xabcdef0123456789, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amoand.w t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* --------------- amoor.w rd, rs2, (rs1) ---------------- */
   TESTINST_1_2_AMOX(4, "amoor.w a0, a1, (a2)", 0xabcdef0123456789, a0, a1, a2);
   TESTINST_1_2_AMOX(4, "amoor.w t4, t5, (t6)", 0xabcdef0123456789, t4, t5, t6);

   /* --------------- amomin.w rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amomin.w a0, a1, (a2)", 0x0000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomin.w a0, a1, (a2)", 0x000000007fffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomin.w a0, a1, (a2)", 0x0000000080000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomin.w a0, a1, (a2)", 0x00000000ffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomin.w t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* --------------- amomax.w rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amomax.w a0, a1, (a2)", 0x0000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomax.w a0, a1, (a2)", 0x000000007fffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomax.w a0, a1, (a2)", 0x0000000080000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomax.w a0, a1, (a2)", 0x00000000ffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomax.w t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* -------------- amominu.w rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amominu.w a0, a1, (a2)", 0x0000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amominu.w a0, a1, (a2)", 0x000000007fffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amominu.w a0, a1, (a2)", 0x0000000080000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amominu.w a0, a1, (a2)", 0x00000000ffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amominu.w t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* -------------- amomaxu.w rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amomaxu.w a0, a1, (a2)", 0x0000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomaxu.w a0, a1, (a2)", 0x000000007fffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomaxu.w a0, a1, (a2)", 0x0000000080000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomaxu.w a0, a1, (a2)", 0x00000000ffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomaxu.w t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   printf("\n");
}

static void test_atomic_additions(void)
{
   printf("RV64A atomic instruction set, additions\n");

   /* ------------------- lr.d rd, (rs1) -------------------- */
   /* ----------------- sc.d rd, rs2, (rs1) ----------------- */
   TESTINST_2_1_LRSC(4, "lr.d a0, (a2)", "sc.d a1, a0, (a2)", a0, a1, a2);
   TESTINST_2_1_LRSC(4, "lr.d t4, (t6)", "sc.d t5, t4, (t6)", t4, t5, t6);

   /* -------------- amoswap.d rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amoswap.d a0, a1, (a2)", 0xabcdef0123456789, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amoswap.d t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* --------------- amoadd.d rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amoadd.d a0, a1, (a2)", 0xabcdef0123456789, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amoadd.d t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* --------------- amoxor.d rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amoxor.d a0, a1, (a2)", 0xabcdef0123456789, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amoxor.d t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* --------------- amoand.d rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amoand.d a0, a1, (a2)", 0xabcdef0123456789, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amoand.d t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* --------------- amoor.d rd, rs2, (rs1) ---------------- */
   TESTINST_1_2_AMOX(4, "amoor.d a0, a1, (a2)", 0xabcdef0123456789, a0, a1, a2);
   TESTINST_1_2_AMOX(4, "amoor.d t4, t5, (t6)", 0xabcdef0123456789, t4, t5, t6);

   /* --------------- amomin.d rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amomin.d a0, a1, (a2)", 0x0000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomin.d a0, a1, (a2)", 0x7fffffffffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomin.d a0, a1, (a2)", 0x8000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomin.d a0, a1, (a2)", 0xffffffffffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomin.d t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* --------------- amomax.d rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amomax.d a0, a1, (a2)", 0x0000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomax.d a0, a1, (a2)", 0x7fffffffffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomax.d a0, a1, (a2)", 0x8000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomax.d a0, a1, (a2)", 0xffffffffffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomax.d t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* -------------- amominu.d rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amominu.d a0, a1, (a2)", 0x0000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amominu.d a0, a1, (a2)", 0x7fffffffffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amominu.d a0, a1, (a2)", 0x8000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amominu.d a0, a1, (a2)", 0xffffffffffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amominu.d t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);

   /* -------------- amomaxu.d rd, rs2, (rs1) --------------- */
   TESTINST_1_2_AMOX(4, "amomaxu.d a0, a1, (a2)", 0x0000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomaxu.d a0, a1, (a2)", 0x7fffffffffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomaxu.d a0, a1, (a2)", 0x8000000000000000, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomaxu.d a0, a1, (a2)", 0xffffffffffffffff, a0, a1,
                     a2);
   TESTINST_1_2_AMOX(4, "amomaxu.d t4, t5, (t6)", 0xabcdef0123456789, t4, t5,
                     t6);
}

int main(void)
{
   test_atomic_shared();
   test_atomic_additions();
   return 0;
}
