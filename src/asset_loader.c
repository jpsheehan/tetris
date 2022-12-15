#include <stdlib.h>
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
} ASSET;

static int next_asset_idx;
static ASSET assets[MAX_ASSETS];

void unload_asset(ASSET *asset);

void asset_loader_init(void)
{
  next_asset_idx = 0;
  for (int i = 0; i < MAX_ASSETS; i++)
  {
    assets[i].p = NULL;
  }
}

void* asset_loader_load(ASSET_TYPE type, AssetLoaderCallback f)
{
  if (f == NULL)
    return false;
  
  if (type < 0 || type >= A_MAX)
    return false;

  if (next_asset_idx >= MAX_ASSETS) {
    safe_exit("No more room for assets", 1);
    return false;
  }
  
  ASSET* pAsset = &assets[next_asset_idx++];
  pAsset->type = type;
  pAsset->p = f();
  
  return pAsset->p;
}

bool asset_loader_unload(void *p)
{
  if (p == NULL)
    return false;

  for (int i = 0; i < MAX_ASSETS; i++)
  {
    ASSET *asset = &assets[i];
    if (asset->p == p)
    {
      unload_asset(asset);
    }
  }
  return false;
}

void asset_loader_unload_all(void)
{
  for (int i = MAX_ASSETS - 1; i >= 0; i--)
  {
    ASSET *asset = &assets[i];
    if (asset->p != NULL)
      unload_asset(asset);
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
}

void asset_loader_deinit_allegro(void)
{
  if (al_is_audio_installed()) {
    al_uninstall_audio();
  }

  if (al_is_keyboard_installed()) {
    al_uninstall_keyboard();
  }

  if (al_is_system_installed()) {
    al_uninstall_system();
  }
}