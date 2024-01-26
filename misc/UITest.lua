UITest = NativeScript:new()

function UITest:start()
    self.uicontext = rmlui.GetContext('main')
    if (self.uicontext == nil) then
        self.uicontext = rmlui.CreateContext('main', Vector2i:new(1280, 720), nil)
    end
    rmlui.LoadFontFace('LatoLatin-Bold.ttf')
    self.data = {}
    self.data['animal'] = 'dog'
    self.data['show_text'] = true
    self.datamodel = self.uicontext:OpenDataModel('animals', self.data)
    self.document = self.uicontext:LoadDocument('hello_world.rml')
	self.document:Show()
end

function UITest:update(deltaTime)
    self.uicontext:Update()
end

function UITest:draw_gui()
    self.uicontext:Render()
end
