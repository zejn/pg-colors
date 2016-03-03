\set ECHO none
BEGIN;

\i test/pgtap-core.sql

CREATE EXTENSION IF NOT EXISTS cube;

\i sql/colors--1.0.sql

-- Before 8.4, there was no unnest(), so create one.
CREATE FUNCTION create_unnest(
) RETURNS SETOF BOOLEAN LANGUAGE PLPGSQL AS $$
BEGIN
    IF pg_version_num() < 80400 THEN
        EXECUTE $F$ CREATE FUNCTION unnest(
            anyarray
        ) RETURNS SETOF anyelement LANGUAGE sql AS $_$
            SELECT $1[i]
              FROM generate_series(array_lower($1, 1), array_upper($1, 1)) AS i;
        $_$;$F$;
    END IF;
END;
$$;

SELECT * FROM create_unnest();

SELECT plan(23);
-- SELECT * FROM no_plan();

SELECT has_function('delta_e_cie_1976', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cie_1994', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cie_1994', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cmc', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cmc', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cie_2000', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cie_2000', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);

SELECT has_function('g_color_distance', ARRAY['internal', 'cube', 'smallint', 'oid', 'internal']);
SELECT has_function('color_distance_cie1976', ARRAY['cube', 'cube']);
SELECT has_function('color_distance_cie2000', ARRAY['cube', 'cube']);

SELECT has_opclass('gist_color_ops');

SELECT has_domain('color');

SELECT is(
    delta_e_cie_2000(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163, 1, 1, 1)::numeric,
    49.0713518946563,
    'calculate delta_e_cie_2000_full');

SELECT is(
    delta_e_cie_2000(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163)::numeric,
    49.0713518946563,
    'calculate delta_e_cie_2000');

SELECT is(
    color_distance_cie2000('(75.06, 48.17, -22.39)'::color, '(55.60, -37.44, -0.5163)'::color)::numeric,
    49.0713518946563,
    'calculate color_distance_cie2000');

SELECT is(
    delta_e_cie_2000(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163)::numeric,
    delta_e_cie_2000(55.60, -37.44, -0.5163, 75.06, 48.17, -22.39)::numeric,
    'comutative delta_e_cie_2000');

SELECT is(
    delta_e_cmc(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163, 2, 1)::numeric,
    46.7826986684777,
    'calculate delta_e_cmc_full');

SELECT is(
    delta_e_cmc(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163)::numeric,
    46.7826986684777,
    'calculate delta_e_cmc');

SELECT is(
    delta_e_cie_1994(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163, 1, 1, 1, 0.045, 0.015)::numeric,
    52.3670343098197,
    'calculate delta_e_cie_1994_full');

SELECT is(
    delta_e_cie_1994(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163)::numeric,
    52.3670343098197,
    'calculate delta_e_cie_1994');

SELECT is(
    delta_e_cie_1976(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163)::numeric,
    90.4777456156485,
    'calculate delta_e_cie_1976');

SELECT is(
    delta_e_cie_1976(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163)::numeric,
    delta_e_cie_1976(55.60, -37.44, -0.5163, 75.06, 48.17, -22.39)::numeric,
    'comutative delta_e_cie_1976');

SELECT is(
    color_distance_cie1976('(75.06, 48.17, -22.39)'::color, '(55.60, -37.44, -0.5163)'::color)::numeric,
    90.4777456156485,
    'calculate color_distance_cie1976');

/* 
currently this pgtap does not have has_index.
CREATE TABLE color_tbl AS SELECT random()*100 as L, random()*256-128 as a, random()*256-128 as b from generate_series(1, 10000);
ALTER TABLE color_tbl ADD COLUMN lab color;
UPDATE color_tbl SET lab = (L || ',' || a || ',' || b)::color;

CREATE INDEX color_tbl_idx ON color_tbl USING gist (lab gist_color_ops);
*/

SELECT * FROM finish();
ROLLBACK;
