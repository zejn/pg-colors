/*
 * PostgreSQL color difference
 * Ported from python-colormath by Gasper Zejn
 * 
 *
 * Copyright 2015- This program is Free Software; see the 
 * README.md file for the license conditions.
 */

#include "postgres.h"
#include <math.h>
#include "utils/builtins.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/* color distance */
Datum       delta_e_cie_1976(PG_FUNCTION_ARGS);
Datum       delta_e_cie_1994(PG_FUNCTION_ARGS);
Datum       delta_e_cmc(PG_FUNCTION_ARGS);
Datum       delta_e_cie_2000(PG_FUNCTION_ARGS);

/* function prototypes */
float8 colors_delta_e_cie_1976(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2);
float8 colors_delta_e_cie_1994(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2, float8 K_L, float8 K_C, float8 K_H, float8 K_1, float8 K_2);
float8 colors_delta_e_cmc(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2, float8 pl, float8 pc);
float8 colors_delta_e_cie_2000(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2, float8 Kl, float8 Kc, float8 Kh);


/* input function: 6 double precision */
PG_FUNCTION_INFO_V1(delta_e_cie_1976);
Datum
delta_e_cie_1976(PG_FUNCTION_ARGS)
{
    float8 result;
    float8 l1 = PG_GETARG_FLOAT8(0);
    float8 a1 = PG_GETARG_FLOAT8(1);
    float8 b1 = PG_GETARG_FLOAT8(2);
    float8 l2 = PG_GETARG_FLOAT8(3);
    float8 a2 = PG_GETARG_FLOAT8(4);
    float8 b2 = PG_GETARG_FLOAT8(5);

    result = colors_delta_e_cie_1976(l1, a1, b1, l2, a2, b2);

    PG_RETURN_FLOAT8(result);
}

/* input function: 11 double precision */
PG_FUNCTION_INFO_V1(delta_e_cie_1994_full);
Datum
delta_e_cie_1994_full(PG_FUNCTION_ARGS)
{
    float8 result;

    float8 l1 = PG_GETARG_FLOAT8(0);
    float8 a1 = PG_GETARG_FLOAT8(1);
    float8 b1 = PG_GETARG_FLOAT8(2);
    float8 l2 = PG_GETARG_FLOAT8(3);
    float8 a2 = PG_GETARG_FLOAT8(4);
    float8 b2 = PG_GETARG_FLOAT8(5);
    float8 K_L = PG_GETARG_FLOAT8(6);
    float8 K_C = PG_GETARG_FLOAT8(7);
    float8 K_H = PG_GETARG_FLOAT8(8);
    float8 K_1 = PG_GETARG_FLOAT8(9);
    float8 K_2 = PG_GETARG_FLOAT8(10);

    result = colors_delta_e_cie_1994(l1, a1, b1, l2, a2, b2, K_L, K_C, K_H, K_1, K_2);

    PG_RETURN_FLOAT8(result);
}

/* input function: 6 double precision */
PG_FUNCTION_INFO_V1(delta_e_cie_1994);
Datum
delta_e_cie_1994(PG_FUNCTION_ARGS)
{
    float8 result;
    float8 l1 = PG_GETARG_FLOAT8(0);
    float8 a1 = PG_GETARG_FLOAT8(1);
    float8 b1 = PG_GETARG_FLOAT8(2);
    float8 l2 = PG_GETARG_FLOAT8(3);
    float8 a2 = PG_GETARG_FLOAT8(4);
    float8 b2 = PG_GETARG_FLOAT8(5);

    result = colors_delta_e_cie_1994(l1, a1, b1, l2, a2, b2, 1.0, 1.0, 1.0, 0.045, 0.015);

    PG_RETURN_FLOAT8(result);
}


/* input function: 8 double precision */
PG_FUNCTION_INFO_V1(delta_e_cmc_full);
Datum
delta_e_cmc_full(PG_FUNCTION_ARGS)
{
    float8  result;
    float8 l1 = PG_GETARG_FLOAT8(0);
    float8 a1 = PG_GETARG_FLOAT8(1);
    float8 b1 = PG_GETARG_FLOAT8(2);
    float8 l2 = PG_GETARG_FLOAT8(3);
    float8 a2 = PG_GETARG_FLOAT8(4);
    float8 b2 = PG_GETARG_FLOAT8(5);
    float8 pl = PG_GETARG_FLOAT8(6);
    float8 pc = PG_GETARG_FLOAT8(7);

    result = colors_delta_e_cmc(l1, a1, b1, l2, a2, b2, pl, pc);

    PG_RETURN_FLOAT8(result);

}

/* input function: 6 double precision */
PG_FUNCTION_INFO_V1(delta_e_cmc);
Datum
delta_e_cmc(PG_FUNCTION_ARGS)
{
    float8  result;
    float8 l1 = PG_GETARG_FLOAT8(0);
    float8 a1 = PG_GETARG_FLOAT8(1);
    float8 b1 = PG_GETARG_FLOAT8(2);
    float8 l2 = PG_GETARG_FLOAT8(3);
    float8 a2 = PG_GETARG_FLOAT8(4);
    float8 b2 = PG_GETARG_FLOAT8(5);

    result = colors_delta_e_cmc(l1, a1, b1, l2, a2, b2, 2.0, 1.0);

    PG_RETURN_FLOAT8(result);
}


/* input function: 9 double precision */
PG_FUNCTION_INFO_V1(delta_e_cie_2000_full);
Datum
delta_e_cie_2000_full(PG_FUNCTION_ARGS)
{
    float8  result;
    float8 l1 = PG_GETARG_FLOAT8(0);
    float8 a1 = PG_GETARG_FLOAT8(1);
    float8 b1 = PG_GETARG_FLOAT8(2);
    float8 l2 = PG_GETARG_FLOAT8(3);
    float8 a2 = PG_GETARG_FLOAT8(4);
    float8 b2 = PG_GETARG_FLOAT8(5);
    float8 Kl = PG_GETARG_FLOAT8(6);
    float8 Kc = PG_GETARG_FLOAT8(7);
    float8 Kh = PG_GETARG_FLOAT8(8);

    result = colors_delta_e_cie_2000(l1, a1, b1, l2, a2, b2, Kl, Kc, Kh);

    PG_RETURN_FLOAT8(result);
}

/* input function: 6 double precision */
PG_FUNCTION_INFO_V1(delta_e_cie_2000);
Datum
delta_e_cie_2000(PG_FUNCTION_ARGS)
{
    float8  result;
    float8 l1 = PG_GETARG_FLOAT8(0);
    float8 a1 = PG_GETARG_FLOAT8(1);
    float8 b1 = PG_GETARG_FLOAT8(2);
    float8 l2 = PG_GETARG_FLOAT8(3);
    float8 a2 = PG_GETARG_FLOAT8(4);
    float8 b2 = PG_GETARG_FLOAT8(5);

    result = colors_delta_e_cie_2000(l1, a1, b1, l2, a2, b2, 1.0, 1.0, 1.0);

    PG_RETURN_FLOAT8(result);
}


/* helper functions for more readability */
inline double to_degrees(double radians) {
    return radians * (180.0 / M_PI);
}

inline double to_radians(double degrees) {
    return degrees * (M_PI / 180.0);
}


/*
 * Function implementations
 */

float8 colors_delta_e_cie_1976(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2)
{
    return sqrt(pow(l1 - l2, 2) + pow(a1 - a2, 2) + pow(b1 - b2, 2));
}

float8 colors_delta_e_cie_1994(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2, float8 K_L, float8 K_C, float8 K_H, float8 K_1, float8 K_2)
{
    float8  C_1,
            C_2,
            delta_L,
            delta_C,
            delta_H_sq,
            delta_H,
            S_L,
            S_C,
            S_H;

    C_1 = sqrt(pow(a1, 2) + pow(b1, 2));
    C_2 = sqrt(pow(a2, 2) + pow(b2, 2));

    delta_L = l1 - l2;
    delta_C = C_1 - C_2;

    delta_H_sq = -pow(delta_C, 2) + pow(a1 - a2, 2) + pow(b1 - b2, 2);
    
    delta_H = sqrt((delta_H_sq > 0 ? delta_H_sq : 0));

    S_L = 1;
    S_C = 1 + K_1 * C_1;
    S_H = 1 + K_2 * C_1;

    return sqrt(pow(delta_L / (K_L * S_L), 2) + pow(delta_C / (K_C * S_C), 2) + pow(delta_H / (K_H * S_H), 2));
}

float8 colors_delta_e_cmc(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2, float8 pl, float8 pc)
{
    float8  C_1,
            C_2,
            delta_L,
            delta_C,
            H_1,
            F,
            T,
            S_L,
            S_C,
            S_H,
            delta_H_sq,
            delta_H;

    C_1 = sqrt(pow(a1, 2) + pow(b1, 2));
    C_2 = sqrt(pow(a2, 2) + pow(b2, 2));

    delta_L = l1 - l2;
    delta_C = C_1 - C_2;

    H_1 = to_degrees(atan2(b1, a1));

    H_1 += (H_1 < 0 ? 360 : 0);

    F = sqrt(pow(C_1, 4) / (pow(C_1, 4) + 1900.0));

    if (164 <= H_1 && H_1 <= 345) {
        T = 0.56 + fabs(0.2 * cos(to_radians(H_1 + 168)));
    } else {
        T = 0.36 + fabs(0.4 * cos(to_radians(H_1 + 35)));
    }

    if (l1 < 16) {
        S_L = 0.511;
    } else {
        S_L = (0.040975 * l1) / (1 + 0.01765 * l1);
    }

    S_C = ((0.0638 * C_1) / (1 + 0.0131 * C_1)) + 0.638;
    S_H = S_C * (F * T + 1 - F);

    delta_H_sq = -pow(delta_C, 2) + pow(a1 - a2, 2) + pow(b1 - b2, 2);
    delta_H = sqrt((delta_H_sq > 0 ? delta_H_sq : 0));

    return sqrt(pow(delta_L / (pl * S_L), 2) + pow(delta_C / (pc * S_C), 2) + pow(delta_H / S_H, 2));
}

float8 colors_delta_e_cie_2000(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2, float8 Kl, float8 Kc, float8 Kh)
{
    float8  avg_Lp,
            C1,
            C2,
            avg_C1_C2,
            G,
            a1p,
            a2p,
            C1p,
            C2p,
            avg_C1p_C2p,
            h1p,
            h2p,
            avg_Hp,
            T,
            diff_h2p_h1p,
            delta_hp,
            delta_Lp,
            delta_Cp,
            delta_Hp,
            S_L,
            S_C,
            S_H,
            delta_ro,
            R_C,
            R_T;

    avg_Lp = (l1+l2)/2.0;

    C1 = sqrt(pow(a1, 2.0) + pow(b1, 2.0));
    C2 = sqrt(pow(a2, 2.0) + pow(b2, 2.0));

    avg_C1_C2 = (C1 + C2) / 2.0;

    G = 0.5 * (1 - sqrt(pow(avg_C1_C2, 7.0) / (pow(avg_C1_C2, 7.0) + pow(25.0, 7.0))));

    a1p = (1.0 + G) * a1;
    a2p = (1.0 + G) * a2;

    C1p = sqrt(pow(a1p, 2.0) + pow(b1, 2.0));
    C2p = sqrt(pow(a2p, 2.0) + pow(b2, 2.0));

    avg_C1p_C2p = (C1p + C2p) / 2.0;

    h1p = to_degrees(atan2(b1, a1p));
    h1p += (h1p < 0 ? 360 : 0);
    h2p = to_degrees(atan2(b2, a2p));
    h2p += (h2p < 0 ? 360 : 0);

    avg_Hp = ((fabs(h1p - h2p) > 180 ? 360 : 0) + h1p + h2p) / 2.0;

    T = 1 - 0.17 * cos(to_radians(avg_Hp - 30)) + 
        0.24 * cos(to_radians(2 * avg_Hp)) +
        0.32 * cos(to_radians(3 * avg_Hp + 6)) - 
        0.2 * cos(to_radians(4 * avg_Hp - 63));

    diff_h2p_h1p = h2p - h1p;
    delta_hp = diff_h2p_h1p + (fabs(diff_h2p_h1p) > 180 ? 360 : 0);
    delta_hp -= (h2p > h1p ? 720 : 0);

    delta_Lp = l2 - l1;
    delta_Cp = C2p - C1p;
    delta_Hp = 2 * sqrt(C2p * C1p) * sin(to_radians(delta_hp) / 2.0);

    S_L = 1 + ((0.015 * pow(avg_Lp - 50, 2.0)) / sqrt(20 + pow(avg_Lp - 50, 2.0)));
    S_C = 1 + 0.045 * avg_C1p_C2p;
    S_H = 1 + 0.015 * avg_C1p_C2p * T;

    delta_ro = 30 * exp(-(pow(((avg_Hp - 275) / 25), 2.0)));

    R_C = sqrt((pow(avg_C1p_C2p, 7.0)) / (pow(avg_C1p_C2p, 7.0) + pow(25.0, 7.0)));
    R_T = -2 * R_C * sin(2 * to_radians(delta_ro));

    return sqrt(
        pow(delta_Lp / (S_L * Kl), 2) +
        pow(delta_Cp / (S_C * Kc), 2) +
        pow(delta_Hp / (S_H * Kh), 2) +
        R_T * (delta_Cp / (S_C * Kc)) * (delta_Hp / (S_H * Kh)));
}
