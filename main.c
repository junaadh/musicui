#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])

uint32_t global_frames[1024] = {0};
size_t global_frames_count = 0;

void callback(void *bufferData, unsigned int frames) {
  if (frames > ARRAY_LEN(global_frames))
    frames = ARRAY_LEN(global_frames);
  memcpy(global_frames, bufferData, sizeof(int32_t) * frames);
  global_frames_count = frames;
}

int main(void) {
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
    float cellWidth = (float)w / global_frames_count;
    for (size_t i = 0; i < global_frames_count; ++i) {
      int16_t sample = *(int16_t *)&global_frames[i];
      if (sample > 0) {
        float t = ((float)sample) / INT16_MAX;
        DrawRectangle(i * cellWidth, (float)h/4 - (float)h/4*t, cellWidth, (float)h/4*t,RED);
      } else {
        float t = ((float)sample)/ INT16_MIN;
        DrawRectangle(i * cellWidth, h/4, cellWidth, (float)h/4*t,RED);
      }
    }
    // printf("\n");
    EndDrawing();
  }
  return 0;
}
