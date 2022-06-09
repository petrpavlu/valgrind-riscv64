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
   /* 3.0 * 2.0 + 1.0 -> 7.0 */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff40400000,
                  0xffffffff40000000, 0xffffffff3f800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + -1.0 -> 0.0 */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffffbf800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * FLT_TRUE_MIN + -FLT_TRUE_MIN -> FLT_TRUE_MIN (no UF because exact)
    */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff00000001, 0xffffffff80000001, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * FLT_MAX + -FLT_MAX -> FLT_MAX */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f7fffff, 0xffffffffff7fffff, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * FLT_MAX + 0.0 -> INFINITY (OF, NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f7fffff, 0xffffffff00000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * INFINITY + -INFINITY -> qNAN (NV) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f800000, 0xffffffffff800000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* 1.0 * 1.0 + FLT_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3, rne", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafterf(1.0) + FLT_EPSILON/2 (RNE) -> 2nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3, rne", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + FLT_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3, rtz", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -FLT_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3, rtz", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffffb3800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + FLT_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3, rdn", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -FLT_EPSILON/2 (RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3, rdn", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffffb3800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + FLT_EPSILON/2 (RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3, rup", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -FLT_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3, rup", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffffb3800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + FLT_EPSILON/2 (RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3, rmm", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -FLT_EPSILON/2 (RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3, rmm", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffffb3800000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* 1.0 * 1.0 + FLT_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafterf(1.0) + FLT_EPSILON/2 (DYN-RNE) -> 2nextafterf(1.0) (NX)
    */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + FLT_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -FLT_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffffb3800000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + FLT_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -FLT_EPSILON/2 (DYN-RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffffb3800000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + FLT_EPSILON/2 (DYN-RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -FLT_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffffb3800000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + FLT_EPSILON/2 (DYN-RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x80, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -FLT_EPSILON/2 (DYN-RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffffb3800000, 0x80, fa0, fa1, fa2,
                  fa3);

   /* ------------ fmsub.s rd, rs1, rs2, rs3, rm ------------ */
   /* 3.0 * 2.0 - 1.0 -> 5.0 */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff40400000,
                  0xffffffff40000000, 0xffffffff3f800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 - 1.0 -> 0.0 */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff3f800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * FLT_TRUE_MIN - FLT_TRUE_MIN -> FLT_TRUE_MIN (no UF because exact)
    */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff00000001, 0xffffffff00000001, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * FLT_MAX - FLT_MAX -> FLT_MAX */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f7fffff, 0xffffffff7f7fffff, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * FLT_MAX - 0.0 -> INFINITY (OF, NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f7fffff, 0xffffffff00000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * INFINITY - INFINITY -> qNAN (NV) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f800000, 0xffffffff7f800000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* 1.0 * nextafterf(1.0) - FLT_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3, rne", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 2nextafterf(1.0) - FLT_EPSILON/2 (RNE) -> 2nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3, rne", 0xffffffff3f800000,
                  0xffffffff3f800002, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafterf(1.0) - FLT_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3, rtz", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - FLT_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3, rtz", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafterf(1.0) - FLT_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3, rdn", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - FLT_EPSILON/2 (RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3, rdn", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafterf(1.0) - FLT_EPSILON/2 (RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3, rup", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - FLT_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3, rup", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafterf(1.0) - FLT_EPSILON/2 (RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3, rmm", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - FLT_EPSILON/2 (RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3, rmm", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* 1.0 * nextafterf(1.0) - FLT_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 2nextafterf(1.0) - FLT_EPSILON/2 (DYN-RNE) -> 2nextafterf(1.0) (NX)
    */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800002, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafterf(1.0) - FLT_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - FLT_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffff33800000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafterf(1.0) - FLT_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - FLT_EPSILON/2 (DYN-RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffff33800000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafterf(1.0) - FLT_EPSILON/2 (DYN-RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - FLT_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffff33800000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafterf(1.0) - FLT_EPSILON/2 (DYN-RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x80, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - FLT_EPSILON/2 (DYN-RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffffbf800000, 0xffffffff33800000, 0x80, fa0, fa1, fa2,
                  fa3);

   /* ----------- fnmsub.s rd, rs1, rs2, rs3, rm ------------ */
   /* -(3.0 * 2.0) + 1.0 -> -5.0 */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffff40400000,
                  0xffffffff40000000, 0xffffffff3f800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + 1.0 -> 0.0 */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff3f800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * FLT_TRUE_MIN) + FLT_TRUE_MIN -> -FLT_TRUE_MIN (no UF because
      exact) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff00000001, 0xffffffff00000001, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * FLT_MAX) + FLT_MAX -> -FLT_MAX */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f7fffff, 0xffffffff7f7fffff, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * FLT_MAX) + 0.0 -> -INFINITY (OF, NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f7fffff, 0xffffffff00000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * INFINITY) + INFINITY -> qNAN (NV) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f800000, 0xffffffff7f800000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* -(-1.0 * 1.0) + FLT_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3, rne", 0xffffffffbf800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafterf(1.0)) + FLT_EPSILON/2 (RNE) -> 2nextafterf(1.0) (NX)
    */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3, rne", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + FLT_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3, rtz", 0xffffffffbf800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -FLT_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3, rtz", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffffb3800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + FLT_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3, rdn", 0xffffffffbf800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -FLT_EPSILON/2 (RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3, rdn", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffffb3800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + FLT_EPSILON/2 (RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3, rup", 0xffffffffbf800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -FLT_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3, rup", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffffb3800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + FLT_EPSILON/2 (RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3, rmm", 0xffffffffbf800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -FLT_EPSILON/2 (RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3, rmm", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffffb3800000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* -(-1.0 * 1.0) + FLT_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafterf(1.0)) + FLT_EPSILON/2 (DYN-RNE) ->
      2nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + FLT_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -FLT_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffffb3800000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + FLT_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -FLT_EPSILON/2 (DYN-RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffffb3800000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + FLT_EPSILON/2 (DYN-RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -FLT_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffffb3800000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + FLT_EPSILON/2 (DYN-RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x80, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -FLT_EPSILON/2 (DYN-RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffffb3800000, 0x80, fa0, fa1, fa2,
                  fa3);

   /* ----------- fnmadd.s rd, rs1, rs2, rs3, rm ------------ */
   /* -(3.0 * 2.0) - 1.0 -> -7.0 */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffff40400000,
                  0xffffffff40000000, 0xffffffff3f800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - -1.0 -> 0.0 */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffffbf800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * FLT_TRUE_MIN) - -FLT_TRUE_MIN -> -FLT_TRUE_MIN (no UF because
      exact) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff00000001, 0xffffffff80000001, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * FLT_MAX) - -FLT_MAX -> -FLT_MAX */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f7fffff, 0xffffffffff7fffff, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * FLT_MAX) - 0.0 -> -INFINITY (OF, NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f7fffff, 0xffffffff00000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * INFINITY) - -INFINITY -> qNAN (NV) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffff40000000,
                  0xffffffff7f800000, 0xffffffffff800000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* -(-1.0 * nextafterf(1.0)) - FLT_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3, rne", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 2nextafterf(1.0)) - FLT_EPSILON/2 (RNE) -> 2nextafterf(1.0) (NX)
    */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3, rne", 0xffffffffbf800000,
                  0xffffffff3f800002, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafterf(1.0)) - FLT_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3, rtz", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - FLT_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3, rtz", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafterf(1.0)) - FLT_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3, rdn", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - FLT_EPSILON/2 (RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3, rdn", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafterf(1.0)) - FLT_EPSILON/2 (RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3, rup", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - FLT_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3, rup", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafterf(1.0)) - FLT_EPSILON/2 (RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3, rmm", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - FLT_EPSILON/2 (RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3, rmm", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* -(-1.0 * nextafterf(1.0)) - FLT_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 2nextafterf(1.0)) - FLT_EPSILON/2 (DYN-RNE) -> 2nextafterf(1.0)
      (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800002, 0xffffffff33800000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafterf(1.0)) - FLT_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - FLT_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafterf(1.0)) - FLT_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - FLT_EPSILON/2 (DYN-RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafterf(1.0)) - FLT_EPSILON/2 (DYN-RUP) ->
      nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - FLT_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafterf(1.0)) - FLT_EPSILON/2 (DYN-RMM) ->
      nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffffbf800000,
                  0xffffffff3f800001, 0xffffffff33800000, 0x80, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - FLT_EPSILON/2 (DYN-RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.s fa0, fa1, fa2, fa3", 0xffffffff3f800000,
                  0xffffffff3f800000, 0xffffffff33800000, 0x80, fa0, fa1, fa2,
                  fa3);

   /* --------------- fadd.s rd, rs1, rs2, rm --------------- */
   /* 2.0 + 1.0 -> 3.0 */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff40000000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa2);
   /* 1.0 + -1.0 -> 0.0 */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffffbf800000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN + FLT_TRUE_MIN -> 2*FLT_TRUE_MIN (no UF because exact) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff00000001, 0x00, fa0, fa1, fa2);
   /* FLT_MAX + FLT_MAX -> INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff7f7fffff,
                  0xffffffff7f7fffff, 0x00, fa0, fa1, fa2);
   /* -FLT_MAX + -FLT_MAX -> -INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffffff7fffff,
                  0xffffffffff7fffff, 0x00, fa0, fa1, fa2);
   /* nextafterf(FLT_MIN) + -FLT_MIN -> FLT_TRUE_MIN (no UF because exact) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff00800001,
                  0xffffffff80800000, 0x00, fa0, fa1, fa2);
   /* INFINITY + -INFINITY -> qNAN (NV) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff7f800000,
                  0xffffffffff800000, 0x00, fa0, fa1, fa2);

   /* 1.0 + FLT_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2, rne", 0xffffffff3f800000,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* nextafterf(1.0) + FLT_EPSILON/2 (RNE) -> 2nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2, rne", 0xffffffff3f800001,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* 1.0 + FLT_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2, rtz", 0xffffffff3f800000,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* -1.0 + -FLT_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2, rtz", 0xffffffffbf800000,
                  0xffffffffb3800000, 0x00, fa0, fa1, fa2);
   /* 1.0 + FLT_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2, rdn", 0xffffffff3f800000,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* -1.0 + -FLT_EPSILON/2 (RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2, rdn", 0xffffffffbf800000,
                  0xffffffffb3800000, 0x00, fa0, fa1, fa2);
   /* 1.0 + FLT_EPSILON/2 (RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2, rup", 0xffffffff3f800000,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* -1.0 + -FLT_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2, rup", 0xffffffffbf800000,
                  0xffffffffb3800000, 0x00, fa0, fa1, fa2);
   /* 1.0 + FLT_EPSILON/2 (RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2, rmm", 0xffffffff3f800000,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* -1.0 + -FLT_EPSILON/2 (RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2, rmm", 0xffffffffbf800000,
                  0xffffffffb3800000, 0x00, fa0, fa1, fa2);

   /* 1.0 + FLT_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* nextafterf(1.0) + FLT_EPSILON/2 (DYN-RNE) -> 2nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff3f800001,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* 1.0 + FLT_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff33800000, 0x20, fa0, fa1, fa2);
   /* -1.0 + -FLT_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffffb3800000, 0x20, fa0, fa1, fa2);
   /* 1.0 + FLT_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff33800000, 0x40, fa0, fa1, fa2);
   /* -1.0 + -FLT_EPSILON/2 (DYN-RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffffb3800000, 0x40, fa0, fa1, fa2);
   /* 1.0 + FLT_EPSILON/2 (DYN-RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff33800000, 0x60, fa0, fa1, fa2);
   /* -1.0 + -FLT_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffffb3800000, 0x60, fa0, fa1, fa2);
   /* 1.0 + FLT_EPSILON/2 (DYN-RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff33800000, 0x80, fa0, fa1, fa2);
   /* -1.0 + -FLT_EPSILON/2 (DYN-RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffffb3800000, 0x80, fa0, fa1, fa2);

   /* --------------- fsub.s rd, rs1, rs2, rm --------------- */
   /* 2.0 - 1.0 -> 1.0 */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff40000000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa2);
   /* 1.0 - 1.0 -> 0.0 */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN - -FLT_TRUE_MIN -> 2*FLT_TRUE_MIN (no UF because exact) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff80000001, 0x00, fa0, fa1, fa2);
   /* FLT_MAX - -FLT_MAX -> INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff7f7fffff,
                  0xffffffffff7fffff, 0x00, fa0, fa1, fa2);
   /* -FLT_MAX - FLT_MAX -> -INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffffff7fffff,
                  0xffffffff7f7fffff, 0x00, fa0, fa1, fa2);
   /* nextafterf(FLT_MIN) - FLT_MIN -> FLT_TRUE_MIN (no UF because exact) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff00800001,
                  0xffffffff00800000, 0x00, fa0, fa1, fa2);
   /* INFINITY - INFINITY -> qNAN (NV) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff7f800000,
                  0xffffffff7f800000, 0x00, fa0, fa1, fa2);

   /* nextafterf(1.0) - FLT_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2, rne", 0xffffffff3f800001,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* 2nextafterf(1.0) - FLT_EPSILON/2 (RNE) -> 2nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2, rne", 0xffffffff3f800002,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* nextafterf(1.0) - FLT_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2, rtz", 0xffffffff3f800001,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* -1.0 - FLT_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2, rtz", 0xffffffffbf800000,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* nextafterf(1.0) - FLT_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2, rdn", 0xffffffff3f800001,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* -1.0 - FLT_EPSILON/2 (RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2, rdn", 0xffffffffbf800000,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* nextafterf(1.0) - FLT_EPSILON/2 (RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2, rup", 0xffffffff3f800001,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* -1.0 - FLT_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2, rup", 0xffffffffbf800000,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* nextafterf(1.0) - FLT_EPSILON/2 (RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2, rmm", 0xffffffff3f800001,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* -1.0 - FLT_EPSILON/2 (RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2, rmm", 0xffffffffbf800000,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);

   /* nextafterf(1.0) - FLT_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff3f800001,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* 2nextafterf(1.0) - FLT_EPSILON/2 (DYN-RNE) -> 2nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff3f800002,
                  0xffffffff33800000, 0x00, fa0, fa1, fa2);
   /* nextafterf(1.0) - FLT_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff3f800001,
                  0xffffffff33800000, 0x20, fa0, fa1, fa2);
   /* -1.0 - FLT_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffff33800000, 0x20, fa0, fa1, fa2);
   /* nextafterf(1.0) - FLT_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff3f800001,
                  0xffffffff33800000, 0x40, fa0, fa1, fa2);
   /* -1.0 - FLT_EPSILON/2 (DYN-RDN) -> -nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffff33800000, 0x40, fa0, fa1, fa2);
   /* nextafterf(1.0) - FLT_EPSILON/2 (DYN-RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff3f800001,
                  0xffffffff33800000, 0x60, fa0, fa1, fa2);
   /* -1.0 - FLT_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffff33800000, 0x60, fa0, fa1, fa2);
   /* nextafterf(1.0) - FLT_EPSILON/2 (DYN-RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffff3f800001,
                  0xffffffff33800000, 0x80, fa0, fa1, fa2);
   /* -1.0 - FLT_EPSILON/2 (DYN-RMM) -> -nextafterf(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffff33800000, 0x80, fa0, fa1, fa2);

   /* --------------- fmul.s rd, rs1, rs2, rm --------------- */
   /* 2.0 * 1.0 -> 2.0 */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff40000000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa2);
   /* 1.0 * 0.0 -> 0.0 */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff00000000, 0x00, fa0, fa1, fa2);
   /* 2**-74 * 2**-75 -> 2**-149 aka FLT_TRUE_MIN (no UF because exact) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff1a800000,
                  0xffffffff1a000000, 0x00, fa0, fa1, fa2);
   /* FLT_MAX * FLT_MAX -> INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff7f7fffff,
                  0xffffffff7f7fffff, 0x00, fa0, fa1, fa2);
   /* FLT_MAX * -FLT_MAX -> -INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff7f7fffff,
                  0xffffffffff7fffff, 0x00, fa0, fa1, fa2);
   /* 1.0 * INFINITY -> INFINITY */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff7f800000, 0x00, fa0, fa1, fa2);
   /* 0.0 * INFINITY -> qNAN (NV) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff7f800000, 0x00, fa0, fa1, fa2);

   /* FLT_TRUE_MIN * 0.5 (RNE) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2, rne", 0xffffffff00000001,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* 3*FLT_TRUE_MIN * 0.5 (RNE) -> 2*FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2, rne", 0xffffffff00000003,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN * 0.5 (RTZ) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2, rtz", 0xffffffff00000001,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN * 0.5 (RTZ) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2, rtz", 0xffffffff80000001,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN * 0.5 (RDN) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2, rdn", 0xffffffff00000001,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN * 0.5 (RDN) -> -FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2, rdn", 0xffffffff80000001,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN * 0.5 (RUP) -> FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2, rup", 0xffffffff00000001,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN * 0.5 (RUP) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2, rup", 0xffffffff80000001,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN * 0.5 (RMM) -> FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2, rmm", 0xffffffff00000001,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN * 0.5 (RMM) -> -FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2, rmm", 0xffffffff80000001,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);

   /* FLT_TRUE_MIN * 0.5 (DYN-RNE) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* 3*FLT_TRUE_MIN * 0.5 (DYN-RNE) -> 2*FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff00000003,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN * 0.5 (DYN-RTZ) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff3f000000, 0x20, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN * 0.5 (DYN-RTZ) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff80000001,
                  0xffffffff3f000000, 0x20, fa0, fa1, fa2);
   /* FLT_TRUE_MIN * 0.5 (DYN-RDN) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff3f000000, 0x40, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN * 0.5 (DYN-RDN) -> -FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff80000001,
                  0xffffffff3f000000, 0x40, fa0, fa1, fa2);
   /* FLT_TRUE_MIN * 0.5 (DYN-RUP) -> FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff3f000000, 0x60, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN * 0.5 (DYN-RUP) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff80000001,
                  0xffffffff3f000000, 0x60, fa0, fa1, fa2);
   /* FLT_TRUE_MIN * 0.5 (DYN-RMM) -> FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff3f000000, 0x80, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN * 0.5 (DYN-RMM) -> -FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.s fa0, fa1, fa2", 0xffffffff80000001,
                  0xffffffff3f000000, 0x80, fa0, fa1, fa2);

   /* --------------- fdiv.s rd, rs1, rs2, rm --------------- */
   /* 2.0 / 1.0 -> 2.0 */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff40000000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa2);
   /* 0.0 / 1.0 -> 0.0 */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa2);
   /* 1.0 / 2**127 -> 1**-127 (no UF because exact) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff7f000000, 0x00, fa0, fa1, fa2);
   /* FLT_MAX / 0.5 -> INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff7f7fffff,
                  0xffffffff3f000000, 0x00, fa0, fa1, fa2);
   /* FLT_MAX / -0.5 -> -INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff7f7fffff,
                  0xffffffffbf000000, 0x00, fa0, fa1, fa2);
   /* 1.0 / INFINITY -> 0.0 */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff7f800000, 0x00, fa0, fa1, fa2);
   /* 1.0 / 0.0 -> INFINITY (DZ) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff00000000, 0x00, fa0, fa1, fa2);
   /* 0.0 / 0.0 -> qNAN (NV) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff00000000, 0x00, fa0, fa1, fa2);

   /* FLT_TRUE_MIN / 2.0 (RNE) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2, rne", 0xffffffff00000001,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* 3*FLT_TRUE_MIN / 2.0 (RNE) -> 2*FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2, rne", 0xffffffff00000003,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN / 2.0 (RTZ) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2, rtz", 0xffffffff00000001,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN / 2.0 (RTZ) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2, rtz", 0xffffffff80000001,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN / 2.0 (RDN) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2, rdn", 0xffffffff00000001,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN / 2.0 (RDN) -> -FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2, rdn", 0xffffffff80000001,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN / 2.0 (RUP) -> FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2, rup", 0xffffffff00000001,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN / 2.0 (RUP) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2, rup", 0xffffffff80000001,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN / 2.0 (RMM) -> FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2, rmm", 0xffffffff00000001,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN / 2.0 (RMM) -> -FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2, rmm", 0xffffffff80000001,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);

   /* FLT_TRUE_MIN / 2.0 (DYN-RNE) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* 3*FLT_TRUE_MIN / 2.0 (DYN-RNE) -> 2*FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff00000003,
                  0xffffffff40000000, 0x00, fa0, fa1, fa2);
   /* FLT_TRUE_MIN / 2.0 (DYN-RTZ) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff40000000, 0x20, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN / 2.0 (DYN-RTZ) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff80000001,
                  0xffffffff40000000, 0x20, fa0, fa1, fa2);
   /* FLT_TRUE_MIN / 2.0 (DYN-RDN) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff40000000, 0x40, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN / 2.0 (DYN-RDN) -> -FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff80000001,
                  0xffffffff40000000, 0x40, fa0, fa1, fa2);
   /* FLT_TRUE_MIN / 2.0 (DYN-RUP) -> FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff40000000, 0x60, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN / 2.0 (DYN-RUP) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff80000001,
                  0xffffffff40000000, 0x60, fa0, fa1, fa2);
   /* FLT_TRUE_MIN / 2.0 (DYN-RMM) -> FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff00000001,
                  0xffffffff40000000, 0x80, fa0, fa1, fa2);
   /* -FLT_TRUE_MIN / 2.0 (DYN-RMM) -> -FLT_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.s fa0, fa1, fa2", 0xffffffff80000001,
                  0xffffffff40000000, 0x80, fa0, fa1, fa2);

   /* ----------------- fsqrt.s rd, rs1, rm ----------------- */
   /* sqrt(0.0) -> 0.0 */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff00000000, 0x00, fa0, fa1);
   /* sqrt(INFINITY) -> INFINITY */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff7f800000, 0x00, fa0, fa1);
   /* sqrt(2*FLT_TRUE_MIN) -> 2**-74 */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff00000002, 0x00, fa0, fa1);
   /* sqrt(qNAN) -> qNAN */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff7fc00000, 0x00, fa0, fa1);
   /* sqrt(-1.0) -> qNAN (NV) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffffbf800000, 0x00, fa0, fa1);

   /* sqrt(nextafterf(1.0)) (RNE) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1, rne", 0xffffffff3f800001, 0x00, fa0,
                  fa1);
   /* sqrt(2nextafterf(1.0)) (RNE) -> nextafterf(1.0) (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1, rne", 0xffffffff3f800002, 0x00, fa0,
                  fa1);
   /* sqrt(nextafterf(1.0)) (RTZ) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1, rtz", 0xffffffff3f800001, 0x00, fa0,
                  fa1);
   /* sqrt(2nextafterf(1.0)) (RTZ) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1, rtz", 0xffffffff3f800002, 0x00, fa0,
                  fa1);
   /* sqrt(nextafterf(1.0)) (RDN) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1, rdn", 0xffffffff3f800001, 0x00, fa0,
                  fa1);
   /* sqrt(2nextafterf(1.0)) (RDN) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1, rdn", 0xffffffff3f800002, 0x00, fa0,
                  fa1);
   /* sqrt(nextafterf(1.0)) (RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1, rup", 0xffffffff3f800001, 0x00, fa0,
                  fa1);
   /* sqrt(2nextafterf(1.0)) (RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1, rup", 0xffffffff3f800002, 0x00, fa0,
                  fa1);
   /* sqrt(nextafterf(1.0)) (RMM) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1, rmm", 0xffffffff3f800001, 0x00, fa0,
                  fa1);
   /* sqrt(2nextafterf(1.0)) (RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1, rmm", 0xffffffff3f800002, 0x00, fa0,
                  fa1);

   /* sqrt(nextafterf(1.0)) (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff3f800001, 0x00, fa0, fa1);
   /* sqrt(2nextafterf(1.0)) (DYN-RNE) -> nextafterf(1.0) (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff3f800002, 0x00, fa0, fa1);
   /* sqrt(nextafterf(1.0)) (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff3f800001, 0x20, fa0, fa1);
   /* sqrt(2nextafterf(1.0)) (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff3f800002, 0x20, fa0, fa1);
   /* sqrt(nextafterf(1.0)) (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff3f800001, 0x40, fa0, fa1);
   /* sqrt(2nextafterf(1.0)) (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff3f800002, 0x40, fa0, fa1);
   /* sqrt(nextafterf(1.0)) (DYN-RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff3f800001, 0x60, fa0, fa1);
   /* sqrt(2nextafterf(1.0)) (DYN-RUP) -> nextafterf(1.0) (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff3f800002, 0x60, fa0, fa1);
   /* sqrt(nextafterf(1.0)) (DYN-RMM) -> 1.0 (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff3f800001, 0x80, fa0, fa1);
   /* sqrt(2nextafterf(1.0)) (DYN-RMM) -> nextafterf(1.0) (NX) */
   TESTINST_1_1_F(4, "fsqrt.s fa0, fa1", 0xffffffff3f800002, 0x80, fa0, fa1);

   /* ---------------- fsgnj.s rd, rs1, rs2 ----------------- */
   /* fmv.s rd, rs1 */
   TESTINST_1_2_F(4, "fsgnj.s fa0, fa1, fa1", 0xffffffff3f800000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa1);
   TESTINST_1_2_F(4, "fsgnj.s fa0, fa1, fa1", 0xffffffffbf800000,
                  0xffffffffbf800000, 0x00, fa0, fa1, fa1);

   /* fsgnj(1.0, +) -> 1.0 */
   TESTINST_1_2_F(4, "fsgnj.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff7fffffff, 0x00, fa0, fa1, fa2);
   /* fsgnj(1.0, -) -> -1.0 */
   TESTINST_1_2_F(4, "fsgnj.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff80000000, 0x00, fa0, fa1, fa2);
   /* fsgnj(-1.0, +) -> 1.0 */
   TESTINST_1_2_F(4, "fsgnj.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffff7fffffff, 0x00, fa0, fa1, fa2);
   /* fsgnj(-1.0, -) -> -1.0 */
   TESTINST_1_2_F(4, "fsgnj.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffff80000000, 0x00, fa0, fa1, fa2);

   /* ---------------- fsgnjn.s rd, rs1, rs2 ---------------- */
   /* fneg.s rd, rs1 */
   TESTINST_1_2_F(4, "fsgnjn.s fa0, fa1, fa1", 0xffffffff3f800000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa1);
   TESTINST_1_2_F(4, "fsgnjn.s fa0, fa1, fa1", 0xffffffffbf800000,
                  0xffffffffbf800000, 0x00, fa0, fa1, fa1);

   /* fsgnjn(1.0, +) -> -1.0 */
   TESTINST_1_2_F(4, "fsgnjn.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff7fffffff, 0x00, fa0, fa1, fa2);
   /* fsgnjn(1.0, -) -> 1.0 */
   TESTINST_1_2_F(4, "fsgnjn.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff80000000, 0x00, fa0, fa1, fa2);
   /* fsgnjn(-1.0, +) -> -1.0 */
   TESTINST_1_2_F(4, "fsgnjn.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffff7fffffff, 0x00, fa0, fa1, fa2);
   /* fsgnjn(-1.0, -) -> 1.0 */
   TESTINST_1_2_F(4, "fsgnjn.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffff80000000, 0x00, fa0, fa1, fa2);

   /* ---------------- fsgnjx.s rd, rs1, rs2 ---------------- */
   /* fabs.s rd, rs1 */
   TESTINST_1_2_F(4, "fsgnjx.s fa0, fa1, fa1", 0xffffffff3f800000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa1);
   TESTINST_1_2_F(4, "fsgnjx.s fa0, fa1, fa1", 0xffffffffbf800000,
                  0xffffffffbf800000, 0x00, fa0, fa1, fa1);

   /* fsgnjx(1.0, +) -> 1.0 */
   TESTINST_1_2_F(4, "fsgnjx.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff7fffffff, 0x00, fa0, fa1, fa2);
   /* fsgnjx(1.0, -) -> -1.0 */
   TESTINST_1_2_F(4, "fsgnjx.s fa0, fa1, fa2", 0xffffffff3f800000,
                  0xffffffff80000000, 0x00, fa0, fa1, fa2);
   /* fsgnjx(-1.0, +) -> -1.0 */
   TESTINST_1_2_F(4, "fsgnjx.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffff7fffffff, 0x00, fa0, fa1, fa2);
   /* fsgnjx(-1.0, -) -> 1.0 */
   TESTINST_1_2_F(4, "fsgnjx.s fa0, fa1, fa2", 0xffffffffbf800000,
                  0xffffffff80000000, 0x00, fa0, fa1, fa2);

   /* ----------------- fmin.s rd, rs1, rs2 ----------------- */
   /* min(0.0, 1.0) -> 0.0 */
   TESTINST_1_2_F(4, "fmin.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa2);
   /* min(0.0, -0.0) -> -0.0 */
   TESTINST_1_2_F(4, "fmin.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff80000000, 0x00, fa0, fa1, fa2);
   /* min(-0.0, 0.0) -> -0.0 */
   TESTINST_1_2_F(4, "fmin.s fa0, fa1, fa2", 0xffffffff80000000,
                  0xffffffff00000000, 0x00, fa0, fa1, fa2);
   /* min(INFINITY, INFINITY) -> INFINITY */
   TESTINST_1_2_F(4, "fmin.s fa0, fa1, fa2", 0xffffffff7f800000,
                  0xffffffff7f800000, 0x00, fa0, fa1, fa2);
   /* min(0.0, qNAN) -> 0.0 */
   TESTINST_1_2_F(4, "fmin.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff7fc00000, 0x00, fa0, fa1, fa2);
   /* min(0.0, sNAN) -> 0.0 (NV) */
   TESTINST_1_2_F(4, "fmin.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff7fa00000, 0x00, fa0, fa1, fa2);

   /* ----------------- fmax.s rd, rs1, rs2 ----------------- */
   /* max(0.0, 1.0) -> 1.0 */
   TESTINST_1_2_F(4, "fmax.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff3f800000, 0x00, fa0, fa1, fa2);
   /* max(0.0, -0.0) -> 0.0 */
   TESTINST_1_2_F(4, "fmax.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff80000000, 0x00, fa0, fa1, fa2);
   /* max(-0.0, 0.0) -> 0.0 */
   TESTINST_1_2_F(4, "fmax.s fa0, fa1, fa2", 0xffffffff80000000,
                  0xffffffff00000000, 0x00, fa0, fa1, fa2);
   /* max(INFINITY, INFINITY) -> INFINITY */
   TESTINST_1_2_F(4, "fmax.s fa0, fa1, fa2", 0xffffffff7f800000,
                  0xffffffff7f800000, 0x00, fa0, fa1, fa2);
   /* max(0.0, qNAN) -> 0.0 */
   TESTINST_1_2_F(4, "fmax.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff7fc00000, 0x00, fa0, fa1, fa2);
   /* max(0.0, sNAN) -> 0.0 (NV) */
   TESTINST_1_2_F(4, "fmax.s fa0, fa1, fa2", 0xffffffff00000000,
                  0xffffffff7fa00000, 0x00, fa0, fa1, fa2);

   /* ---------------- fcvt.w.s rd, rs1, rm ----------------- */
   /* 0.0 -> 0 */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff00000000, 0x00, a0, fa0);
   /* FLT_TRUE_MIN -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff00000001, 0x00, a0, fa0);
   /* INFINITY -> 2**31-1 aka INT_MAX (NV)  */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff7f800000, 0x00, a0, fa0);
   /* qNAN -> 2**31-1 aka INT_MAX (NV) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff7fc00000, 0x00, a0, fa0);
   /* nextafterf(2**31, 0.0) -> 2**31-128 */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff4effffff, 0x00, a0, fa0);
   /* -2**31 -> -2**31 aka INT_MIN */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffffcf000000, 0x00, a0, fa0);
   /* 2**31 -> 2**31-1 aka INT_MAX (NV) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff4f000000, 0x00, a0, fa0);
   /* -nextafterf(2**31) -> -2**31 aka INT_MIN (NV) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffffcf000001, 0x00, a0, fa0);

   /* 1.0 (rd=zero) -> 0 */
   TESTINST_1_1_IF(4, "fcvt.w.s zero, fa0", 0xffffffff3f800000, 0x00, zero,
                   fa0);

   /* 0.5 (RNE) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0, rne", 0xffffffff3f000000, 0x00, a0,
                   fa0);
   /* 1.5 (RNE) -> 2 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0, rne", 0xffffffff3fc00000, 0x00, a0,
                   fa0);
   /* 0.5 (RTZ) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0, rtz", 0xffffffff3f000000, 0x00, a0,
                   fa0);
   /* -0.5 (RTZ) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0, rtz", 0xffffffffbf000000, 0x00, a0,
                   fa0);
   /* 0.5 (RDN) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0, rdn", 0xffffffff3f000000, 0x00, a0,
                   fa0);
   /* -0.5 (RDN) -> -1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0, rdn", 0xffffffffbf000000, 0x00, a0,
                   fa0);
   /* 0.5 (RUP) -> 1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0, rup", 0xffffffff3f000000, 0x00, a0,
                   fa0);
   /* -0.5 (RUP) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0, rup", 0xffffffffbf000000, 0x00, a0,
                   fa0);
   /* 0.5 (RMM) -> 1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0, rmm", 0xffffffff3f000000, 0x00, a0,
                   fa0);
   /* -0.5 (RMM) -> -1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0, rmm", 0xffffffffbf000000, 0x00, a0,
                   fa0);

   /* 0.5 (DYN-RNE) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff3f000000, 0x00, a0, fa0);
   /* 1.5 (DYN-RNE) -> 2 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff3fc00000, 0x00, a0, fa0);
   /* 0.5 (DYN-RTZ) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff3f000000, 0x20, a0, fa0);
   /* -0.5 (DYN-RTZ) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffffbf000000, 0x20, a0, fa0);
   /* 0.5 (DYN-RDN) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff3f000000, 0x40, a0, fa0);
   /* -0.5 (DYN-RDN) -> -1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffffbf000000, 0x40, a0, fa0);
   /* 0.5 (DYN-RUP) -> 1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff3f000000, 0x60, a0, fa0);
   /* -0.5 (DYN-RUP) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffffbf000000, 0x60, a0, fa0);
   /* 0.5 (DYN-RMM) -> 1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffff3f000000, 0x80, a0, fa0);
   /* -0.5 (DYN-RMM) -> -1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.w.s a0, fa0", 0xffffffffbf000000, 0x80, a0, fa0);

   /* ---------------- fcvt.wu.s rd, rs1, rm ---------------- */
   /* 0.0 -> 0 */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff00000000, 0x00, a0, fa0);
   /* FLT_TRUE_MIN -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff00000001, 0x00, a0, fa0);
   /* INFINITY -> 2**32-1 aka UINT_MAX (NV) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff7f800000, 0x00, a0, fa0);
   /* qNAN -> 2**32-1 aka UINT_MAX (NV) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff7fc00000, 0x00, a0, fa0);
   /* nextafter(2**32, 0.0) -> 2**32-256 */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff4f7fffff, 0x00, a0, fa0);
   /* 2**32 -> 2**32-1 aka UINT_MAX (NV) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff4f800000, 0x00, a0, fa0);
   /* -1.0 -> 0 (NV) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffffbf800000, 0x00, a0, fa0);

   /* 1.0 (rd=zero) -> 0 */
   TESTINST_1_1_IF(4, "fcvt.wu.s zero, fa0", 0xffffffff3f800000, 0x00, zero,
                   fa0);

   /* 0.5 (RNE) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0, rne", 0xffffffff3f000000, 0x00, a0,
                   fa0);
   /* 1.5 (RNE) -> 2 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0, rne", 0xffffffff3fc00000, 0x00, a0,
                   fa0);
   /* 0.5 (RTZ) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0, rtz", 0xffffffff3f000000, 0x00, a0,
                   fa0);
   /* 0.5 (RDN) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0, rdn", 0xffffffff3f000000, 0x00, a0,
                   fa0);
   /* 0.5 (RUP) -> 1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0, rup", 0xffffffff3f000000, 0x00, a0,
                   fa0);
   /* 0.5 (RMM) -> 1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0, rmm", 0xffffffff3f000000, 0x00, a0,
                   fa0);

   /* 0.5 (DYN-RNE) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff3f000000, 0x00, a0, fa0);
   /* 1.5 (DYN-RNE) -> 2 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff3fc00000, 0x00, a0, fa0);
   /* 0.5 (DYN-RTZ) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff3f000000, 0x20, a0, fa0);
   /* 0.5 (DYN-RDN) -> 0 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff3f000000, 0x40, a0, fa0);
   /* 0.5 (DYN-RUP) -> 1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff3f000000, 0x60, a0, fa0);
   /* 0.5 (DYN-RMM) -> 1 (NX) */
   TESTINST_1_1_IF(4, "fcvt.wu.s a0, fa0", 0xffffffff3f000000, 0x80, a0, fa0);

   /* ------------------- fmv.x.w rd, rs1 ------------------- */
   TESTINST_1_1_IF(4, "fmv.x.w a0, fa0", 0xabcdef0123456789, 0x00, a0, fa0);

   /* "0xffffffff7fffffff" -> "0x000000007fffffff" */
   TESTINST_1_1_IF(4, "fmv.x.w a0, fa0", 0xffffffff7fffffff, 0x00, a0, fa0);
   /* "0x0000000080000000" -> "0xffffffff80000000" */
   TESTINST_1_1_IF(4, "fmv.x.w a0, fa0", 0x0000000080000000, 0x00, a0, fa0);

   /* 1.0 (rd=zero) -> 0 */
   TESTINST_1_1_IF(4, "fmv.x.w zero, fa0", 0xffffffff3f800000, 0x00, zero, fa0);

   /* ----------------- feq.s rd, rs1, rs2 ------------------ */
   /* 0.0 == 1.0 -> 0 */
   TESTINST_1_2_FCMP(4, "feq.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff3f800000, 0x00, a0, fa0, fa1);
   /* 0.0 == 0.0 -> 1 */
   TESTINST_1_2_FCMP(4, "feq.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff00000000, 0x00, a0, fa0, fa1);
   /* 0.0 == -0.0 -> 1 */
   TESTINST_1_2_FCMP(4, "feq.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff80000000, 0x00, a0, fa0, fa1);
   /* -0.0 == 0.0 -> 1 */
   TESTINST_1_2_FCMP(4, "feq.s a0, fa0, fa1", 0xffffffff80000000,
                     0xffffffff00000000, 0x00, a0, fa0, fa1);
   /* INFINITY == INFINITY -> 1 */
   TESTINST_1_2_FCMP(4, "feq.s a0, fa0, fa1", 0xffffffff7f800000,
                     0xffffffff7f800000, 0x00, a0, fa0, fa1);
   /* 0.0 == qNAN -> 0 */
   TESTINST_1_2_FCMP(4, "feq.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff7fc00000, 0x00, a0, fa0, fa1);
   /* 0.0 == sNAN -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "feq.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff7fa00000, 0x00, a0, fa0, fa1);

   /* sNAN == sNAN (rd=zero) -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "feq.s zero, fa0, fa1", 0xffffffff7fa00000,
                     0xffffffff7fa00000, 0x00, zero, fa0, fa1);

   /* ----------------- flt.s rd, rs1, rs2 ------------------ */
   /* 0.0 < 0.0 -> 0 */
   TESTINST_1_2_FCMP(4, "flt.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff00000000, 0x00, a0, fa0, fa1);
   /* 0.0 < 1.0 -> 1 */
   TESTINST_1_2_FCMP(4, "flt.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff3f800000, 0x00, a0, fa0, fa1);
   /* 0.0 < -0.0 -> 0 */
   TESTINST_1_2_FCMP(4, "flt.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff80000000, 0x00, a0, fa0, fa1);
   /* -0.0 < 0.0 -> 0 */
   TESTINST_1_2_FCMP(4, "flt.s a0, fa0, fa1", 0xffffffff80000000,
                     0xffffffff00000000, 0x00, a0, fa0, fa1);
   /* INFINITY < INFINITY -> 0 */
   TESTINST_1_2_FCMP(4, "flt.s a0, fa0, fa1", 0xffffffff7f800000,
                     0xffffffff7f800000, 0x00, a0, fa0, fa1);
   /* 0.0 < qNAN -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "flt.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff7fc00000, 0x00, a0, fa0, fa1);
   /* 0.0 < sNAN -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "flt.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff7fa00000, 0x00, a0, fa0, fa1);

   /* sNAN < sNAN (rd=zero) -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "flt.s zero, fa0, fa1", 0xffffffff7fa00000,
                     0xffffffff7fa00000, 0x00, zero, fa0, fa1);

   /* ----------------- fle.s rd, rs1, rs2 ------------------ */
   /* 1.0 < 0.0 -> 0 */
   TESTINST_1_2_FCMP(4, "fle.s a0, fa0, fa1", 0xffffffff3f800000,
                     0xffffffff00000000, 0x00, a0, fa0, fa1);
   /* 0.0 <= 0.0 -> 1 */
   TESTINST_1_2_FCMP(4, "fle.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff00000000, 0x00, a0, fa0, fa1);
   /* 0.0 <= 1.0 -> 1 */
   TESTINST_1_2_FCMP(4, "fle.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff3f800000, 0x00, a0, fa0, fa1);
   /* 0.0 <= -0.0 -> 1 */
   TESTINST_1_2_FCMP(4, "fle.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff80000000, 0x00, a0, fa0, fa1);
   /* -0.0 <= 0.0 -> 1 */
   TESTINST_1_2_FCMP(4, "fle.s a0, fa0, fa1", 0xffffffff80000000,
                     0xffffffff00000000, 0x00, a0, fa0, fa1);
   /* INFINITY <= INFINITY -> 1 */
   TESTINST_1_2_FCMP(4, "fle.s a0, fa0, fa1", 0xffffffff7f800000,
                     0xffffffff7f800000, 0x00, a0, fa0, fa1);
   /* 0.0 <= qNAN -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "fle.s a0, fa0, fa1", 0xffffffff00000000,
                     0xffffffff7fc00000, 0x00, a0, fa0, fa1);
   /* 0.0 <= sNAN -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "fle.s a0, fa0, fa1", 0xffffffff7fa00000,
                     0x7ff4000000000000, 0x00, a0, fa0, fa1);

   /* sNAN <= sNAN (rd=zero) -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "fle.s zero, fa0, fa1", 0xffffffff7fa00000,
                     0xffffffff7fa00000, 0x00, zero, fa0, fa1);

   /* ------------------ fclass.s rd, rs1 ------------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.s.w rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fcvt.s.wu rd, rs1, rm ---------------- */
   /* TODO Implement. */

   /* ------------------- fmv.w.x rd, rs1 ------------------- */
   TESTINST_1_1_FI(4, "fmv.w.x fa0, a0", 0xabcdef0123456789, 0x00, fa0, a0);

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
