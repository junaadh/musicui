#include "plug.h"
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define N (1 << 15)

typedef struct {
  float left;
  float right;
} Frame;

float in[N];
float complex out[N];
float volume;

float ampCalc(float complex x) {
  float a = fabs(creal(x));
  float b = fabs(cimag(x));

  if (a > b)
    return a;
  return b;
}

void fft(float in[], size_t stride, float complex out[], size_t n) {
  assert(n > 0);

  if (n == 1) {
    out[0] = in[0];
    return;
  }

  fft(in, stride * 2, out, n / 2);
  fft(in + stride, stride * 2, out + n / 2, n / 2);

  for (size_t k = 0; k < n / 2; ++k) {
    float t = (float)k / n;
    float complex calc = cexp(-2 * I * PI * t) * out[k + n / 2];
    float complex prev = out[k];
    out[k] = prev + calc;
    out[k + n / 2] = prev - calc;
  }
}

void callback(void *bufferData, unsigned int frames) {
  Frame *frame = bufferData;

  for (size_t i = 0; i < frames; ++i) {
    memmove(in, in + 1, (N - 1) * sizeof(in[0]));
    in[N - 1] = frame[i].left;
  }
}

void plug_hello() { printf("Hello \n"); }

void plug_init(Plug *plug, const char *file_path) {
  volume = 0.5f;
  plug->music = LoadMusicStream(file_path);
  assert(plug->music.stream.channels == 2);
  printf("music.frameCount = %u\n", plug->music.frameCount);
  printf("music.stream.sampleRate = %u\n", plug->music.stream.sampleRate);

  SetMusicVolume(plug->music, volume);
  PlayMusicStream(plug->music);
  AttachAudioStreamProcessor(plug->music.stream, callback);
}

void plug_update(Plug *plug) {
  UpdateMusicStream(plug->music);

  if (IsKeyPressed(KEY_SPACE)) {
    if (IsMusicStreamPlaying(plug->music)) {
      PauseMusicStream(plug->music);
    } else {
      ResumeMusicStream(plug->music);
    }
  } else if (IsKeyPressed(KEY_UP)) {
    SetMusicVolume(plug->music, volume < 1 ? volume = volume + 0.05f : volume);
  } else if (IsKeyPressed(KEY_DOWN)) {
    SetMusicVolume(plug->music, volume > 0 ? volume = volume - 0.05f : volume);
  } else if (IsKeyPressed(KEY_Q)) {
    StopMusicStream(plug->music);
    PlayMusicStream(plug->music);
  }

  int w = GetRenderWidth();
  int h = GetRenderHeight();

  BeginDrawing();
  ClearBackground(CLITERAL(Color){0x18, 0x18, 0x18, 0xff});

  fft(in, 1, out, N);

  float max = 0.0f;
  for (size_t i = 0; i < N; ++i) {
    float a = ampCalc(out[i]);
    if (max < a)
      max = a;
  }

  float step = 1.06f;
  size_t m = 0;
  for (float f = 20.0f; (size_t)f < N; f *= step) {
    m += 1;
  }

  float cellWidth = ((float)w / 2) / m;
  m = 0;
  // for (size_t i = 0; i < N; ++i) {
  for (float i = 20.0f; (size_t)i < N; i *= step) {
    float f1 = i * step;
    float a = 0.0f;
    for (size_t q = (size_t)i; q < N && q < (size_t)f1; ++q) {
      a += ampCalc(out[q]);
    }
    a /= (size_t)f1 - (size_t)i + 1;
    float t =  a / max;
    Color c = ColorAlphaBlend(RED, ColorAlpha(GREEN, t), WHITE);
    DrawRectangle(m * cellWidth, (float)h / 4 - (float)h / 4 * t, cellWidth,
                  (float)h / 4 * t, c);
    m += 1;
  }
  EndDrawing();
}

void plug_pre_hotreload(Plug *plug) {
  DetachAudioStreamProcessor(plug->music.stream, callback);
}

void plug_post_hotreload(Plug *plug) {
  AttachAudioStreamProcessor(plug->music.stream, callback);
}
