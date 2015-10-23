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

