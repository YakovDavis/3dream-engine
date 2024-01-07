NativeScript = {}
function NativeScript:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    self.owner_id = -1
    return o
end

function NativeScript:init()
end

function NativeScript:start()
end

function NativeScript:update(deltaTime)
end

function NativeScript:on_collision_enter(collision)
end

function NativeScript:on_collision_stay(collision)
end

function NativeScript:on_collision_exit(collision)
end

function error_handler(message)
    return "Lua Error: " .. message
end
