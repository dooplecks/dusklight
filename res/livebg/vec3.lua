local Vec3 = {}
Vec3.__index = Vec3

function Vec3.new(x, y, z)
  local self = setmetatable({}, Vec3)
  self.x = tonumber(x) or 0
  self.y = tonumber(y) or 0
  self.z = tonumber(z) or 0
  return self
end

function Vec3.__add(v1, v2)
  return Vec3.new(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z)
end

function Vec3.__sub(v1, v2)
  return Vec3.new(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z)
end

function Vec3.__mul(a, b)
  if type(a) == "number" then
    return Vec3.new(b.x * a, b.y * a, b.z * a)
  elseif type(b) == "number" then
    return Vec3.new(a.x * b, a.y * b, a.z * b)
  else
    return Vec3.new(a.x * b.x, a.y * b.y, a.z * b.z)
  end
end

function Vec3.__unm(v)
  return Vec3.new(-v.x, -v.y, -v.z)
end

function Vec3:rotate_y(theta)
  local ct = math.cos(theta)
  local st = math.sin(theta)
  return Vec3.new(
    self.x * ct - self.z * st,
    self.y,
    self.x * st + self.z * ct
  )
end

return Vec3
