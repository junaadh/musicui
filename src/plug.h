#ifndef PLUG_H_
#define PLUG_H_

#include <raylib.h>
#include <stddef.h>

typedef struct {
  size_t version;
  Music music;
} Plug;

// typedef bool (*init_libs_t)(void);
typedef void(plug_hello_t)(void);
typedef void(plug_init_t)(Plug *plug, const char *file_path);
typedef void(plug_pre_hotreload_t)(Plug *plug);
typedef void(plug_post_hotreload_t)(Plug *plug);
typedef void(plug_update_t)(Plug *plug);

#define LIST_OF_PLUGS                                                          \
  PLUG(plug_hello)                                                             \
  PLUG(plug_init)                                                              \
  PLUG(plug_pre_hotreload)                                                     \
  PLUG(plug_post_hotreload)                                                    \
  PLUG(plug_update)

#endif // plug.h
