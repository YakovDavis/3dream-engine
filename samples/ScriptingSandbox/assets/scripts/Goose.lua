Goose = NativeScript:new()

function Goose:init()
    print('Goose init()')
end

function Goose:start()
    print('Goose start()')
end

function Goose:update(dT)
	print(string.format('Goose update(%f)', dT))
end