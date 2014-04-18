

// convert area (x,y,xsize,ysize) to (minx,miny,maxx,maxy)
void area_rel2abs(int x, int y, int xsize, int ysize,
		  int *minx, int *miny, int *maxx, int *maxy)
{
  *minx = x;
  *miny = y;
  *maxx = x+xsize-1;
  *maxy = y+ysize-1;
}

// convert area (minx,miny,maxx,maxy) to (x,y,xsize,ysize)
void area_abs2rel(int minx, int miny, int maxx, int maxy,
		  int *x, int *y, int *xsize, int *ysize)
{
  *x = minx;
  *y = miny;
  *xsize = maxx-minx+1;
  *ysize = maxy-miny+1;
}

// the set intersection between two areas
// returns -1 if intersection is empty
int area_intersect(int aminx, int aminy, int amaxx, int amaxy,
		   int bminx, int bminy, int bmaxx, int bmaxy,
		   int *cminx, int *cminy, int *cmaxx, int *cmaxy)
{
  int maxofmins,minofmaxs;

  // x axis
  if (aminx > bminx) maxofmins = aminx;
  else maxofmins = bminx;
  if (amaxx < bmaxx) minofmaxs = amaxx;
  else minofmaxs = bmaxx;
  if (maxofmins > minofmaxs) return(-1);
  *cminx = maxofmins;
  *cmaxx = minofmaxs;
  // y axis
  if (aminy > bminy) maxofmins = aminy;
  else maxofmins = bminy;
  if (amaxy < bmaxy) minofmaxs = amaxy;
  else minofmaxs = bmaxy;
  if (maxofmins > minofmaxs) return(-1);
  *cminy = maxofmins;
  *cmaxy = minofmaxs;
  return(0);
}

int area_intersect_rel(int ax, int ay, int axsize, int aysize,
		       int bx, int by, int bxsize, int bysize,
		       int *cx, int *cy, int *cxsize, int *cysize)
{
  int aminx,aminy,amaxx,amaxy;
  int bminx,bminy,bmaxx,bmaxy;
  int cminx,cminy,cmaxx,cmaxy;
  int k;

  area_rel2abs(ax, ay, axsize, aysize, &aminx, &aminy, &amaxx, &amaxy);
  area_rel2abs(bx, by, bxsize, bysize, &bminx, &bminy, &bmaxx, &bmaxy);
  k = area_intersect(aminx, aminy, amaxx, amaxy,
		     bminx, bminy, bmaxx, bmaxy,
		     &cminx, &cminy, &cmaxx, &cmaxy);
  if (k == -1) return(-1);
  area_abs2rel(cminx, cminy, cmaxx, cmaxy, cx, cy, cxsize, cysize);
  return (k);
}
