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

typedef struct {
  float left;
  float right;
} Frame;

Frame global_frames[4410*2] = {0};
size_t global_frames_count = 0;

void callback(void *bufferData, unsigned int frames) {
  size_t capacity = ARRAY_LEN(global_frames);
  if (capacity - global_frames_count >= frames) {
    memcpy(global_frames + global_frames_count, bufferData,
           sizeof(Frame) * frames);
    global_frames_count += frames;
  } else if (frames <= capacity) {
    memmove(global_frames, global_frames + frames,sizeof(Frame) * (capacity - frames));
    memcpy(global_frames + (capacity - frames), bufferData, sizeof(Frame)*frames);
  } else {
    memcpy(global_frames, bufferData, sizeof(Frame)*capacity);
    global_frames_count = capacity;
  }
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
      float t = global_frames[i].left;
      if (t > 0) {
        DrawRectangle(i * cellWidth, (float)h / 4 - (float)h / 4 * t, 1, (float)h / 4 * t, BLUE);
      } else {
        DrawRectangle(i * cellWidth, h / 4, 1, (float)h / 4 * t, RED);
      }
    }
    EndDrawing();
  }
  return 0;
}
