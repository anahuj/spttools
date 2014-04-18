
#ifndef AREA_H
#define AREA_H

// convert area (x,y,xsize,ysize) to (minx,miny,maxx,maxy)
void area_rel2abs(int, int, int, int,
		  int *, int *, int *, int *);

// convert area (minx,miny,maxx,maxy) to (x,y,xsize,ysize)
void area_abs2rel(int, int, int, int,
		  int *, int *, int *, int *);

// the set intersection between two areas
// returns -1 if intersection is empty
int area_intersect(int, int, int, int,
		   int, int, int, int,
		   int *, int *, int *, int *);

// the set intersection between two areas
// returns -1 if intersection is empty
int area_intersect_rel(int, int, int, int,
		       int, int, int, int,
		       int *, int *, int *, int *);

#endif
