SET client_min_messages TO warning;
SET log_min_messages    TO warning;

-- Create Delta E functions.
--


--
-- functions
--
CREATE OR REPLACE FUNCTION delta_e_cie_1976(double precision, double precision, double precision, double precision, double precision, double precision)
    RETURNS double precision
    AS 'colors', 'delta_e_cie_1976'
    LANGUAGE C STRICT IMMUTABLE;

COMMENT ON FUNCTION delta_e_cie_1976(double precision, double precision, double precision, double precision, double precision, double precision)
IS 'delta_e_cie_1976(L_1, a_1, b_1, L_2, a_2, b_2)';


CREATE OR REPLACE FUNCTION delta_e_cie_1994(double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision)
    RETURNS double precision
    AS 'colors', 'delta_e_cie_1994_full'
    LANGUAGE C STRICT IMMUTABLE;

COMMENT ON FUNCTION delta_e_cie_1994(double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision)
IS 'delta_e_cie_1994(L_1, a_1, b_1, L_2, a_2, b_2, K_L=1, K_C=1, K_H=1, K_1=0.045, K_2=0.015)';


CREATE OR REPLACE FUNCTION delta_e_cie_1994(double precision, double precision, double precision, double precision, double precision, double precision)
    RETURNS double precision
    AS 'colors', 'delta_e_cie_1994'
    LANGUAGE C STRICT IMMUTABLE;

COMMENT ON FUNCTION delta_e_cie_1994(double precision, double precision, double precision, double precision, double precision, double precision)
IS 'delta_e_cie_1994(L_1, a_1, b_1, L_2, a_2, b_2)';


CREATE OR REPLACE FUNCTION delta_e_cmc(double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision)
    RETURNS double precision
    AS 'colors', 'delta_e_cmc_full'
    LANGUAGE C STRICT IMMUTABLE;

COMMENT ON FUNCTION delta_e_cmc(double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision)
IS 'delta_e_cmc(L_1, a_1, b_1, L_2, a_2, b_2, pl=2, pc=1)';


CREATE OR REPLACE FUNCTION delta_e_cmc(double precision, double precision, double precision, double precision, double precision, double precision)
    RETURNS double precision
    AS 'colors', 'delta_e_cmc'
    LANGUAGE C STRICT IMMUTABLE;

COMMENT ON FUNCTION delta_e_cmc(double precision, double precision, double precision, double precision, double precision, double precision)
IS 'delta_e_cmc(L_1, a_1, b_1, L_2, a_2, b_2, pl=2, pc=1)';


CREATE OR REPLACE FUNCTION delta_e_cie_2000(double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision)
    RETURNS double precision
    AS 'colors', 'delta_e_cie_2000_full'
    LANGUAGE C STRICT IMMUTABLE;

COMMENT ON FUNCTION delta_e_cie_2000(double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision)
IS 'delta_e_cie_2000(L_1, a_1, b_1, L_2, a_2, b_2, Kl=1, Kc=1, Kh=1)';

CREATE OR REPLACE FUNCTION delta_e_cie_2000(double precision, double precision, double precision, double precision, double precision, double precision)
    RETURNS double precision
    AS 'colors', 'delta_e_cie_2000'
    LANGUAGE C STRICT IMMUTABLE;

COMMENT ON FUNCTION delta_e_cie_2000(double precision, double precision, double precision, double precision, double precision, double precision)
IS 'delta_e_cie_2000(L_1, a_1, b_1, L_2, a_2, b_2, Kl=1, Kc=1, Kh=1)';

create domain color cube check (cube_dim(VALUE) = 3 and cube_is_point(VALUE));

CREATE FUNCTION g_color_distance (internal, cube, smallint, oid, internal)
RETURNS float8
AS 'colors', 'g_color_distance'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION color_distance_cie1976(cube, cube)
RETURNS float8
AS 'colors', 'color_distance_cie1976'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION color_distance_cie2000(cube, cube)
RETURNS float8
AS 'colors', 'color_distance_cie2000'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR <~> (
    LEFTARG = cube, RIGHTARG = cube, PROCEDURE = color_distance_cie2000,
    COMMUTATOR = '<~>'
);

CREATE OPERATOR CLASS gist_color_ops
    FOR TYPE cube USING gist AS
    OPERATOR    3   && ,
    OPERATOR    6   = ,
    OPERATOR    7   @> ,
    OPERATOR    8   <@ ,
    OPERATOR    13  @ ,
    OPERATOR    14  ~ ,
    OPERATOR    15  <-> (cube, cube) FOR ORDER BY float_ops,
    OPERATOR    16  <~> (cube, cube) FOR ORDER BY float_ops,

    FUNCTION    1   g_cube_consistent (internal, cube, integer, oid, internal),
    FUNCTION    2   g_cube_union (internal, internal),
    FUNCTION    3   g_cube_compress (internal),
    FUNCTION    4   g_cube_decompress (internal),
    FUNCTION    5   g_cube_penalty (internal, internal, internal),
    FUNCTION    6   g_cube_picksplit (internal, internal),
    FUNCTION    7   g_cube_same (cube, cube, internal),
    FUNCTION    8   g_color_distance (internal, cube, smallint, oid, internal);
