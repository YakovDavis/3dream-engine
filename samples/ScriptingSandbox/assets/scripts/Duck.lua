Duck = NativeScript:new()

function Duck:init()
    self.initialized = false

    print(string.format('initialized = %s', self.initialized))    
end

function Duck:start()
    self.transform = Component:get_component(self.owner_id, ComponentType.Transform)
    self.parent_position = Vector3.new(0, 0, 0)
    self.yaw = self.transform.rotation.y
end

function Duck:update(dT)
    if not self.initialized then
        self.initialized = true

        print(string.format('initialized = %s', self.initialized))

        local rot = self.transform.rotation:to_euler()
        print(string.format("yaw = %f pitch = %f, roll = %f", rot.x, rot.y, rot.z))
    end

    -- local rotation
    self.yaw = self.yaw + dT / 100;
    self.transform.rotation = Quaternion.create_from_yaw_pitch_roll(self.yaw, 0, 0)

    -- global rotation
    local yaw_delta = dT / 1000
    local relative_position = self.parent_position + self.transform.position
    local rotation = Quaternion.create_from_yaw_pitch_roll(yaw_delta, 0, 0)
    local rotation_matrix = Martix.create_from_quaternion(rotation)
    local new_position = Vector3.transform(relative_position, rotation_matrix)
    self.transform.position = new_position
end
