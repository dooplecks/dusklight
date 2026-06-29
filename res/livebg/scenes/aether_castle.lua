local Vec3 = require "vec3"

local function calc_fovx(fovy, ar)
  return 2.0 * math.atan(math.tan(fovy * 0.5) * ar)
end

local scene = {
  stage = { name = "F_SP200", point = 0, room = 0, layer = -1 }
}

local target = Vec3.new(2120.0, 2000.0, -6190.0)
local offset = Vec3.new(0.0, 4000.0, 15000.0)
local fovy = 30.0

function scene.camera(t, ar)
  local fovx = calc_fovx(math.rad(fovy), ar)
  local theta = t / 30.0 * math.pi

  local eye = target + offset:rotate_y(theta)
  local center = eye + (-offset):rotate_y(theta - fovx * 0.25)

  return { eye = eye, center = center, fov = fovy }
end

return scene
