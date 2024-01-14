Goose = NativeScript:new()

function Goose:init()
    print('Goose init()')
end

function Goose:start()
    print('Goose init()')
end

function Goose:update(dT)
	print(string.format('Goose update(%d)', dT))
end