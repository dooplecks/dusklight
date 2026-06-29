#pragma once

#include "d/d_camera.h"

namespace dusk::live_bg {

void init();
bool is_active();
void deactivate();
bool run_opening_scene_hook();
void run_scene_created_hook();
void run_scene_ready_hook();
bool operate_camera(dCamera_c* cam);

}
