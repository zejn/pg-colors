colors 0.0.1
============

[![PGXN version](https://badge.fury.io/pg/colors.svg)](https://badge.fury.io/pg/colors)

This library contains four PostgreSQL C functions for calculating color
difference in L\*a\*b\* color space, which is designed for evaluating
perceptual similarity.

Installation
------------

To build colors:

    make
    make install
    make installcheck

If you encounter an error such as:

    "Makefile", line 8: Need an operator

You need to use GNU make, which may well be installed on your system as
`gmake`:

    gmake
    gmake install
    gmake installcheck

If you encounter an error such as:

    make: pg_config: Command not found

Be sure that you have `pg_config` installed and in your path. If you used a
package management system such as RPM to install PostgreSQL, be sure that the
`-devel` package is also installed. If necessary tell the build process where
to find it:

    env PG_CONFIG=/path/to/pg_config make && make installcheck && make install

If you encounter an error such as:

    ERROR:  must be owner of database regression

You need to run the test suite using a super user, such as the default
"postgres" super user:

    make installcheck PGUSER=postgres

Once colors is installed, you can add it to a database. If you're running
PostgreSQL 9.1.0 or greater, it's a simple as connecting to a database as a
super user and running:

    CREATE EXTENSION colors;

For versions of PostgreSQL less than 9.1.0, you'll need to run the
installation script:

    psql -d mydb -f /path/to/pgsql/share/contrib/colors.sql

If you want to install colors and all of its supporting objects into a
specific schema, use the `PGOPTIONS` environment variable to specify the
schema, like so:

    PGOPTIONS=--search_path=extensions psql -d mydb -f colors.sql

Dependencies
------------
The `colors` data type has no dependencies other than PostgreSQL and PL/pgSQL.

Copyright and License
---------------------

Copyright (c) 2010-2014 Gasper Zejn.

This module is free software; you can redistribute it and/or modify it under
the [PostgreSQL License](http://www.opensource.org/licenses/postgresql).

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose, without fee, and without a written agreement is
hereby granted, provided that the above copyright notice and this paragraph
and the following two paragraphs appear in all copies.

In no event shall Gasper Zejn be liable to any party for direct, indirect,
special, incidental, or consequential damages, including lost profits,
arising out of the use of this software and its documentation, even if
Gasper Zejn has been advised of the possibility of such damage.

Gasper Zejn specifically disclaims any warranties, including, but not limited
to, the implied warranties of merchantability and fitness for a particular
purpose. The software provided hereunder is on an "as is" basis, and
Gasper Zejn has no obligations to provide maintenance, support, updates,
enhancements, or modifications.
