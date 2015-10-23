\set ECHO none
BEGIN;

\i test/pgtap-core.sql
\i sql/colors.sql

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

SELECT plan(14);
-- SELECT * FROM no_plan();

SELECT has_function('delta_e_cie_1976', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cie_1994', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cie_1994', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cmc', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cmc', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cie_2000', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);
SELECT has_function('delta_e_cie_2000', ARRAY['double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision', 'double precision']);

SELECT is(
    delta_e_cie_2000(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163, 1, 1, 1)::numeric,
    49.0713518946563,
    'calculate delta_e_cie_2000_full');

SELECT is(
    delta_e_cie_2000(75.06, 48.17, -22.39, 55.60, -37.44, -0.5163)::numeric,
    49.0713518946563,
    'calculate delta_e_cie_2000');

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


SELECT * FROM finish();
ROLLBACK;
