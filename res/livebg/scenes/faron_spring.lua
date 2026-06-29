local Vec3 = require "vec3"

local scene = {
  stage = { name = "F_SP108", point = 0, room = 1, layer = 3 }
}

function scene.created()
  -- BUG: this will trigger the Cave of Ordeals achievement
  game.set_flag(502) -- F_0502 - spawns fairies
  game.set_flag(505) -- F_0505 - spawns spirit
  game.set_time(285.0) -- set time to 19:00

  -- BUG: see note in live_bg.cpp
  sound.start { id = 0x02000018, seek = 90 } -- Z2STRM_DEMO09_00
end

local home = Vec3.new(-17300.0, 30.0, -4400.0)
local target = Vec3.new(-19300.0, 240.0, -6500.0)
local target_to_home = home - target

function scene.camera(t)
  local amt = math.sin(t / 30.0 * math.pi) * math.pi / 45.0
  local eye = target + target_to_home:rotate_y(amt)
  return { eye = eye, center = target, fov = 36.0 }
end

return scene
