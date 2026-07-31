#define CFLAGS "-g", "-IVulkan-Headers/include"
#define RES_PATH(X) X".app/Contents/Resources"
#include "build.h"

static int pch() {
  RUN("clang", "-Wall", "-g", "-x", "c-header",
    "-IVulkan-Headers/include",
    "-D", "VK_USE_PLATFORM_METAL_EXT",
    "-D", "VLK_USE_VOLK",
    "-o", "pch.pch", "pch.h");
  return 0;
}

static int link_exe() {
  RUN("clang", "-Wall",
    "-framework", "AppKit",
    "-framework", "AudioToolbox",
    "-framework", "MetalKit",
    "-o", APP".app/Contents/MacOS/main", 
    OBJS, "app.o", "volk.o");
  return 0;
}

static int link_shots_exe() {
  RUN("clang", "-Wall",
    "-framework", "AppKit",
    "-framework", "AudioToolbox",
    "-framework", "MetalKit",
    "-o", APP".app/Contents/MacOS/shots", 
    OBJS, "stb_image.o", "volk.o", "shots.o");
  return 0;
}


int main(int argc, char ** argv) {
  mkdir(APP".app", 0777);
  mkdir(APP".app/Contents", 0777);
  mkdir(APP".app/Contents/MacOS", 0777);
  mkdir(APP".app/Contents/Resources", 0777);

  RUN("cp", "libvulkan.dylib", APP".app/Contents/MacOS/");

  if (pch()) return 1;

  // It's nearly mandatory to use "modules" with ObjC.
  // The compilation speed without it is abismal.
  HDR("stb_image", "STB_IMAGE_IMPLEMENTATION");
  RUN("clang", "-Wall", "-g", "-fmodules", "-o", "app.o", "-c", "app-osx.m");
  CC("volk");
  if (compile_and_link_exe()) return 1;
  if (shaders()) return 1;

  CC("shots");
  if (link_shots_exe()) return 1;

  return 0;
}
