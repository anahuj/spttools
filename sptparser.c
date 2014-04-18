
// Copyright 2006-2008 Juhana Sadeharju

// sptparser: writes the spt file to an editable file.
// sptcompiler: writes the editable file to the spt file.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "mylib/filesdirs.h"
#include "mylib/buffer.h"
#include "mylib/image.h"


void print_string(char *s)
{
  printf("string = \"%s\"\n",s);
}

void print_desc(char *s)
{
  printf("desc = \"%s\"\n",s);
}

void print_int(int n)
{
  printf("int = %i\n",n);
}

// XXXX Fix the float accuracy problem by printing the original
// XXXX float as hexadecimal. If the float is modified, the
// XXXX hexadecimal number should be changed to "---" or deleted.
void print_float(float f)
{
  printf("float = %.16f\n",f);
}

void print_byte(int n)
{
  printf("byte = %i\n",n);
}

void print_short(int n)
{
  printf("short = %i\n",n);
}


int main(int ac, char **av)
{
  unsigned char *buffer;
  int len,n,i;
  void *b;
  char *s;
  float f;
  int hierarchy;

  if (ac < 2) {
    fprintf(stderr,"Usage: sptparser <file>\n");
    exit(-1);
  }

  file2buffer(av[1],&buffer,&len);

  b = buffer_new(buffer,len);

  hierarchy = 0;
  while (buffer_eof(b) != 1) {
    // Section number.
    n = buffer_read_int(b);
#if 0
    for (i = 0; i < hierarchy; i++) {
      fprintf(stderr,"  ");
    }
    fprintf(stderr,"Section = %i\n",n);
    // fprintf(stderr,"Section = %i in %s\n",n,av[1]);
#endif

    printf("section = %i\n",n);
    switch (n) {
    case 1000:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("File type ID/Version");
      break;
    case 1001:
      break;
    case 1002:
      hierarchy++;
      break;
    case 1003:
      hierarchy--;
      break;
    case 1004:
      hierarchy++;
      break;
    case 1005:
      hierarchy--;
      break;
    case 1006:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Leaves/Number of leaf texture maps (equals 60007)");
      break;
    case 1007:
      hierarchy++;
      break;
    case 1008:
      hierarchy--;
      break;
    case 1009:
      hierarchy++;
      break;
    case 1010:
      hierarchy--;
      break;
    case 1011:
      hierarchy++;
      break;
    case 1012:
      hierarchy--;
      break;
    case 1014:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Number of tree levels (4 = trunk, branches level 1, branches level 2, leaves; does not include roots)");
      hierarchy++;
      break;
    case 1015:
      hierarchy--;
      break;
    case 1016:
      hierarchy++;
      break;
    case 1017:
      hierarchy--;
      break;
    case 2000:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Trunk and branches texture map");
      break;
    case 2001:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Compute LOD/Far distance Actual value");
      break;
    case 2002:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 2003:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Compute LOD/Near distance Actual value");
      break;
    case 2004:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 2005:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Global/Seeds/Main");
      break;
    case 2006:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Global/Size and Orientation/Size");
      break;
    case 2007:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Global/Size and Orientation/Size Variance");
      break;
    case 3000:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Blossom Rules/Distance(%%)");
      break;
    case 3001:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Leaves/Blossom Rules/Depth");
      break;
    case 3002:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Blossom Rules/Weight(%%)");
      break;
    case 3003:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 3004:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 3005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 3006:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 3007:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Placement/Tolerance");
      break;
    case 3008:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Leaves/Placement/Collision detection %% 0 None 1 Branch 2 Tree");
      break;
    case 3009:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Lighting/Leaf Lighting/Dimming toggle %% 0 off 1 on");
      break;
    case 3010:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Leaf Lighting/Dimming(%%)");
      break;
    case 4000:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Make Blossom %% 0 No 1 Yes");
      break;
    case 4001:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Texture Maps/Color");
      break;
    case 4002:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Texture Maps/Orientation Var(%%)");
      break;
    case 4003:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Leaves/Texture Maps/Leaf texture map filename");
      break;
    case 4004:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Texture Maps/Texture Map Origin (XYZ)");
      break;
    case 4005:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Texture Maps/Size / 100.0 %% in XY");
      break;
    case 4006:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 4007:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 5000:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 5001:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 5002:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 5003:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 5004:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 5005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 5006:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 6000:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Disturbance");
      break;
    case 6001:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Gravity");
      break;
    case 6002:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Flexibility");
      break;
    case 6003:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Flexibility Profile");
      break;
    case 6004:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Length");
      break;
    case 6005:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Radius");
      break;
    case 6006:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Radius Profile");
      break;
    case 6007:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Start Angle");
      break;
    case 6008:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Segments/Cross-section");
      break;
    case 6009:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Segments/Length");
      break;
    case 6010:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Branches/Generation/First");
      break;
    case 6011:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Branches/Generation/Last");
      break;
    case 6012:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Branches/Generation/Frequency");
      break;
    case 6013:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Texture Map Tiling/U tiling");
      break;
    case 6014:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Texture Map Tiling/V tiling");
      break;
    case 6015:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Texture Map Tiling/U Abs");
      break;
    case 6016:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Texture Map Tiling/V Abs");
      break;
    case 6017:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Gravity Profile");
      break;
    case 7000:
      hierarchy++;
      break;
    case 7001:
      hierarchy--;
      break;
    case 8000:
      hierarchy++;
      break;
    case 8001:
      hierarchy--;
      break;
    case 8002:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("% Lighting/Branch Lighting/Lighting style %% 0 Dynamic 1 Static");
      break;
    case 8003:
      for (i = 0; i < 13; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("Lighting/Materials/Branches/DiffuseColor AmbientColor SpecularColor EmissiveColor Shine");
      break;
    case 8004:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Lighting/Leaf Lighting/Lighting style %% 0 Dynamic 1 Static");
      break;
    case 8005:
      for (i = 0; i < 13; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("Lighting/Materials/Leaves/DiffuseColor AmbientColor SpecularColor EmissiveColor Shine");
      break;
    case 8006:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Leaf Lighting/Lighting Adjust");
      break;
    case 8007:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Lighting/Leaf Lighting/Static lighting style + Simulate shadows %% 0x0 Basic + Off 0x1 Use light sources + Off 0x2 Basic + On 0x3 Use light sources + On");
      break;
    case 8008:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Lighting/Frond Lighting/Lighting style %% 0 Dynamic 1 Static");
      break;
    case 8009:
      for (i = 0; i < 13; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("Lighting/Materials/Fronds/DiffuseColor AmbientColor SpecularColor EmissiveColor Shine");
      break;
    case 9000:
      hierarchy++;
      break;
    case 9001:
      hierarchy--;
      break;
    case 9002:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("LOD/LeavesBillboard/Transition method %% 0 Pop 1 Smooth 3 None");
      break;
    case 9003:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/LeavesBillboard/Radius");
      break;
    case 9004:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/LeavesBillboard/Exponent");
      break;
    case 9005:
      break;
    case 9006:
      break;
    case 9007:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("LOD/Branches/Num LODs");
      break;
    case 9008:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Branches/Min vol(%%)");
      break;
    case 9009:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/LeavesBillboard/Size(%%)");
      break;
    case 9010:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/LeavesBillboard/Reduction(%%)");
      break;
    case 9011:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("LOD/LeavesBillboard/Num LODs");
      break;
    case 9012:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Branches/Max vol(%%)");
      break;
    case 9013:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Branches/Fuzziness(%%)");
      break;
    case 9014:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Branches/Retention(%%)");
      break;
    case 10000:
      hierarchy++;
      break;
    case 10001:
      hierarchy--;
      break;
    case 10002:
      n = buffer_read_int(b);
      print_int(n);
      for (i = 0; i < 8*n; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("Composite map/Texture quads for leaves");
      break;
    case 10003:
      n = buffer_read_int(b);
      print_int(n);
      for (i = 0; i < 8*n; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("Composite map/Texture quads for 360 deg billboards");
      break;
    case 10004:
      n = buffer_read_int(b);
      print_int(n);
      for (i = 0; i < 8*n; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("Composite map/Texture quads for fronds");
      break;
    case 11000:
      hierarchy++;
      break;
    case 11001:
      hierarchy--;
      break;
    case 11002:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Wind/Branch Weighting Level/First level receiving wind effects");
      break;
    case 12000:
      hierarchy++;
      break;
    case 12001:
      hierarchy--;
      break;
    case 12002:
      for (i = 0; i < 4; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("Collision Object/Sphere: XYZ position, Radius");
      break;
    case 12003:
      for (i = 0; i < 5; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("Collision Object/Capsule: XYZ position, Radius, Length");
      break;
    case 12004:
      for (i = 0; i < 6; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("Collision Object/Box: XYZ extend 1, XYZ extend 2 (nearly XYZ Min XYZ Max)");
      break;
    case 13000:
      hierarchy++;
      break;
    case 13001:
      hierarchy--;
      break;
    case 13002:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fronds/Generation/Level");
      break;
    case 13003:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fronds/Blades+ExtrudedProfile toggle %% 0 Blades 1 Extruded Profile");
      break;
    case 13004:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fronds/Blades/Number of blades per frond");
      break;
    case 13005:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Fronds/Extruded Profile/Profile");
      break;
    case 13006:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fronds/Extruded Profile/Segments + 1");
      break;
    case 13007:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Fronds/Generation/Enabled");
      break;
    case 13008:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fronds/Number of frond texture maps (equals 60008)");
      break;
    case 13009:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("LOD/Fronds/Num LODs");
      break;
    case 13010:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Fronds/Max area(%%)");
      break;
    case 13011:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Fronds/Min area(%%)");
      break;
    case 13012:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Fronds/Fuzziness(%%)");
      break;
    case 13013:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Fronds/Retention(%%)");
      break;
    case 14000:
      hierarchy++;
      break;
    case 14001:
      hierarchy--;
      break;
    case 14002:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Fronds/Texture Maps/Frond texture map filename");
      break;
    case 14003:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 14004:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Fronds/Texture Maps/Frond size factor");
      break;
    case 14005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Fronds/Texture Maps/Min offset angle");
      break;
    case 14006:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Fronds/Texture Maps/Max offset angle");
      break;
    case 14007:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("LOD/Fronds/Minimum extrusion segments/Length");
      break;
    case 14008:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("LOD/Fronds/Minimum extrusion segments/Profile");
      break;
    case 15000:
      hierarchy++;
      break;
    case 15001:
      hierarchy--;
      break;
    case 15002:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Trunk/Texture Map Tiling/Random V offset");
      break;
    case 15003:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Trunk/Texture Map Tiling/Twist");
      break;
    case 16000:
      hierarchy++;
      break;
    case 16001:
      hierarchy--;
      break;
    case 16002:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Segments/Pack");
      break;
    case 16003:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Flares/Flare Parameters/Number");
      break;
    case 16004:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Flares/Flare Parameters/Balance");
      break;
    case 16005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Flares/Flare Parameters/Radial influence");
      break;
    case 16006:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Flares/Flare Parameters/Radial influence variance");
      break;
    case 16007:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Flares/Flare Parameters/Radial exponent");
      break;
    case 16008:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Flares/Flare Parameters/Radial distance");
      break;
    case 16009:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Flares/Flare Parameters/Radial distance variance");
      break;
    case 16010:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Flares/Flare Parameters/Length distance");
      break;
    case 16011:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Flares/Flare Parameters/Length distance variance");
      break;
    case 16012:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Flares/Flare Parameters/Length exponent");
      break;
    case 16013:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Global/Seeds/Flare");
      break;
    case 16014:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/LeavesBillboards/Leaf transition(%) / 2.0");
      break;
    case 18000:
      hierarchy++;
      break;
    case 18001:
      hierarchy--;
      break;
    case 18002:
      for (i = 0; i < 3; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 18003:
      for (i = 0; i < 3; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 18004:
      for (i = 0; i < 3; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 18005:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Global/Self-shadow map filename");
      break;
    case 19000:
      hierarchy++;
      break;
    case 19001:
      hierarchy--;
      break;
    case 19002:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 20000:
      hierarchy++;
      break;
    case 20001:
      hierarchy--;
      break;
    case 20002:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Global/Composite map filename");
      break;
    case 20003:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 20004:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 20005:
      for (i = 0; i < 8; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 21000:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Wind/SpeedWind Support/Leaf rock angle scalar");
      break;
    case 21001:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Wind/SpeedWind Support/Leaf rustle angle scalar");
      break;
    case 22000:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Wind/Branch Weighting Level/Propagate flexibility to higher levels");
      break;
    case 23002:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 23003:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("XXXX Unknown");
      // print_desc("");
      break;
    case 25000:
      hierarchy++;
      break;
    case 25001:
      hierarchy--;
      break;
    case 25002:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Fronds/Generation/Rules/Distance(%%)");
      break;
    case 25003:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fronds/Generation/Rules/Depth");
      break;
    case 25004:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fronds/Generation/Rules/Above condition %% 0 Enabled 1 Disabled 2 Pruned");
      break;
    case 25005:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fronds/Generation/Rules/Below condition %% 0 Enabled 1 Disabled 2 Pruned");
      break;
    case 25006:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fronds/Length Segment Override/Value");
      break;
    case 25007:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Fronds/Length Segment Override/Enabled");
      break;
    case 26000:
      hierarchy++;
      break;
    case 26001:
      hierarchy--;
      break;
    case 26002:
      break;
    case 26003:
      break;
    case 26004:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Roughness/Amount");
      break;
    case 26005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Segments/Keep%% (Length)");
      break;
    case 26006:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Segments/Keep%% (Cross-section)");
      break;
    case 26007:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Generation/Dist(%%)");
      break;
    case 26008:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Generation/Depth");
      break;
    case 26009:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Fork");
      break;
    case 26010:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Fork/Bias");
      break;
    case 26011:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Fork/Angle");
      break;
    case 26012:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fork/Limit");
      break;
    case 26013:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Segments/Profile curve");
      break;
    case 26014:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Light Seam Reduction/Normal Profile curve");
      break;
    case 26015:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Roughness/Vert freq");
      break;
    case 26016:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Roughness/Horiz freq");
      break;
    case 26017:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Roughness/Amount Variance");
      break;
    case 26018:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Roughness/Rough profile");
      break;
    case 26019:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Generation/Profile (Freq profile)");
      break;
    case 26020:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Light Seam Reduction/Bias");
      break;
    case 26021:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Gnarl");
      break;
    case 26022:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Roughness/Gnarl profile");
      break;
    case 26023:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Roughness/Unison");
      break;
    case 27000:
      hierarchy++;
      break;
    case 27001:
      hierarchy--;
      break;
    case 27002:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Global/Floor toggle");
      break;
    case 27003:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Global/Floor/Value");
      break;
    case 27004:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Global/Floor/Level");
      break;
    case 27005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Global/Floor/Exponent");
      break;
    case 27006:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("1.0 - Global/Floor/Bias");
      break;
    case 28000:
      hierarchy++;
      break;
    case 28001:
      hierarchy--;
      break;
    case 28002:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Lighting/Leaf Lighting/Smoothing toggle %% 0 off 1 on");
      break;
    case 28003:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Leaf Lighting/Smoothing");
      break;
    case 28004:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Lighting/Leaf Lighting/Dimming depth");
      break;
    case 29000:
      hierarchy++;
      break;
    case 29001:
      hierarchy--;
      break;
    case 29002:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Global/Cluster Support/First level with visible branches");
      break;
    case 30000:
      hierarchy++;
      break;
    case 30001:
      hierarchy--;
      break;
    case 30002:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Standard Shaders/3D light/Branch");
      break;
    case 30003:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Standard Shaders/3D light/Frond");
      break;
    case 30004:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Standard Shaders/3D light/Leaf");
      break;
    case 30005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Standard Shaders/3D light/Global");
      break;
    case 30006:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Standard Shaders/3D light/Ambient");
      break;
    case 30007:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Standard Shaders/Billboard light/Dark side");
      break;
    case 30008:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Standard Shaders/Billboard light/Ambient");
      break;
    case 30009:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Lighting/Standard Shaders/Billboard light/Bright side");
      break;
    case 40000:
      hierarchy++;
      break;
    case 40001:
      hierarchy--;
      break;
    case 40002:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Root Level");
      break;
    case 40003:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Roots/Generation/First");
      break;
    case 40004:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Roots/Generation/Last");
      break;
    case 40005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Roots/Generation/Freq");
      break;
    case 40006:
      // XXXX Begin of roots subspt section.
      break;
    case 40007:
      // XXXX End of roots subspt section.
      break;
    case 40008:
      // XXXX End of ??
      break;
    case 50000:
      hierarchy++;
      break;
    case 50001:
      hierarchy--;
      break;
    case 50002:
      break;
    case 50003:
      break;
    case 50004:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("U tiling");
      break;
    case 50005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("V tiling");
      break;
    case 50006:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("U Absolute");
      break;
    case 50007:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("V Absolute");
      break;
    case 50008:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Twist");
      break;
    case 50009:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Random V offset");
      break;
    case 50010:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("V offset");
      break;
    case 50011:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Clamp U");
      break;
    case 50012:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Clamp V");
      break;
    case 50013:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Left");
      break;
    case 50014:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Right");
      break;
    case 50015:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Bottom");
      break;
    case 50016:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Top");
      break;
    case 50017:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("U offset");
      break;
    case 50018:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Sync to diffuse");
      break;
    case 60000:
      hierarchy++;
      break;
    case 60001:
      hierarchy--;
      break;
    case 60002:
      print_desc("Begin of trunk+branches texture map layers");
      break;
    case 60003:
      print_desc("Begin of leaves texture map layers");
      break;
    case 60004:
      print_desc("Begin of fronds texture map layers");
      break;
    case 60005:
      print_desc("Begin of composite map layers (leaves+fronds)");
      break;
    case 60006:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Global/Self-shadow map filename");
      break;
    case 60007:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Leaves/Number of leaf texture maps (equals 1006)");
      break;
    case 60008:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Fronds/Number of frond texture maps (equals 13008)");
      break;
    case 60009:
      print_desc("Begin of separate billboards map layers");
      break;
    case 70000:
      hierarchy++;
      break;
    case 70001:
      hierarchy--;
      break;
    case 70002:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Diffuse map filename");
      break;
    case 70003:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Detail map filename");
      break;
    case 70004:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Normal map filename");
      break;
    case 70005:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Height map filename");
      break;
    case 70006:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Specular map filename");
      break;
    case 70007:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("User 1 map filename");
      break;
    case 70008:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("User 2 map filename");
      break;
    case 71000:
      break;
    case 71001:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Leaves/Use Mesh/Number of leaf meshes");
      break;
    case 71002:
      break;
    case 71003:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      print_desc("Leaves/Mesh/Mesh Name");
      break;
    case 71004:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Leaves/Mesh/Number of vertices");
      break;
    case 71005:
      break;
    case 71006:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Mesh/Vertex/Position");
      break;
    case 71007:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Mesh/Vertex/Normal");
      break;
    case 71008:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Mesh/Vertex/Tangent");
      break;
    case 71009:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Mesh/Vertex/Binormal");
      break;
    case 71010:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Mesh/Vertex/UV coordinates");
      break;
    case 71011:
      break;
    case 71012:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Leaves/Mesh/Triangle Table/Number of vertex indices");
      break;
    case 71013:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Leaves/Mesh/Triangle Table/Index");
      break;
    case 71014:
      break;
    case 71015:
      break;
    case 72000:
      break;
    case 72001:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("Leaves/Use Mesh toggle");
      break;
    case 72003:
      n = buffer_read_int(b);
      print_int(n);
      print_desc("Leaves/Use mesh/Number of the selected mesh");
      break;
    case 72004:
      break;
    case 72005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Texture Maps/Hang(%%)");
      break;
    case 72006:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Leaves/Texture Maps/Rotate");
      break;
    case 73000:
      hierarchy++;
      break;
    case 73001:
      hierarchy--;
      break;
    case 73002:
      break;
    case 73003:
      break;
    case 73004:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Collision Object/X angle");
      break;
    case 73005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Collision Object/Y angle");
      break;
    case 73006:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Collision Object/Z angle");
      break;
    case 74000:
      hierarchy++;
      break;
    case 74001:
      hierarchy--;
      break;
    case 74002:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("Global/Size and Orientation/Angle");
      break;
    case 75000:
      hierarchy++;
      break;
    case 75001:
      hierarchy--;
      break;
    case 75002:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Branches/Fade distance");
      break;
    case 75003:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/Fronds/Fade distance");
      break;
    case 75004:
      n = buffer_read_byte(b);
      print_byte(n);
      print_desc("LOD/Fronds/Apply fading to extruded fronds");
      break;
    case 75005:
      f = buffer_read_float(b);
      print_float(f);
      print_desc("LOD/LeavesBillboards/Billboard transition(%) / 2.0");
      break;
    default:
      fprintf(stderr,"Unknown section %i at %i / %s\n",n,buffer_getloc(b),av[1]);
      exit(-1);
    }
  }

  return 1;
}
