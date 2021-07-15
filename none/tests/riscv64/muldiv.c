/* Tests for the RV64M standard multiplication and division instruction-set
   extension. */

#include "testinst.h"

static void test_muldiv_shared(void)
{
   printf(
      "RV64M multiplication and division instruction set, shared operations\n");

   /* ------------------ mul rd, rs1, rs2 ------------------- */
   TESTINST_1_2(4, "mul a0, a1, a2", 0x0000000000005000, 0x0000000000002000, a0,
                a1, a2);
   TESTINST_1_2(4, "mul a0, a1, a2", 0x7fffffffffffffff, 0x0000000000000002, a0,
                a1, a2);
   TESTINST_1_2(4, "mul a0, a1, a2", 0x7fffffffffffffff, 0x7fffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "mul a0, a1, a2", 0x7fffffffffffffff, 0xffffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "mul a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "mul a0, a1, a2", 0x8000000000000000, 0x0000000000000002, a0,
                a1, a2);
   TESTINST_1_2(4, "mul a0, a1, a2", 0x8000000000000000, 0xffffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "mul a0, a1, a2", 0x8000000000000000, 0x8000000000000000, a0,
                a1, a2);
   TESTINST_1_2(4, "mul a0, a1, a2", 0x0000000000000001, 0x0000000000000000, a0,
                a1, a2);
   TESTINST_1_2(4, "mul a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000, a0,
                a1, a2);
   TESTINST_1_2(4, "mul t4, t5, t6", 0x0000000000001000, 0x0000000000002000, t4,
                t5, t6);

   /* ------------------ mulh rd, rs1, rs2 ------------------ */
#if 0 /* TODO Enable. */
   TESTINST_1_2(4, "mulh a0, a1, a2", 0x0000000000005000, 0x0000000000002000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulh a0, a1, a2", 0x7fffffffffffffff, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "mulh a0, a1, a2", 0x7fffffffffffffff, 0x7fffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulh a0, a1, a2", 0x7fffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulh a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulh a0, a1, a2", 0x8000000000000000, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "mulh a0, a1, a2", 0x8000000000000000, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulh a0, a1, a2", 0x8000000000000000, 0x8000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulh a0, a1, a2", 0x0000000000000001, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulh a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulh t4, t5, t6", 0x0000000000001000, 0x0000000000002000,
                t4, t5, t6);
#endif

   /* ----------------- mulhsu rd, rs1, rs2 ----------------- */
#if 0 /* TODO Enable. */
   TESTINST_1_2(4, "mulhsu a0, a1, a2", 0x0000000000005000, 0x0000000000002000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhsu a0, a1, a2", 0x7fffffffffffffff, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhsu a0, a1, a2", 0x7fffffffffffffff, 0x7fffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhsu a0, a1, a2", 0x7fffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhsu a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhsu a0, a1, a2", 0x8000000000000000, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhsu a0, a1, a2", 0x8000000000000000, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhsu a0, a1, a2", 0x8000000000000000, 0x8000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhsu a0, a1, a2", 0x0000000000000001, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhsu a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhsu t4, t5, t6", 0x0000000000001000, 0x0000000000002000,
                t4, t5, t6);
#endif

   /* ----------------- mulhu rd, rs1, rs2 ------------------ */
   TESTINST_1_2(4, "mulhu a0, a1, a2", 0x0000000000005000, 0x0000000000002000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhu a0, a1, a2", 0x7fffffffffffffff, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhu a0, a1, a2", 0x7fffffffffffffff, 0x7fffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhu a0, a1, a2", 0x7fffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhu a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhu a0, a1, a2", 0x8000000000000000, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhu a0, a1, a2", 0x8000000000000000, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhu a0, a1, a2", 0x8000000000000000, 0x8000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhu a0, a1, a2", 0x0000000000000001, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhu a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulhu t4, t5, t6", 0x0000000000001000, 0x0000000000002000,
                t4, t5, t6);

   /* ------------------ div rd, rs1, rs2 ------------------- */
   TESTINST_1_2(4, "div a0, a1, a2", 0x0000000000005000, 0x0000000000002000, a0,
                a1, a2);
   TESTINST_1_2(4, "div a0, a1, a2", 0x7fffffffffffffff, 0x0000000000000002, a0,
                a1, a2);
   TESTINST_1_2(4, "div a0, a1, a2", 0x7fffffffffffffff, 0x7fffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "div a0, a1, a2", 0x7fffffffffffffff, 0xffffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "div a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "div a0, a1, a2", 0x8000000000000000, 0x0000000000000002, a0,
                a1, a2);
   TESTINST_1_2(4, "div a0, a1, a2", 0x8000000000000000, 0xffffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "div a0, a1, a2", 0x8000000000000000, 0x8000000000000000, a0,
                a1, a2);
   TESTINST_1_2(4, "div a0, a1, a2", 0x0000000000000001, 0x0000000000000000, a0,
                a1, a2);
   TESTINST_1_2(4, "div a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000, a0,
                a1, a2);
   TESTINST_1_2(4, "div t4, t5, t6", 0x0000000000005000, 0x0000000000002000, t4,
                t5, t6);

   /* ------------------ divu rd, rs1, rs2 ------------------ */
   TESTINST_1_2(4, "divu a0, a1, a2", 0x0000000000005000, 0x0000000000002000,
                a0, a1, a2);
   TESTINST_1_2(4, "divu a0, a1, a2", 0x7fffffffffffffff, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "divu a0, a1, a2", 0x7fffffffffffffff, 0x7fffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divu a0, a1, a2", 0x7fffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divu a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divu a0, a1, a2", 0x8000000000000000, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "divu a0, a1, a2", 0x8000000000000000, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divu a0, a1, a2", 0x8000000000000000, 0x8000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "divu a0, a1, a2", 0x0000000000000001, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "divu a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "divu t4, t5, t6", 0x0000000000005000, 0x0000000000002000,
                t4, t5, t6);

   /* ------------------ rem rd, rs1, rs2 ------------------- */
   TESTINST_1_2(4, "rem a0, a1, a2", 0x0000000000005000, 0x0000000000002000, a0,
                a1, a2);
   TESTINST_1_2(4, "rem a0, a1, a2", 0x7fffffffffffffff, 0x0000000000000002, a0,
                a1, a2);
   TESTINST_1_2(4, "rem a0, a1, a2", 0x7fffffffffffffff, 0x7fffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "rem a0, a1, a2", 0x7fffffffffffffff, 0xffffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "rem a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "rem a0, a1, a2", 0x8000000000000000, 0x0000000000000002, a0,
                a1, a2);
   TESTINST_1_2(4, "rem a0, a1, a2", 0x8000000000000000, 0xffffffffffffffff, a0,
                a1, a2);
   TESTINST_1_2(4, "rem a0, a1, a2", 0x8000000000000000, 0x8000000000000000, a0,
                a1, a2);
   TESTINST_1_2(4, "rem a0, a1, a2", 0x0000000000000001, 0x0000000000000000, a0,
                a1, a2);
   TESTINST_1_2(4, "rem a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000, a0,
                a1, a2);
   TESTINST_1_2(4, "rem t4, t5, t6", 0x0000000000005000, 0x0000000000002000, t4,
                t5, t6);

   /* ------------------ remu rd, rs1, rs2 ------------------ */
   TESTINST_1_2(4, "remu a0, a1, a2", 0x0000000000005000, 0x0000000000002000,
                a0, a1, a2);
   TESTINST_1_2(4, "remu a0, a1, a2", 0x7fffffffffffffff, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "remu a0, a1, a2", 0x7fffffffffffffff, 0x7fffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remu a0, a1, a2", 0x7fffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remu a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remu a0, a1, a2", 0x8000000000000000, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "remu a0, a1, a2", 0x8000000000000000, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remu a0, a1, a2", 0x8000000000000000, 0x8000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "remu a0, a1, a2", 0x0000000000000001, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "remu a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "remu t4, t5, t6", 0x0000000000005000, 0x0000000000002000,
                t4, t5, t6);

   printf("\n");
}

static void test_muldiv_additions(void)
{
   printf("RV64M multiplication and division instruction set, additions\n");

   /* ------------------ mulw rd, rs1, rs2 ------------------ */
   TESTINST_1_2(4, "mulw a0, a1, a2", 0x0000000000005000, 0x0000000000002000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulw a0, a1, a2", 0x000000007fffffff, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "mulw a0, a1, a2", 0x000000007fffffff, 0x000000007fffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulw a0, a1, a2", 0x000000007fffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulw a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulw a0, a1, a2", 0x0000000080000000, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "mulw a0, a1, a2", 0x0000000080000000, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "mulw a0, a1, a2", 0x0000000080000000, 0x0000000080000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulw a0, a1, a2", 0x0000000000000001, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulw a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "mulw t4, t5, t6", 0x0000000000001000, 0x0000000000002000,
                t4, t5, t6);

   /* ------------------ divw rd, rs1, rs2 ------------------ */
   TESTINST_1_2(4, "divw a0, a1, a2", 0x0000000000005000, 0x0000000000002000,
                a0, a1, a2);
   TESTINST_1_2(4, "divw a0, a1, a2", 0x000000007fffffff, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "divw a0, a1, a2", 0x000000007fffffff, 0x000000007fffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divw a0, a1, a2", 0x000000007fffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divw a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divw a0, a1, a2", 0x0000000080000000, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "divw a0, a1, a2", 0x0000000080000000, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divw a0, a1, a2", 0x0000000080000000, 0x0000000080000000,
                a0, a1, a2);
   TESTINST_1_2(4, "divw a0, a1, a2", 0x0000000000000001, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "divw a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "divw t4, t5, t6", 0x0000000000001000, 0x0000000000002000,
                t4, t5, t6);

   /* ----------------- divuw rd, rs1, rs2 ------------------ */
   TESTINST_1_2(4, "divuw a0, a1, a2", 0x0000000000005000, 0x0000000000002000,
                a0, a1, a2);
   TESTINST_1_2(4, "divuw a0, a1, a2", 0x000000007fffffff, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "divuw a0, a1, a2", 0x000000007fffffff, 0x000000007fffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divuw a0, a1, a2", 0x000000007fffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divuw a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divuw a0, a1, a2", 0x0000000080000000, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "divuw a0, a1, a2", 0x0000000080000000, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "divuw a0, a1, a2", 0x0000000080000000, 0x0000000080000000,
                a0, a1, a2);
   TESTINST_1_2(4, "divuw a0, a1, a2", 0x0000000000000001, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "divuw a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "divuw t4, t5, t6", 0x0000000000001000, 0x0000000000002000,
                t4, t5, t6);

   /* ------------------ remw rd, rs1, rs2 ------------------ */
   TESTINST_1_2(4, "remw a0, a1, a2", 0x0000000000005000, 0x0000000000002000,
                a0, a1, a2);
   TESTINST_1_2(4, "remw a0, a1, a2", 0x000000007fffffff, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "remw a0, a1, a2", 0x000000007fffffff, 0x000000007fffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remw a0, a1, a2", 0x000000007fffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remw a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remw a0, a1, a2", 0x0000000080000000, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "remw a0, a1, a2", 0x0000000080000000, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remw a0, a1, a2", 0x0000000080000000, 0x0000000080000000,
                a0, a1, a2);
   TESTINST_1_2(4, "remw a0, a1, a2", 0x0000000000000001, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "remw a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "remw t4, t5, t6", 0x0000000000001000, 0x0000000000002000,
                t4, t5, t6);

   /* ----------------- remuw rd, rs1, rs2 ------------------ */
   TESTINST_1_2(4, "remuw a0, a1, a2", 0x0000000000005000, 0x0000000000002000,
                a0, a1, a2);
   TESTINST_1_2(4, "remuw a0, a1, a2", 0x000000007fffffff, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "remuw a0, a1, a2", 0x000000007fffffff, 0x000000007fffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remuw a0, a1, a2", 0x000000007fffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remuw a0, a1, a2", 0xffffffffffffffff, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remuw a0, a1, a2", 0x0000000080000000, 0x0000000000000002,
                a0, a1, a2);
   TESTINST_1_2(4, "remuw a0, a1, a2", 0x0000000080000000, 0xffffffffffffffff,
                a0, a1, a2);
   TESTINST_1_2(4, "remuw a0, a1, a2", 0x0000000080000000, 0x0000000080000000,
                a0, a1, a2);
   TESTINST_1_2(4, "remuw a0, a1, a2", 0x0000000000000001, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "remuw a0, a1, a2", 0xffffffffffffffff, 0x0000000000000000,
                a0, a1, a2);
   TESTINST_1_2(4, "remuw t4, t5, t6", 0x0000000000001000, 0x0000000000002000,
                t4, t5, t6);
}

int main(void)
{
   test_muldiv_shared();
   test_muldiv_additions();
   return 0;
}
