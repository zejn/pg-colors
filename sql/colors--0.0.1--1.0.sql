SET client_min_messages TO warning;
SET log_min_messages    TO warning;

-- Create Delta E functions.
--
\echo Use "ALTER EXTENSION colors UPDATE TO '1.0'" to load this file. \quit


--
-- functions
--

create domain color cube check (cube_dim(VALUE) = 3 and cube_is_point(VALUE));

CREATE FUNCTION g_color_distance (internal, cube, smallint, oid, internal)
RETURNS float8
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION color_distance_cie1976(cube, cube)
RETURNS float8
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION color_distance_cie1994(cube, cube)
RETURNS float8
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR <@> (
    LEFTARG = cube, RIGHTARG = cube, PROCEDURE = color_distance_cie1994,
    COMMUTATOR = '<@>'
);

CREATE FUNCTION color_distance_cie2000(cube, cube)
RETURNS float8
AS 'MODULE_PATHNAME'
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
    OPERATOR    16  <@> (cube, cube) FOR ORDER BY float_ops,
    OPERATOR    17  <~> (cube, cube) FOR ORDER BY float_ops,

    FUNCTION    1   g_cube_consistent (internal, cube, smallint, oid, internal),
    FUNCTION    2   g_cube_union (internal, internal),
    FUNCTION    3   g_cube_compress (internal),
    FUNCTION    4   g_cube_decompress (internal),
    FUNCTION    5   g_cube_penalty (internal, internal, internal),
    FUNCTION    6   g_cube_picksplit (internal, internal),
    FUNCTION    7   g_cube_same (cube, cube, internal),
    FUNCTION    8   g_color_distance (internal, cube, smallint, oid, internal);
