#include "plug.h"
#include <assert.h>
#include <complex.h>
#include <dlfcn.h>
#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])

const char *libplug_file_name = "libplug.dylib";
void *libplug = NULL;
plug_hello_t plug_hello = NULL;
plug_init_t plug_init = NULL;
plug_update_t plug_update = NULL;
Plug plug = {0};



const char *shift_args(int *argc, const char ***argv) {
  assert(*argv > 0);
  const char *result = (**argv);
  (*argv) += 1;
  (*argc) -= 1;
  return result;
}

bool init_libs(void) {
  if (libplug != NULL)
    dlclose(libplug);

  void *libplug = dlopen(libplug_file_name, RTLD_NOW);
  if (libplug == NULL) {
    fprintf(stderr, "ERROR: couldnt load %s: %s", libplug_file_name, dlerror());
    return false;
  }

  plug_init = dlsym(libplug, "plug_init");
  if (plug_init == NULL) {
    fprintf(stderr, "ERROR: couldnt find plug_init symbol in %s",
            libplug_file_name);
    return false;
  }
  plug_update = dlsym(libplug, "plug_update");
  if (plug_update == NULL) {
    fprintf(stderr, "ERROR: couldnt find plug_update symbol in %s",
            libplug_file_name);
    return false;
  }
  return true;
}

int main(int argc, const char **argv) {
  if (!init_libs())
    return 1;

  const char *program = shift_args(&argc, &argv);
  if (argc == 0) {
    fprintf(stderr, "Usage:  %s <input>\n", program);
    return 1;
  }
  const char *file_path = shift_args(&argc, &argv);

  InitWindow(800, 600, "MusicUI");
  SetTargetFPS(60);
  InitAudioDevice();

  plug_init(&plug, file_path);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_R)) {
      if (!init_libs())
        return 1;
    }
    plug_update(&plug);
  }
  return 0;
}
