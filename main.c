#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <complex.h>

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])
#define N 256

float in[N];
float complex out[N];
float pi;
float max;

typedef struct {
  float left;
  float right;
} Frame;

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
    float complex calc = cexp(-2 * I * pi * t) * out[k + n / 2];
    float complex prev = out[k];
    out[k] = prev + calc;
    out[k + n / 2] = prev - calc;
  }
}

float ampCalc(float complex x) {
  float a = fabs(creal(x));
  float b = fabs(cimag(x));

  if (a > b) return a;
  return b;
}

void callback(void *bufferData, unsigned int frames) {
  if (frames < N) {
    return;
  }

  Frame *frame = bufferData;

  for (size_t i = 0; i < frames; ++i) {
    in[i] = frame[i].left;
  }

  fft(in, 1, out, N);

  max = 0.0f;
  for (size_t i = 0; i < frames; ++i) {
    float a = ampCalc(out[i]);
    if (max < a) max = a;
  }
}

int main(void) {
  pi = atan2f(1, 1)*4;  
  InitWindow(800, 600, "MusicUI");
  SetTargetFPS(60);
  float volume = 0.5f;

  InitAudioDevice();
  Music music = LoadMusicStream("psycho.mp3");
  // assert(music.stream.sampleRate == 16);
  assert(music.stream.channels == 2);
  printf("music.frameCount = %u\n", music.frameCount);
  printf("music.stream.sampleRate = %u\n", music.stream.sampleRate);

  PlayMusicStream(music);
  SetMusicVolume(music, volume);
  AttachAudioStreamProcessor(music.stream, callback);

  while (!WindowShouldClose()) {
    UpdateMusicStream(music);

    if (IsKeyPressed(KEY_SPACE)) {
      if (IsMusicStreamPlaying(music))
        PauseMusicStream(music);
      else
        ResumeMusicStream(music);
    } else if (IsKeyPressed(KEY_UP))
      SetMusicVolume(music, volume < 1 ? volume = volume + 0.05f : volume);
    else if (IsKeyPressed(KEY_DOWN))
      SetMusicVolume(music, volume > 0 ? volume = volume - 0.05f : volume);

    int w = GetRenderWidth();
    int h = GetRenderHeight();

    BeginDrawing();
    ClearBackground(CLITERAL(Color){0x18, 0x18, 0x18, 0xff});
    float cellWidth = (float)w / N;
    for (size_t i = 0; i < N; ++i) {
        float t = ampCalc(out[i]); ///max;
        DrawRectangle(i*cellWidth, (float)h/4 - (float)h/4*t, 1, (float)h/4*t, RED);
    }
    EndDrawing();
  }
  return 0;
}
