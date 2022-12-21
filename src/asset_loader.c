#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>

#include "asset_loader.h"
#include "utils.h"

#define MAX_ASSETS 30

typedef struct ASSET
{
  ASSET_TYPE type;
  void *p;
  char* tag;
} ASSET;

static ASSET assets[MAX_ASSETS] = {0};
static int get_next_free_slot(void);

void unload_asset(ASSET *asset);

void asset_loader_init(void)
{
  for (int i = 0; i < MAX_ASSETS; i++)
  {
    assets[i].p = NULL;
  }
}

int asset_loader_load(const char *tag, ASSET_TYPE type, AssetLoaderCallback f)
{
  if (f == NULL)
    return false;

  ASSERT_RANGE(type, 0, A_MAX, "asset type");

  int next_asset_idx = get_next_free_slot();
  if (next_asset_idx >= MAX_ASSETS)
    safe_exit("No more room for assets", 1);

  ASSET *pAsset = &assets[next_asset_idx];
  pAsset->type = type;
  pAsset->p = f();
  must_init(pAsset->p, tag);
  pAsset->tag = strdup(tag);
  must_init(pAsset->tag, "asset tag string");

  printf("Loaded [%02d] %s\n", next_asset_idx, pAsset->tag);

  return next_asset_idx;
}

void* asset_loader_get(int handle)
{
  ASSERT_RANGE(handle, 0, MAX_ASSETS, "asset handle must be in range");
  return assets[handle].p;
}

bool asset_loader_unload(int handle)
{
  ASSERT_RANGE(handle, 0, MAX_ASSETS, "asset handle");

  ASSET *pAsset= &assets[handle];

  if (pAsset == NULL)
    return false;

  printf("Unloaded [%02d] %s\n", handle, pAsset->tag);
  unload_asset(pAsset);
  return true;
}

void asset_loader_unload_all(void)
{
  for (int i = MAX_ASSETS - 1; i >= 0; i--)
  {
    ASSET *asset = &assets[i];
    if (asset->p != NULL)
      asset_loader_unload(i);
  }
}

void unload_asset(ASSET *asset)
{
  switch (asset->type)
  {
  case A_FONT:
    al_destroy_font(asset->p);
    break;

  case A_BITMAP:
    al_destroy_bitmap(asset->p);
    break;

  case A_DISPLAY:
    al_destroy_display(asset->p);
    break;

  case A_TIMER:
    al_destroy_timer(asset->p);
    break;

  case A_EVENT_QUEUE:
    al_destroy_event_queue(asset->p);
    break;

  case A_SAMPLE:
    al_destroy_sample(asset->p);
    break;

  case A_AUDIO_STREAM:
    al_destroy_audio_stream(asset->p);
    break;

  default:
    unsafe_exit("Could not unload asset with invalid type.", 1);
    break;
  }

  asset->p = NULL;

  free(asset->tag);
  asset->tag = NULL;

}

void asset_loader_deinit_allegro(void)
{
  if (al_is_audio_installed())
  {
    al_uninstall_audio();
  }

  if (al_is_keyboard_installed())
  {
    al_uninstall_keyboard();
  }

  if (al_is_system_installed())
  {
    al_uninstall_system();
  }
}

static int get_next_free_slot(void)
{
  for (int i = 0; i < MAX_ASSETS; i++)
  {
    if (assets[i].p == NULL)
      return i;
  }
  return MAX_ASSETS;
}