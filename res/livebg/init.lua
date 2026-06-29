-- Dusklight live background entrypoint
-- should return a scene table or nil

local manifest = {
    { name = "aether_castle" },
    { name = "faron_spring" },
}

local eligible = {}
for _, v in pairs(manifest) do
    if not v.condition or v.condition() then
        table.insert(eligible, v.name)
    end
end

local name = eligible[math.random(#eligible)]
return require("scenes." .. name)
