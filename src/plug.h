#ifndef PLUG_H_
#define PLUG_H_

// Defined using compiler code generation
// typedef bool (*init_libs_t)(void);
// typedef void(plug_init_t)(Plug *plug, const char *file_path);
// typedef void(plug_pre_hotreload_t)(Plug *plug);
// typedef void(plug_post_hotreload_t)(Plug *plug);
// typedef void(plug_update_t)(Plug *plug);

#define LIST_OF_PLUGS                                                          \
  PLUG(plug_init, void, void)                                                  \
  PLUG(plug_pre_hotreload, void *, void)                                       \
  PLUG(plug_post_hotreload, void, void *)                                      \
  PLUG(plug_update, void, void)

#define PLUG(name, ret, ...) typedef ret(name##_t)(__VA_ARGS__);
LIST_OF_PLUGS
#undef PLUG

#endif // plug.h
