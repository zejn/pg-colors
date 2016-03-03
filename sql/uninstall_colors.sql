
DROP OPERATOR CLASS gist_color_ops;
DROP DOMAIN color;
DROP FUNCTION g_color_distance(internal, cube, smallint, oid, internal);

DROP FUNCTION delta_e_cie_1976(double precision, double precision, double precision, double precision, double precision, double precision);
DROP FUNCTION delta_e_cie_1994(double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision);
DROP FUNCTION delta_e_cie_1994(double precision, double precision, double precision, double precision, double precision, double precision);
DROP FUNCTION delta_e_cmc(double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision);
DROP FUNCTION delta_e_cmc(double precision, double precision, double precision, double precision, double precision, double precision);
DROP FUNCTION delta_e_cie_2000(double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision, double precision);
DROP FUNCTION delta_e_cie_2000(double precision, double precision, double precision, double precision, double precision, double precision);


DROP FUNCTION color_distance_cie2000(cube, cube);
DROP FUNCTION color_distance_cie1994(cube, cube);
DROP FUNCTION color_distance_cie1974(cube, cube);