#define CFLAGS "-g"
#define RES_PATH(X) X".app/Contents/Resources"
#include "build.h"

#define CROSS(X) RUN("spirv-cross", X".spv", "--msl", "--output", APP".app/Contents/Resources/"X".metal", "--flip-vert-y");

static void print_key(FILE * f, const char * key) {}

static int pch() {
  RUN("clang", "-Wall", "-x", "c-header", "-o", "pch.pch", "pch.h", CFLAGS);
  return 0;
}

#define LINK(X, ...) RUN("clang", "-Wall", "-o", APP".app/Contents/MacOS/"X, __VA_ARGS__)

static int link_exe() {
  LINK("main", OBJS, "app-osx.o");
  return 0;
}

int main(int argc, char ** argv) {
  mkdir(APP".app", 0777);
  mkdir(APP".app/Contents", 0777);
  mkdir(APP".app/Contents/MacOS", 0777);
  mkdir(APP".app/Contents/Resources", 0777);

  if (pch()) return 1;

  CM("app-osx");
  if (compile_and_link_exe()) return 1;
  if (shaders()) return 1;
  CROSS("shader.vert");
  CROSS("shader.frag");

  CM("bited");
  LINK("bited", "bited.o");

  CM("shots-osx");
  LINK("shots", OBJS, "shots-osx.o");
  SHADER("bited.vert"); CROSS("bited.vert");
  SHADER("bited.frag"); CROSS("bited.frag");

  return 0;
}
