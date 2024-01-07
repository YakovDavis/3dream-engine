CollisionTest = NativeScript:new()

function CollisionTest:init()
    self.initialized = false

    print(string.format('initialized = %s', self.initialized))    
end

function CollisionTest:update(dT)
    if not self.initialized then
        self.initialized = true

        print(string.format('initialized = %s', self.initialized))
    end
end

function CollisionTest:on_collision_enter(collision)
    print("Collision enter")    
    print(collision)

    e = Component:find_with_body_id(collision)
    print(e)
end

function CollisionTest:on_collision_stay(collision)
    print("Collision stay")
    print(collision)

    e = Component:find_with_body_id(collision)
    print(e)
end

function CollisionTest:on_collision_exit(collision)
    print("Collision exit")
    print(collision)

    e = Component:find_with_body_id(collision)
    print(e)
end
