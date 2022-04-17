/* Tests for the RV64D standard double-precision floating-point instruction-set
   extension. */

#include "testinst.h"

static void test_float64_shared(void)
{
   printf("RV64D double-precision FP instruction set, shared operations\n");

   /* --------------- fld rd, imm[11:0](rs1) ---------------- */
   TESTINST_1_1_FLOAD(4, "fld fa0, 0(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, 4(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, 8(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, 16(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, 32(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, 64(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, 128(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, 256(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, 512(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, 1024(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, 2040(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, -4(a1)", fa0, a1);
   TESTINST_1_1_FLOAD(4, "fld fa0, -2048(a1)", fa0, a1);

   TESTINST_1_1_FLOAD(4, "fld fa4, 0(a5)", fa4, a5);

   /* --------------- fsd rs2, imm[11:0](rs1) --------------- */
   TESTINST_0_2_FSTORE(4, "fsd fa0, 0(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, 4(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, 8(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, 16(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, 32(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, 64(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, 128(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, 256(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, 512(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, 1024(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, 2040(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, -4(a1)", 0xabcdef0123456789, fa0, a1);
   TESTINST_0_2_FSTORE(4, "fsd fa0, -2048(a1)", 0xabcdef0123456789, fa0, a1);

   TESTINST_0_2_FSTORE(4, "fsd fa4, 0(a5)", 0xabcdef0123456789, fa4, a5);

   /* ------------ fmadd.d rd, rs1, rs2, rs3, rm ------------ */
   /* 3.0 * 2.0 + 1.0 -> 7.0 */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x4008000000000000,
                  0x4000000000000000, 0x3ff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + -1.0 -> 0.0 */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0xbff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * DBL_TRUE_MIN + -DBL_TRUE_MIN -> DBL_TRUE_MIN (no UF because exact)
    */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x0000000000000001, 0x8000000000000001, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * DBL_MAX + -DBL_MAX -> DBL_MAX */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7fefffffffffffff, 0xffefffffffffffff, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * DBL_MAX + 0.0 -> INFINITY (OF, NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7fefffffffffffff, 0x0000000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * INFINITY + -INFINITY -> qNAN (NV) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7ff0000000000000, 0xfff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* 1.0 * 1.0 + DBL_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3, rne", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafter(1.0) + DBL_EPSILON/2 (RNE) -> 2nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3, rne", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + DBL_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3, rtz", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -DBL_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3, rtz", 0x3ff0000000000000,
                  0xbff0000000000000, 0xbca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + DBL_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3, rdn", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -DBL_EPSILON/2 (RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3, rdn", 0x3ff0000000000000,
                  0xbff0000000000000, 0xbca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + DBL_EPSILON/2 (RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3, rup", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -DBL_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3, rup", 0x3ff0000000000000,
                  0xbff0000000000000, 0xbca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + DBL_EPSILON/2 (RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3, rmm", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -DBL_EPSILON/2 (RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3, rmm", 0x3ff0000000000000,
                  0xbff0000000000000, 0xbca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* 1.0 * 1.0 + DBL_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafter(1.0) + DBL_EPSILON/2 (DYN-RNE) -> 2nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + DBL_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -DBL_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0xbff0000000000000, 0xbca0000000000000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + DBL_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -DBL_EPSILON/2 (DYN-RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0xbff0000000000000, 0xbca0000000000000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + DBL_EPSILON/2 (DYN-RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -DBL_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0xbff0000000000000, 0xbca0000000000000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 + DBL_EPSILON/2 (DYN-RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x80, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 + -DBL_EPSILON/2 (DYN-RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0xbff0000000000000, 0xbca0000000000000, 0x80, fa0, fa1, fa2,
                  fa3);

   /* ------------ fmsub.d rd, rs1, rs2, rs3, rm ------------ */
   /* 3.0 * 2.0 - 1.0 -> 5.0 */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x4008000000000000,
                  0x4000000000000000, 0x3ff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 1.0 - 1.0 -> 0.0 */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * DBL_TRUE_MIN - DBL_TRUE_MIN -> DBL_TRUE_MIN (no UF because exact)
    */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x0000000000000001, 0x0000000000000001, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * DBL_MAX - DBL_MAX -> DBL_MAX */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7fefffffffffffff, 0x7fefffffffffffff, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * DBL_MAX - 0.0 -> INFINITY (OF, NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7fefffffffffffff, 0x0000000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 2.0 * INFINITY - INFINITY -> qNAN (NV) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7ff0000000000000, 0x7ff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* 1.0 * nextafter(1.0) - DBL_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3, rne", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 2nextafter(1.0) - DBL_EPSILON/2 (RNE) -> 2nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3, rne", 0x3ff0000000000000,
                  0x3ff0000000000002, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafter(1.0) - DBL_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3, rtz", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - DBL_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3, rtz", 0x3ff0000000000000,
                  0xbff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafter(1.0) - DBL_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3, rdn", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - DBL_EPSILON/2 (RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3, rdn", 0x3ff0000000000000,
                  0xbff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafter(1.0) - DBL_EPSILON/2 (RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3, rup", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - DBL_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3, rup", 0x3ff0000000000000,
                  0xbff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafter(1.0) - DBL_EPSILON/2 (RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3, rmm", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - DBL_EPSILON/2 (RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3, rmm", 0x3ff0000000000000,
                  0xbff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* 1.0 * nextafter(1.0) - DBL_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * 2nextafter(1.0) - DBL_EPSILON/2 (DYN-RNE) -> 2nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000002, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafter(1.0) - DBL_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - DBL_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0xbff0000000000000, 0x3ca0000000000000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafter(1.0) - DBL_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - DBL_EPSILON/2 (DYN-RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0xbff0000000000000, 0x3ca0000000000000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafter(1.0) - DBL_EPSILON/2 (DYN-RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - DBL_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0xbff0000000000000, 0x3ca0000000000000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * nextafter(1.0) - DBL_EPSILON/2 (DYN-RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x80, fa0, fa1, fa2,
                  fa3);
   /* 1.0 * -1.0 - DBL_EPSILON/2 (DYN-RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0xbff0000000000000, 0x3ca0000000000000, 0x80, fa0, fa1, fa2,
                  fa3);

   /* ----------- fnmsub.d rd, rs1, rs2, rs3, rm ------------ */
   /* -(3.0 * 2.0) + 1.0 -> -5.0 */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0x4008000000000000,
                  0x4000000000000000, 0x3ff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + 1.0 -> 0.0 */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * DBL_TRUE_MIN) + DBL_TRUE_MIN -> -DBL_TRUE_MIN (no UF because
      exact) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x0000000000000001, 0x0000000000000001, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * DBL_MAX) + DBL_MAX -> -DBL_MAX */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7fefffffffffffff, 0x7fefffffffffffff, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * DBL_MAX) + 0.0 -> -INFINITY (OF, NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7fefffffffffffff, 0x0000000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * INFINITY) + INFINITY -> qNAN (NV) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7ff0000000000000, 0x7ff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* -(-1.0 * 1.0) + DBL_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3, rne", 0xbff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafter(1.0)) + DBL_EPSILON/2 (RNE) -> 2nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3, rne", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + DBL_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3, rtz", 0xbff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -DBL_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3, rtz", 0x3ff0000000000000,
                  0x3ff0000000000000, 0xbca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + DBL_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3, rdn", 0xbff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -DBL_EPSILON/2 (RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3, rdn", 0x3ff0000000000000,
                  0x3ff0000000000000, 0xbca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + DBL_EPSILON/2 (RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3, rup", 0xbff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -DBL_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3, rup", 0x3ff0000000000000,
                  0x3ff0000000000000, 0xbca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + DBL_EPSILON/2 (RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3, rmm", 0xbff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -DBL_EPSILON/2 (RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3, rmm", 0x3ff0000000000000,
                  0x3ff0000000000000, 0xbca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* -(-1.0 * 1.0) + DBL_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafter(1.0)) + DBL_EPSILON/2 (DYN-RNE) -> 2nextafter(1.0) (NX)
    */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + DBL_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -DBL_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0xbca0000000000000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + DBL_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -DBL_EPSILON/2 (DYN-RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0xbca0000000000000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + DBL_EPSILON/2 (DYN-RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -DBL_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0xbca0000000000000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 1.0) + DBL_EPSILON/2 (DYN-RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x80, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) + -DBL_EPSILON/2 (DYN-RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmsub.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0xbca0000000000000, 0x80, fa0, fa1, fa2,
                  fa3);

   /* ----------- fnmadd.d rd, rs1, rs2, rs3, rm ------------ */
   /* -(3.0 * 2.0) - 1.0 -> -7.0 */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0x4008000000000000,
                  0x4000000000000000, 0x3ff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - -1.0 -> 0.0 */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0xbff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * DBL_TRUE_MIN) - -DBL_TRUE_MIN -> -DBL_TRUE_MIN (no UF because
      exact) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x0000000000000001, 0x8000000000000001, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * DBL_MAX) - -DBL_MAX -> -DBL_MAX */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7fefffffffffffff, 0xffefffffffffffff, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * DBL_MAX) - 0.0 -> -INFINITY (OF, NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7fefffffffffffff, 0x0000000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(2.0 * INFINITY) - -INFINITY -> qNAN (NV) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0x4000000000000000,
                  0x7ff0000000000000, 0xfff0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* -(-1.0 * nextafter(1.0)) - DBL_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3, rne", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 2nextafter(1.0)) - DBL_EPSILON/2 (RNE) -> 2nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3, rne", 0xbff0000000000000,
                  0x3ff0000000000002, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafter(1.0)) - DBL_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3, rtz", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - DBL_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3, rtz", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafter(1.0)) - DBL_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3, rdn", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - DBL_EPSILON/2 (RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3, rdn", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafter(1.0)) - DBL_EPSILON/2 (RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3, rup", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - DBL_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3, rup", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafter(1.0)) - DBL_EPSILON/2 (RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3, rmm", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - DBL_EPSILON/2 (RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3, rmm", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);

   /* -(-1.0 * nextafter(1.0)) - DBL_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * 2nextafter(1.0)) - DBL_EPSILON/2 (DYN-RNE) -> 2nextafter(1.0)
      (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000002, 0x3ca0000000000000, 0x00, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafter(1.0)) - DBL_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - DBL_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x20, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafter(1.0)) - DBL_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - DBL_EPSILON/2 (DYN-RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x40, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafter(1.0)) - DBL_EPSILON/2 (DYN-RUP) -> nextafter(1.0) (NX)
    */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - DBL_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x60, fa0, fa1, fa2,
                  fa3);
   /* -(-1.0 * nextafter(1.0)) - DBL_EPSILON/2 (DYN-RMM) -> nextafter(1.0) (NX)
    */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0xbff0000000000000,
                  0x3ff0000000000001, 0x3ca0000000000000, 0x80, fa0, fa1, fa2,
                  fa3);
   /* -(1.0 * 1.0) - DBL_EPSILON/2 (DYN-RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_3_F(4, "fnmadd.d fa0, fa1, fa2, fa3", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x3ca0000000000000, 0x80, fa0, fa1, fa2,
                  fa3);

   /* --------------- fadd.d rd, rs1, rs2, rm --------------- */
   /* 2.0 + 1.0 -> 3.0 */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x4000000000000000,
                  0x3ff0000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 + -1.0 -> 0.0 */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0xbff0000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN + DBL_TRUE_MIN -> 2*DBL_TRUE_MIN (no UF because exact) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x0000000000000001,
                  0x0000000000000001, 0x00, fa0, fa1, fa2);
   /* DBL_MAX + DBL_MAX -> INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x7fefffffffffffff,
                  0x7fefffffffffffff, 0x00, fa0, fa1, fa2);
   /* -DBL_MAX + -DBL_MAX -> -INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0xffefffffffffffff,
                  0xffefffffffffffff, 0x00, fa0, fa1, fa2);
   /* nextafter(DBL_MIN) + -DBL_MIN -> DBL_TRUE_MIN (no UF because exact) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x0010000000000001,
                  0x8010000000000000, 0x00, fa0, fa1, fa2);
   /* INFINITY + -INFINITY -> qNAN (NV) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x7ff0000000000000,
                  0xfff0000000000000, 0x00, fa0, fa1, fa2);

   /* 1.0 + DBL_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2, rne", 0x3ff0000000000000,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* nextafter(1.0) + DBL_EPSILON/2 (RNE) -> 2nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2, rne", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 + DBL_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2, rtz", 0x3ff0000000000000,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* -1.0 + -DBL_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2, rtz", 0xbff0000000000000,
                  0xbca0000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 + DBL_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2, rdn", 0x3ff0000000000000,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* -1.0 + -DBL_EPSILON/2 (RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2, rdn", 0xbff0000000000000,
                  0xbca0000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 + DBL_EPSILON/2 (RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2, rup", 0x3ff0000000000000,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* -1.0 + -DBL_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2, rup", 0xbff0000000000000,
                  0xbca0000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 + DBL_EPSILON/2 (RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2, rmm", 0x3ff0000000000000,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* -1.0 + -DBL_EPSILON/2 (RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2, rmm", 0xbff0000000000000,
                  0xbca0000000000000, 0x00, fa0, fa1, fa2);

   /* 1.0 + DBL_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* nextafter(1.0) + DBL_EPSILON/2 (DYN-RNE) -> 2nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 + DBL_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0x3ca0000000000000, 0x20, fa0, fa1, fa2);
   /* -1.0 + -DBL_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0xbff0000000000000,
                  0xbca0000000000000, 0x20, fa0, fa1, fa2);
   /* 1.0 + DBL_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0x3ca0000000000000, 0x40, fa0, fa1, fa2);
   /* -1.0 + -DBL_EPSILON/2 (DYN-RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0xbff0000000000000,
                  0xbca0000000000000, 0x40, fa0, fa1, fa2);
   /* 1.0 + DBL_EPSILON/2 (DYN-RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0x3ca0000000000000, 0x60, fa0, fa1, fa2);
   /* -1.0 + -DBL_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0xbff0000000000000,
                  0xbca0000000000000, 0x60, fa0, fa1, fa2);
   /* 1.0 + DBL_EPSILON/2 (DYN-RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0x3ca0000000000000, 0x80, fa0, fa1, fa2);
   /* -1.0 + -DBL_EPSILON/2 (DYN-RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fadd.d fa0, fa1, fa2", 0xbff0000000000000,
                  0xbca0000000000000, 0x80, fa0, fa1, fa2);

   /* --------------- fsub.d rd, rs1, rs2, rm --------------- */
   /* 2.0 - 1.0 -> 1.0 */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x4000000000000000,
                  0x3ff0000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 - 1.0 -> 0.0 */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN - -DBL_TRUE_MIN -> 2*DBL_TRUE_MIN (no UF because exact) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x0000000000000001,
                  0x8000000000000001, 0x00, fa0, fa1, fa2);
   /* DBL_MAX - -DBL_MAX -> INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x7fefffffffffffff,
                  0xffefffffffffffff, 0x00, fa0, fa1, fa2);
   /* -DBL_MAX - DBL_MAX -> -INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0xffefffffffffffff,
                  0x7fefffffffffffff, 0x00, fa0, fa1, fa2);
   /* nextafter(DBL_MIN) - DBL_MIN -> DBL_TRUE_MIN (no UF because exact) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x0010000000000001,
                  0x0010000000000000, 0x00, fa0, fa1, fa2);
   /* INFINITY - INFINITY -> qNAN (NV) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x7ff0000000000000,
                  0x7ff0000000000000, 0x00, fa0, fa1, fa2);

   /* nextafter(1.0) - DBL_EPSILON/2 (RNE) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2, rne", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* 2nextafter(1.0) - DBL_EPSILON/2 (RNE) -> 2nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2, rne", 0x3ff0000000000002,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* nextafter(1.0) - DBL_EPSILON/2 (RTZ) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2, rtz", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* -1.0 - DBL_EPSILON/2 (RTZ) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2, rtz", 0xbff0000000000000,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* nextafter(1.0) - DBL_EPSILON/2 (RDN) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2, rdn", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* -1.0 - DBL_EPSILON/2 (RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2, rdn", 0xbff0000000000000,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* nextafter(1.0) - DBL_EPSILON/2 (RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2, rup", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* -1.0 - DBL_EPSILON/2 (RUP) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2, rup", 0xbff0000000000000,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* nextafter(1.0) - DBL_EPSILON/2 (RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2, rmm", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* -1.0 - DBL_EPSILON/2 (RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2, rmm", 0xbff0000000000000,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);

   /* nextafter(1.0) - DBL_EPSILON/2 (DYN-RNE) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* 2nextafter(1.0) - DBL_EPSILON/2 (DYN-RNE) -> 2nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x3ff0000000000002,
                  0x3ca0000000000000, 0x00, fa0, fa1, fa2);
   /* nextafter(1.0) - DBL_EPSILON/2 (DYN-RTZ) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x20, fa0, fa1, fa2);
   /* -1.0 - DBL_EPSILON/2 (DYN-RTZ) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0xbff0000000000000,
                  0x3ca0000000000000, 0x20, fa0, fa1, fa2);
   /* nextafter(1.0) - DBL_EPSILON/2 (DYN-RDN) -> 1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x40, fa0, fa1, fa2);
   /* -1.0 - DBL_EPSILON/2 (DYN-RDN) -> -nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0xbff0000000000000,
                  0x3ca0000000000000, 0x40, fa0, fa1, fa2);
   /* nextafter(1.0) - DBL_EPSILON/2 (DYN-RUP) -> nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x60, fa0, fa1, fa2);
   /* -1.0 - DBL_EPSILON/2 (DYN-RUP) -> -1.0 (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0xbff0000000000000,
                  0x3ca0000000000000, 0x60, fa0, fa1, fa2);
   /* nextafter(1.0) - DBL_EPSILON/2 (DYN-RMM) -> nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0x3ff0000000000001,
                  0x3ca0000000000000, 0x80, fa0, fa1, fa2);
   /* -1.0 - DBL_EPSILON/2 (DYN-RMM) -> -nextafter(1.0) (NX) */
   TESTINST_1_2_F(4, "fsub.d fa0, fa1, fa2", 0xbff0000000000000,
                  0x3ca0000000000000, 0x80, fa0, fa1, fa2);

   /* --------------- fmul.d rd, rs1, rs2, rm --------------- */
   /* 2.0 * 1.0 -> 2.0 */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x4000000000000000,
                  0x3ff0000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 * 0.0 -> 0.0 */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x4000000000000000,
                  0x0000000000000000, 0x00, fa0, fa1, fa2);
   /* 2**-537 * 2**-537 -> 2**-1074 aka DBL_TRUE_MIN (no UF because exact) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x1e60000000000000,
                  0x1e60000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_MAX * DBL_MAX -> INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x7fefffffffffffff,
                  0x7fefffffffffffff, 0x00, fa0, fa1, fa2);
   /* DBL_MAX * -DBL_MAX -> -INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x7fefffffffffffff,
                  0xffefffffffffffff, 0x00, fa0, fa1, fa2);
   /* 1.0 * INFINITY -> INFINITY */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0x7ff0000000000000, 0x00, fa0, fa1, fa2);
   /* 0.0 * INFINITY -> qNAN (NV) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x0000000000000000,
                  0x7ff0000000000000, 0x00, fa0, fa1, fa2);

   /* DBL_TRUE_MIN * 2**-1 (RNE) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2, rne", 0x0000000000000001,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* 3*DBL_TRUE_MIN * 2**-1 (RNE) -> 2*DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2, rne", 0x0000000000000003,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN * 2**-1 (RTZ) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2, rtz", 0x0000000000000001,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN * 2**-1 (RTZ) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2, rtz", 0x8000000000000001,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN * 2**-1 (RND) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2, rdn", 0x0000000000000001,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN * 2**-1 (RND) -> -DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2, rdn", 0x8000000000000001,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN * 2**-1 (RUP) -> DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2, rup", 0x0000000000000001,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN * 2**-1 (RUP) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2, rup", 0x8000000000000001,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN * 2**-1 (RMM) -> DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2, rmm", 0x0000000000000001,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN * 2**-1 (RMM) -> -DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2, rmm", 0x8000000000000001,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);

   /* DBL_TRUE_MIN * 2**-1 (DYN-RNE) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x0000000000000001,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* 3*DBL_TRUE_MIN * 2**-1 (DYN-RNE) -> 2*DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x0000000000000003,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN * 2**-1 (DYN-RTZ) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x0000000000000001,
                  0x3fe0000000000000, 0x20, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN * 2**-1 (DYN-RTZ) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x8000000000000001,
                  0x3fe0000000000000, 0x20, fa0, fa1, fa2);
   /* DBL_TRUE_MIN * 2**-1 (DYN-RND) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x0000000000000001,
                  0x3fe0000000000000, 0x40, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN * 2**-1 (DYN-RND) -> -DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x8000000000000001,
                  0x3fe0000000000000, 0x40, fa0, fa1, fa2);
   /* DBL_TRUE_MIN * 2**-1 (DYN-RUP) -> DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x0000000000000001,
                  0x3fe0000000000000, 0x60, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN * 2**-1 (DYN-RUP) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x8000000000000001,
                  0x3fe0000000000000, 0x60, fa0, fa1, fa2);
   /* DBL_TRUE_MIN * 2**-1 (DYN-RMM) -> DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x0000000000000001,
                  0x3fe0000000000000, 0x80, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN * 2**-1 (DYN-RMM) -> -DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fmul.d fa0, fa1, fa2", 0x8000000000000001,
                  0x3fe0000000000000, 0x80, fa0, fa1, fa2);

   /* --------------- fdiv.d rd, rs1, rs2, rm --------------- */
   /* 2.0 / 1.0 -> 2.0 */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x4000000000000000,
                  0x3ff0000000000000, 0x00, fa0, fa1, fa2);
   /* 0.0 / 1.0 -> 0.0 */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x0000000000000000,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 / 2**1023 -> 1**-1023 (no UF because exact) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0x7fe0000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_MAX / 2**-1 -> INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x7fefffffffffffff,
                  0x3fe0000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_MAX / -2**-1 -> -INFINITY (OF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x7fefffffffffffff,
                  0xbfe0000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 / INFINITY -> 0.0 */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0x7ff0000000000000, 0x00, fa0, fa1, fa2);
   /* 1.0 / 0.0 -> INFINITY (DZ) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x3ff0000000000000,
                  0x0000000000000000, 0x00, fa0, fa1, fa2);
   /* 0.0 / 0.0 -> qNAN (NV) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x0000000000000000,
                  0x0000000000000000, 0x00, fa0, fa1, fa2);

   /* DBL_TRUE_MIN / 2.0 (RNE) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2, rne", 0x0000000000000001,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* 3*DBL_TRUE_MIN / 2.0 (RNE) -> 2*DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2, rne", 0x0000000000000003,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN / 2.0 (RTZ) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2, rtz", 0x0000000000000001,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN / 2.0 (RTZ) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2, rtz", 0x8000000000000001,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN / 2.0 (RND) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2, rdn", 0x0000000000000001,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN / 2.0 (RND) -> -DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2, rdn", 0x8000000000000001,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN / 2.0 (RUP) -> DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2, rup", 0x0000000000000001,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN / 2.0 (RUP) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2, rup", 0x8000000000000001,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN / 2.0 (RMM) -> DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2, rmm", 0x0000000000000001,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN / 2.0 (RMM) -> -DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2, rmm", 0x8000000000000001,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);

   /* DBL_TRUE_MIN / 2.0 (DYN-RNE) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x0000000000000001,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* 3*DBL_TRUE_MIN / 2.0 (DYN-RNE) -> 2*DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x0000000000000003,
                  0x4000000000000000, 0x00, fa0, fa1, fa2);
   /* DBL_TRUE_MIN / 2.0 (DYN-RTZ) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x0000000000000001,
                  0x4000000000000000, 0x20, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN / 2.0 (DYN-RTZ) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x8000000000000001,
                  0x4000000000000000, 0x20, fa0, fa1, fa2);
   /* DBL_TRUE_MIN / 2.0 (DYN-RND) -> 0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x0000000000000001,
                  0x4000000000000000, 0x40, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN / 2.0 (DYN-RND) -> -DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x8000000000000001,
                  0x4000000000000000, 0x40, fa0, fa1, fa2);
   /* DBL_TRUE_MIN / 2.0 (DYN-RUP) -> DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x0000000000000001,
                  0x4000000000000000, 0x60, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN / 2.0 (DYN-RUP) -> -0.0 (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x8000000000000001,
                  0x4000000000000000, 0x60, fa0, fa1, fa2);
   /* DBL_TRUE_MIN / 2.0 (DYN-RMM) -> DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x0000000000000001,
                  0x4000000000000000, 0x80, fa0, fa1, fa2);
   /* -DBL_TRUE_MIN / 2.0 (DYN-RMM) -> -DBL_TRUE_MIN (UF, NX) */
   TESTINST_1_2_F(4, "fdiv.d fa0, fa1, fa2", 0x8000000000000001,
                  0x4000000000000000, 0x80, fa0, fa1, fa2);

   /* ----------------- fsqrt.d rd, rs1, rm ----------------- */
   /* TODO Implement. */

   /* ---------------- fsgnj.d rd, rs1, rs2 ----------------- */
   /* fmv.d fa0, fa1 */
   TESTINST_1_2_F(4, "fsgnj.d fa0, fa1, fa1", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x00, fa0, fa1, fa1);
   TESTINST_1_2_F(4, "fsgnj.d fa0, fa1, fa1", 0xbff0000000000000,
                  0xbff0000000000000, 0x00, fa0, fa1, fa1);
   /* TODO Implement. */

   /* ---------------- fsgnjn.d rd, rs1, rs2 ---------------- */
   /* fneg.d fa0, fa1 */
   TESTINST_1_2_F(4, "fsgnjn.d fa0, fa1, fa1", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x00, fa0, fa1, fa1);
   TESTINST_1_2_F(4, "fsgnjn.d fa0, fa1, fa1", 0xbff0000000000000,
                  0xbff0000000000000, 0x00, fa0, fa1, fa1);
   /* TODO Implement. */

   /* ---------------- fsgnjx.d rd, rs1, rs2 ---------------- */
   /* fabs.d fa0, fa1 */
   TESTINST_1_2_F(4, "fsgnjx.d fa0, fa1, fa1", 0x3ff0000000000000,
                  0x3ff0000000000000, 0x00, fa0, fa1, fa1);
   TESTINST_1_2_F(4, "fsgnjx.d fa0, fa1, fa1", 0xbff0000000000000,
                  0xbff0000000000000, 0x00, fa0, fa1, fa1);
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
   /* 0.0 == 1.0 -> 0 */
   TESTINST_1_2_FCMP(4, "feq.d a0, fa0, fa1", 0x0000000000000000,
                     0x3ff0000000000000, 0x00, a0, fa0, fa1);
   /* 0.0 == 0.0 -> 1 */
   TESTINST_1_2_FCMP(4, "feq.d a0, fa0, fa1", 0x0000000000000000,
                     0x0000000000000000, 0x00, a0, fa0, fa1);
   /* INFINITY == INFINITY -> 1 */
   TESTINST_1_2_FCMP(4, "feq.d a0, fa0, fa1", 0x7ff0000000000000,
                     0x7ff0000000000000, 0x00, a0, fa0, fa1);
   /* 0.0 == qNAN -> 0 */
   TESTINST_1_2_FCMP(4, "feq.d a0, fa0, fa1", 0x0000000000000000,
                     0x7ff8000000000000, 0x00, a0, fa0, fa1);
   /* TODO Implement. */
#if 0
   /* 0.0 == sNAN -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "feq.d a0, fa0, fa1", 0x0000000000000000,
                     0x7ff4000000000000, 0x00, a0, fa0, fa1);
#endif

   /* ----------------- flt.d rd, rs1, rs2 ------------------ */
   /* 0.0 < 0.0 -> 0 */
   TESTINST_1_2_FCMP(4, "flt.d a0, fa0, fa1", 0x0000000000000000,
                     0x0000000000000000, 0x00, a0, fa0, fa1);
   /* 0.0 < 1.0 -> 1 */
   TESTINST_1_2_FCMP(4, "flt.d a0, fa0, fa1", 0x0000000000000000,
                     0x3ff0000000000000, 0x00, a0, fa0, fa1);
   /* INFINITY < INFINITY -> 0 */
   TESTINST_1_2_FCMP(4, "flt.d a0, fa0, fa1", 0x7ff0000000000000,
                     0x7ff0000000000000, 0x00, a0, fa0, fa1);
   /* TODO Implement. */
#if 0
   /* 0.0 < qNAN -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "flt.d a0, fa0, fa1", 0x0000000000000000,
                     0x7ff8000000000000, 0x00, a0, fa0, fa1);
   /* 0.0 < sNAN -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "flt.d a0, fa0, fa1", 0x0000000000000000,
                     0x7ff4000000000000, 0x00, a0, fa0, fa1);
#endif

   /* ----------------- fle.d rd, rs1, rs2 ------------------ */
   /* 1.0 < 0.0 -> 0 */
   TESTINST_1_2_FCMP(4, "fle.d a0, fa0, fa1", 0x3ff0000000000000,
                     0x0000000000000000, 0x00, a0, fa0, fa1);
   /* 0.0 <= 0.0 -> 1 */
   TESTINST_1_2_FCMP(4, "fle.d a0, fa0, fa1", 0x0000000000000000,
                     0x0000000000000000, 0x00, a0, fa0, fa1);
   /* 0.0 <= 1.0 -> 1 */
   TESTINST_1_2_FCMP(4, "fle.d a0, fa0, fa1", 0x0000000000000000,
                     0x3ff0000000000000, 0x00, a0, fa0, fa1);
   /* INFINITY <= INFINITY -> 1 */
   TESTINST_1_2_FCMP(4, "fle.d a0, fa0, fa1", 0x7ff0000000000000,
                     0x7ff0000000000000, 0x00, a0, fa0, fa1);
   /* TODO Implement. */
#if 0
   /* 0.0 <= qNAN -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "fle.d a0, fa0, fa1", 0x0000000000000000,
                     0x7ff8000000000000, 0x00, a0, fa0, fa1);
   /* 0.0 <= sNAN -> 0 (NV) */
   TESTINST_1_2_FCMP(4, "fle.d a0, fa0, fa1", 0x0000000000000000,
                     0x7ff4000000000000, 0x00, a0, fa0, fa1);
#endif

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
