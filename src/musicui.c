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

#ifdef HOTRELOAD
#define PLUG(name) name##_t *name = NULL;
#else
#define PLUG(name) name##_t name;
#endif
LIST_OF_PLUGS
#undef PLUG

const char *libplug_file_name = "libplug.dylib";
void *libplug = NULL;
Plug plug = {0};

const char *shift_args(int *argc, const char ***argv) {
  assert(*argv > 0);
  const char *result = (**argv);
  (*argv) += 1;
  (*argc) -= 1;
  return result;
}

#ifdef HOTRELOAD
bool init_libs(void) {
  if (libplug != NULL)
    dlclose(libplug);

  libplug = dlopen(libplug_file_name, RTLD_NOW);
  if (libplug == NULL) {
    fprintf(stderr, "ERROR: couldnt load %s: %s", libplug_file_name, dlerror());
    return false;
  }

  #define PLUG(name) \
  name = dlsym(libplug, #name); \
  if (name == NULL) { \
    fprintf(stderr, "ERROR: couldnt find %s symbol in %s: %s", \
            #name, libplug_file_name, dlerror()); \
    return false; \
  } 
  LIST_OF_PLUGS
  #undef PLUG

  return true;
}
#else
#define init_libs() true
#endif

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
      plug_pre_hotreload(&plug);
      if(!init_libs()) return 1;
      plug_post_hotreload(&plug);
    }
    plug_update(&plug);
  }
  return 0;
}
