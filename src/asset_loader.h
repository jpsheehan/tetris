#ifndef ASSET_LOADER_H_
#define ASSET_LOADER_H_

#include <stdbool.h>

typedef enum ASSET_TYPE {
  A_FONT,
  A_DISPLAY,
  A_TIMER,
  A_BITMAP,
  A_EVENT_QUEUE,
  A_SAMPLE,
  A_AUDIO_STREAM,
  A_MAX,
} ASSET_TYPE;

typedef void* (*AssetLoaderCallback)(void);

void asset_loader_init(void);
int asset_loader_load(const char* tag, ASSET_TYPE type, AssetLoaderCallback f);
bool asset_loader_unload(int id);
void asset_loader_unload_all(void);
void asset_loader_deinit_allegro(void);
void* asset_loader_get(int id);

#define A(id) asset_loader_get(id)

#endif