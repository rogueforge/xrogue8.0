/*
 * version number.  Whenever a new version number is desired, use
 * sccs to get vers.c.  Environ and encstr are declared here to
 * force them to be loaded before the version number, and therefore
 * not to be written in saved games.
 */

unsigned char encstr[] = "\0\0\0";
char version[] = "@(#)vers.c    8.0.1 - 05/01/2000";
char *release = "8.0.1";

