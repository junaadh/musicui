#include "plug.h"
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N (1 << 13)
#define FONT_SIZE 48

typedef struct {
  float left;
  float right;
} Frame;

typedef struct {
  Music music;
  Font font;
  bool error;
} Plug;

Plug *plug = NULL;
float in[N];
float complex out[N];
float volume = 0.5f;

float ampCalc(float complex x) {
  // float a = fabs(creal(x));
  // float b = fabs(cimag(x));

  // if (a > b)
  //   return a;
  // return b;
  return cabsf(x);
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
  float(*frame)[plug->music.stream.channels] = bufferData;

  for (size_t i = 0; i < frames; ++i) {
    memmove(in, in + 1, (N - 1) * sizeof(in[0]));
    in[N - 1] = frame[i][0];
  }
}

void plug_init(void) {
  plug = malloc(sizeof(*plug));
  assert(plug != NULL && "NOMO RAM!");
  memset(plug, 0, sizeof(*plug));

  plug->font = LoadFontEx("fonts/dank-mono.ttf", FONT_SIZE, NULL, 0);
}

void plug_update() {
  if (IsMusicReady(plug->music)) {
    UpdateMusicStream(plug->music);
  }

  if (IsKeyPressed(KEY_SPACE)) {
    if (IsMusicReady(plug->music)) {
      if (IsMusicStreamPlaying(plug->music)) {
        PauseMusicStream(plug->music);
      } else {
        ResumeMusicStream(plug->music);
      }
    }
  }
  if (IsKeyPressed(KEY_UP)) {
    if (IsMusicReady(plug->music)) {
      SetMusicVolume(plug->music,
                     volume < 1 ? volume = volume + 0.05f : volume);
    }
  } else if (IsKeyPressed(KEY_DOWN)) {
    if (IsMusicReady(plug->music)) {
      SetMusicVolume(plug->music,
                     volume > 0 ? volume = volume - 0.05f : volume);
    }
  }

  if (IsKeyPressed(KEY_Q)) {
    if (IsMusicReady(plug->music)) {
      StopMusicStream(plug->music);
      PlayMusicStream(plug->music);
    }
  }

  if (IsFileDropped()) {
    FilePathList dropped = LoadDroppedFiles();
    if (dropped.count > 0) {
      const char *file_path = dropped.paths[0];

      if (IsMusicReady(plug->music)) {
        StopMusicStream(plug->music);
        UnloadMusicStream(plug->music);
      }

      plug->music = LoadMusicStream(file_path);

      if (IsMusicReady(plug->music)) {
        plug->error = false;
        printf("music.frameCount = %u\n", plug->music.frameCount);
        printf("music.stream.sampleRate = %u\n", plug->music.stream.sampleRate);

        SetMusicVolume(plug->music, volume);
        PlayMusicStream(plug->music);
        AttachAudioStreamProcessor(plug->music.stream, callback);
      } else {
        plug->error = true;
      }
    }
    UnloadDroppedFiles(dropped);
  }

  int w = GetRenderWidth();
  int h = GetRenderHeight();

  BeginDrawing();
  ClearBackground(CLITERAL(Color){0x18, 0x18, 0x18, 0xff});

  if (IsMusicReady(plug->music)) {
    fft(in, 1, out, N);

    float step = 1.06f;
    float max = 0.0f;
    for (size_t i = 0; i < N / 2; ++i) {
      float a = ampCalc(out[i]);
      if (max < a)
        max = a;
    }

    size_t m = 0;
    float lowf = 1.0f;
    for (float f = lowf; (size_t)f < N / 2; f = ceil(f * step)) {
      m += 1;
    }

    float cellWidth = ((float)w / 2) / m;
    m = 0;
    // for (size_t i = 0; i < N/2/2; ++i) {
    for (float i = lowf; (size_t)i < N / 2; i = ceil(i * step)) {
      float f1 = ceil(i * step);
      float a = 0.0f;
      for (size_t q = (size_t)i; q < N / 2 && q < (size_t)f1; ++q) {
        float high = ampCalc(out[q]);
        if (a < high)
          a = high;
      }
      // a /= (size_t)f1 - (size_t)i + 1;
      float t = a / max;
      // if (t < 0.01f) {
      //     t = 0.01f;
      //   }
      Color c = ColorAlphaBlend(RED, ColorAlpha(GREEN, t), YELLOW);
      double screen_height = (double)h / 4;
      double wave_height = screen_height * (double)t;
      // if (wave_height < 1) {
      //   wave_height = 1;
      // }
      DrawRectangle(m * cellWidth, screen_height - wave_height, cellWidth,
                    wave_height, c);
      // DrawRectangle(m * cellWidth, (float)h / 4 + ((float)h * 4) / t,
      // cellWidth, (float)h / 4 * t, c);
      // DrawCircle(m * cellWidth, (float)h / 4, (float)h / 4 * t, c);
      m += 1;
    }
  } else {
    const char *msg;
    Color color;
    if (plug->error) {
      msg = "Cudnt Load FILE Bruh";
      color = RED;
    } else {
      msg = "DRAG MUSIC HERE!";
      color = BLUE;
    }
    Vector2 vec = MeasureTextEx(plug->font, msg, plug->font.baseSize, 0);
    Vector2 pos = {(float)w / 4 - vec.x / 2, (float)h / 4 - vec.y / 2.};
    DrawTextEx(plug->font, msg, pos, plug->font.baseSize, 0, color);
  }
  EndDrawing();
}

Plug *plug_pre_hotreload(void) {
  if (IsMusicReady(plug->music)) {
    DetachAudioStreamProcessor(plug->music.stream, callback);
  }
  return plug;
}

void plug_post_hotreload(Plug *state) {
  plug = state;
  if (IsMusicReady(plug->music)) {
    AttachAudioStreamProcessor(plug->music.stream, callback);
  }
}
