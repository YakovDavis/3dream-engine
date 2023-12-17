Duck = NativeScript:new(nil)

function Duck:init()
    self.a = 1
    self.b = 2
    self.initialized = false

    print(string.format('a = %d', self.a))
    print(string.format('b = %d', self.b))
    print(string.format('initialized = %s', self.initialized))

    self.c = self.a + self.b
end

function Duck:update(dT)
    -- transform.position.x = transform.position.x + deltaTime / 1000
    if not self.initialized then
        self.initialized = true

        print(string.format("c = a + b: %d", self.c))
        print(string.format('initialized = %s', self.initialized))
    end
end
