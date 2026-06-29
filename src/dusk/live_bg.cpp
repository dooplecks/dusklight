#include "live_bg.hpp"
#include <thread>
#include "d/d_camera.h"
#include "dusk/logging.h"
#include "dusk/ui/ui.hpp"

#include <sol/sol.hpp>

namespace dusk::live_bg {

namespace {

bool s_active = false;
sol::state s_lua;
struct {
    sol::protected_function created;
    sol::protected_function ready;
    sol::protected_function camera;
} s_scene_lua_funcs;

}  // namespace

void init() {
    s_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);
    s_lua.script(R"(
        package.path = package.path .. ";res/livebg/?.lua"
    )");

    auto game = s_lua.create_table();
    game.set_function(
        "set_flag", [](size_t idx) { dComIfGs_onEventBit(dSv_event_flag_c::saveBitLabels[idx]); });
    game.set_function("set_time",
        [](double time) { dComIfGs_getSaveData()->getPlayer().getPlayerStatusB().setTime(time); });
    s_lua["game"] = game;

    auto sound = s_lua.create_table();
    sound.set_function("start", [](sol::table params) {
        const u32 id = params["id"];
        const sol::optional<u32> seek = params["seek"];
        std::thread t([=] {
            JAISoundHandle handle;
            Z2GetAudioMgr()->startSound(id, &handle, nullptr);
            if (seek) {
                // HACK: the game's audio APIs provide no mechanism for seeking
                // in audio streams, so we resort to this absolute nonsense to
                // simulate it. at the loop point, you will be subjected to an
                // ear-destroying noise. this is mainly for the faron_spring.lua demo.
                auto strm = &handle->asStream()->inner_.aramStream_;
                while (strm->mBufCount == 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
                strm->mBlock = *seek;
                strm->mpLasts[0] = 124;
                strm->mpPenults[0] = 151;
                strm->mpLasts[1] = -217;
                strm->mpPenults[1] = -146;
            }
        });
        t.detach();
    });
    s_lua["sound"] = sound;
}

bool is_active() {
    return s_active;
}

void deactivate() {
    s_active = false;
}

void run_scene_created_hook() {
    if (s_active && s_scene_lua_funcs.created.valid()) {
        s_scene_lua_funcs.created();
    }
}

void run_scene_ready_hook() {
    // BUG: this runs a bit too late, well after the scene is visible (see note inside)
    if (s_active) {
        if (s_scene_lua_funcs.ready.valid()) {
            s_scene_lua_funcs.ready();
        }

        const auto player = dComIfGp_getLinkPlayer();
        if (player) {
            // NOTE: the game unpauses every actor *twice* at separate times during the fade-in.
            // if this function runs too early, the next line doesn't really do anything.
            fpcM_PauseEnable(player, 3);
        }

        // HACK: in typical TP fashion, this is normally set by `dScnKy_env_light_c::setSunpos`,
        // *except* if the current stage is F_SP200. this is the stage used for the
        // `aether_castle.lua` scene, so loading it first will place the moon at the origin.
        g_env_light.moon_pos = cXyz{0.0f, 80000.0f, 0.0f};
    }
}

bool operate_camera(dCamera_c* cam) {
    if (s_active && s_scene_lua_funcs.camera.valid()) {
        const auto t = cam->mFrameCounter / 30.0;
        const sol::protected_function_result result =
            s_scene_lua_funcs.camera(t, mDoGph_gInf_c::getAspect());

        if (result.valid()) {
            const sol::table data = result;
            const sol::table eye = data["eye"];
            const sol::table center = data["center"];
            cam->mEye = cXyz{eye["x"], eye["y"], eye["z"]};
            cam->mCenter = cXyz{center["x"], center["y"], center["z"]};
            cam->mFovy = data["fov"];
            // TODO: cam reset?

            return true;
        }
    }
    return false;
}

bool run_opening_scene_hook() {
    static bool first_run = true;

    if (first_run && dusk::ui::is_prelaunch_open()) {
        const auto result = s_lua.safe_script_file("res/livebg/init.lua");

        if (result.valid() && result.get_type() == sol::type::table) {
            const sol::table scene = result;
            const sol::table stage = scene["stage"];
            s_scene_lua_funcs.created = scene["created"];
            s_scene_lua_funcs.ready = scene["ready"];
            s_scene_lua_funcs.camera = scene["camera"];

            const std::string stage_name = stage["name"];
            dComIfGp_setNextStage(
                stage_name.c_str(), stage["point"], stage["room"], stage["layer"]);
            s_active = true;
        } else {
            DuskLog.warn("livebg init script did not return scene table");
        }
    }

    first_run = false;
    return s_active;
}

}  // namespace dusk::live_bg
