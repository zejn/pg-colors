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
#include "access/gist.h"
#include "access/stratnum.h"
#include "cubedata.h"
#include "fmgr.h"

#include "colors.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


#define CUBE_IS_POINT(cube) (LL_COORD(cube, 0) == UR_COORD(cube, 0) && LL_COORD(cube, 1) == UR_COORD(cube, 1) && LL_COORD(cube, 2) == UR_COORD(cube, 2))

/* KNN strategies */
#define ColorKNNDistanceCIE1976         15      /* <-> */
#define ColorKNNDistanceCIE1994         16      /* <@> */
#define ColorKNNDistanceCIE2000         17      /* <~> */

/* color distance */
Datum       delta_e_cie_1976(PG_FUNCTION_ARGS);
Datum       delta_e_cie_1994(PG_FUNCTION_ARGS);
Datum       delta_e_cmc(PG_FUNCTION_ARGS);
Datum       delta_e_cie_2000(PG_FUNCTION_ARGS);

/* GiST support function for distance */
Datum       g_color_distance(PG_FUNCTION_ARGS);

/* parameters are (cube, cube) */
Datum       color_distance_cie1976(PG_FUNCTION_ARGS);
Datum       color_distance_cie1994(PG_FUNCTION_ARGS);
Datum       color_distance_cie2000(PG_FUNCTION_ARGS);

/* internal function prototypes */
float8 colors_delta_e_cie_1976(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2);
float8 colors_delta_e_cie_1994(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2, float8 K_L, float8 K_C, float8 K_H, float8 K_1, float8 K_2);
float8 colors_delta_e_cmc(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2, float8 pl, float8 pc);
float8 colors_delta_e_cie_2000(float8 l1, float8 a1, float8 b1, float8 l2, float8 a2, float8 b2, float8 Kl, float8 Kc, float8 Kh);
float8 color_cube_distance(NDBOX *query, NDBOX *color, uint16 strategy);
bool cube_contains_v0(NDBOX *a, NDBOX *b);



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

static double
distance_1D(double a1, double a2, double b1, double b2)
{
    double tmp;
    if (a1 > a2) {
        tmp = a1; a1 = a2; a2 = tmp;
    }
    if (b1 > b2) {
        tmp = b1; b1 = b2; b2 = tmp;
    }

    /* interval (a) is entirely on the left of (b) */
    if ((a1 <= b1) && (a2 <= b1) && (a1 <= b2) && (a2 <= b2))
        return (Min(b1, b2) - Max(a1, a2));

    /* interval (a) is entirely on the right of (b) */
    if ((a1 > b1) && (a2 > b1) && (a1 > b2) && (a2 > b2))
        return (Max(b1, b2) - Min(a1, a2));

    /* the rest are all sorts of intersections */
    return (0.0);
}

PG_FUNCTION_INFO_V1(g_color_distance);
Datum g_color_distance(PG_FUNCTION_ARGS)
{
    GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
    NDBOX      *query = PG_GETARG_NDBOX(1);
    StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
    NDBOX      *color = DatumGetNDBOX(entry->key);
    bool       *recheck = (bool *) PG_GETARG_POINTER(4);
    double      retval;

    *recheck = false;

    retval = color_cube_distance(query, color, strategy);
    
    PG_RETURN_FLOAT8(retval);
}

PG_FUNCTION_INFO_V1(color_distance_cie2000);
Datum color_distance_cie2000(PG_FUNCTION_ARGS)
{
    NDBOX      *query = PG_GETARG_NDBOX(0);
    NDBOX      *color = PG_GETARG_NDBOX(1);

    double      retval;

    retval = color_cube_distance(query, color, ColorKNNDistanceCIE2000);
    PG_RETURN_FLOAT8(retval);
}

PG_FUNCTION_INFO_V1(color_distance_cie1976);
Datum color_distance_cie1976(PG_FUNCTION_ARGS)
{
    NDBOX      *query = PG_GETARG_NDBOX(0);
    NDBOX      *color = PG_GETARG_NDBOX(1);
    float8      retval = DBL_MAX; // too big to fail

    retval = color_cube_distance(query, color, ColorKNNDistanceCIE1976);
    PG_RETURN_FLOAT8(retval);
}

PG_FUNCTION_INFO_V1(color_distance_cie1994);
Datum color_distance_cie1994(PG_FUNCTION_ARGS)
{
    NDBOX      *query = PG_GETARG_NDBOX(0);
    NDBOX      *color = PG_GETARG_NDBOX(1);
    float8      retval = DBL_MAX; // too big to fail

    retval = color_cube_distance(query, color, ColorKNNDistanceCIE1994);
    PG_RETURN_FLOAT8(retval);
}

float8 color_cube_distance(NDBOX *query, NDBOX *color, uint16 strategy)
{
    float8      pt[3];
    float8      retval = DBL_MAX; // too big to fail
    bool        zero;

    if (DIM(color) != 3)
        ereport(ERROR,
                (errcode(ERRCODE_ARRAY_ELEMENT_ERROR),
                 errmsg("color should have 3 dimensions, has %d", DIM(color))));
    if (DIM(query) != 3)
        ereport(ERROR,
                (errcode(ERRCODE_ARRAY_ELEMENT_ERROR),
                 errmsg("query should have 3 dimensions, has %d", DIM(query))));
    if (!IS_POINT(query)) 
        ereport(ERROR,
                (errcode(ERRCODE_ARRAY_ELEMENT_ERROR),
                 errmsg("query should be a point")));

    /*
     * TODO: first check if point, then do 
     *  point - point = colors_delta_e_cie_2000
     *  point - cube = if cube contains point then 0 else check distance from closest point by euclidean distance
     *  cube - cube = decided that distance is not defined
     * 
     */

    // check if points
    if (CUBE_IS_POINT(color) && CUBE_IS_POINT(query)) {
        // both are points
        switch (strategy) 
        {
            case ColorKNNDistanceCIE1976:
                retval = colors_delta_e_cie_1976(
                    LL_COORD(color, 0), LL_COORD(color, 1), LL_COORD(color, 2),
                    LL_COORD(query, 0), LL_COORD(query, 1), LL_COORD(query, 2)
                    );
                break;
            case ColorKNNDistanceCIE2000:
                retval = colors_delta_e_cie_2000(
                    LL_COORD(color, 0), LL_COORD(color, 1), LL_COORD(color, 2),
                    LL_COORD(query, 0), LL_COORD(query, 1), LL_COORD(query, 2),
                    1.0, 1.0, 1.0);
                break;
            case ColorKNNDistanceCIE1994:
                retval = colors_delta_e_cie_1994(
                    LL_COORD(color, 0), LL_COORD(color, 1), LL_COORD(color, 2),
                    LL_COORD(query, 0), LL_COORD(query, 1), LL_COORD(query, 2),
                    1.0, 1.0, 1.0, 0.045, 0.015);
                break;
            default:
                elog(ERROR, "unrecognized color strategy number: %d", strategy);
                retval = 0;     /* keep compiler quiet */
                break;
        }

    } else if (!CUBE_IS_POINT(color) && !CUBE_IS_POINT(query)) {
        // when both are cubes
        // the distance measure is not defined, because it is not position invariant
        ereport(ERROR, 
            (errcode(ERRCODE_ARRAY_ELEMENT_ERROR),
             errmsg("delta E is not defined for two cubes")));
    } else {
        // when one is cube, other is point
        NDBOX       *cube;
        NDBOX       *point;
        if (CUBE_IS_POINT(color) && !CUBE_IS_POINT(query)) {
            cube = query;
            point = color;
        } else if (!CUBE_IS_POINT(color) && CUBE_IS_POINT(query)) {
            cube = color;
            point = query;
        } else {
            ereport(ERROR, 
                (errcode(ERRCODE_ARRAY_ELEMENT_ERROR),
                 errmsg("both cubes or both points? cant be!")));
        }

        zero = cube_contains_v0(cube, point);
        if (zero) {
            retval = 0.0;
        } else {

            // find the nearest point by eucliedan distance
            pt[0] = LL_COORD(point, 0) - distance_1D(LL_COORD(point, 0), UR_COORD(point, 0), LL_COORD(cube, 0), UR_COORD(cube, 0));
            pt[1] = LL_COORD(point, 1) - distance_1D(LL_COORD(point, 1), UR_COORD(point, 1), LL_COORD(cube, 1), UR_COORD(cube, 1));
            pt[2] = LL_COORD(point, 2) - distance_1D(LL_COORD(point, 2), UR_COORD(point, 2), LL_COORD(cube, 2), UR_COORD(cube, 2));
            
            switch (strategy) 
            {
                case ColorKNNDistanceCIE1976:
                    retval = colors_delta_e_cie_1976(
                        pt[0], pt[1], pt[2],
                        LL_COORD(point, 0), LL_COORD(point, 1), LL_COORD(point, 2)
                        );
                    break;
                case ColorKNNDistanceCIE2000:
                    retval = colors_delta_e_cie_2000(
                        pt[0], pt[1], pt[2],
                        LL_COORD(point, 0), LL_COORD(point, 1), LL_COORD(point, 2),
                        1.0, 1.0, 1.0);
                    break;
                case ColorKNNDistanceCIE1994:
                    retval = colors_delta_e_cie_1994(
                        pt[0], pt[1], pt[2],
                        LL_COORD(point, 0), LL_COORD(point, 1), LL_COORD(point, 2),
                        1.0, 1.0, 1.0, 0.045, 0.015);
                    break;
                default:
                    elog(ERROR, "unrecognized color strategy number: %d", strategy);
                    retval = 0;     /* keep compiler quiet */
                    break;
            }
        }
    }

    return retval;
}

/* COPIED from contrib/cube/cube.c */
/* Contains */
/* Box(A) CONTAINS Box(B) IFF pt(A) < pt(B) */
bool
cube_contains_v0(NDBOX *a, NDBOX *b)
{
    int         i;

    if ((a == NULL) || (b == NULL))
        return (FALSE);

    if (DIM(a) < DIM(b))
    {
        /*
         * the further comparisons will make sense if the excess dimensions of
         * (b) were zeroes Since both UL and UR coordinates must be zero, we
         * can check them all without worrying about which is which.
         */
        for (i = DIM(a); i < DIM(b); i++)
        {
            if (LL_COORD(b, i) != 0)
                return (FALSE);
            if (UR_COORD(b, i) != 0)
                return (FALSE);
        }
    }

    /* Can't care less about the excess dimensions of (a), if any */
    for (i = 0; i < Min(DIM(a), DIM(b)); i++)
    {
        if (Min(LL_COORD(a, i), UR_COORD(a, i)) >
            Min(LL_COORD(b, i), UR_COORD(b, i)))
            return (FALSE);
        if (Max(LL_COORD(a, i), UR_COORD(a, i)) <
            Max(LL_COORD(b, i), UR_COORD(b, i)))
            return (FALSE);
    }

    return (TRUE);
}
