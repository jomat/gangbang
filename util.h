/*
	Copyright (C) 2011 jomat <jomat+gangbang@jmt.gr>
	Published under the terms of the GNU General Public License (GPL).
*/

#ifndef GANGBANG_UTIL_H
#define GANGBANG_UTIL_H

#define debug(...) do { FILE *f=fopen("/tmp/gangbang.log","a"); fprintf(f, __VA_ARGS__); fclose(f); } while(0);
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#endif /* GANGBANG_UTIL_H */
