NativeScript = {}
function NativeScript:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function NativeScript:init()
end

function NativeScript:start()
end

function NativeScript:update(deltaTime)
end
