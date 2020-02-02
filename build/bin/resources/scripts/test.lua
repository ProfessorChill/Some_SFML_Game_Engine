textureManager:loadTexture("oga", selfPath .. "/resources/spritesheets/oga_preview.png")

local e1 = entityHandler:addEntity(textureManager:getRef("oga"))
e1.pos.x = 60
e1.pos.y = 60

local e2 = entityHandler:addEntity(textureManager:getRef("oga"))
e2.pos.x = 90
e2.pos.y = 60

local e3 = entityHandler:addEntity(textureManager:getRef("oga"))
e3.pos.x = 120
e3.pos.y = 60

local e4 = entityHandler:addEntity(textureManager:getRef("oga"))
e4.pos.x = 150
e4.pos.y = 60

local e5 = entityHandler:addEntity(textureManager:getRef("oga"))
e5.pos.x = 180
e5.pos.y = 60

local e6 = entityHandler:addEntity(textureManager:getRef("oga"))
e6.pos.x = 210
e6.pos.y = 60

local entities = {}

--for y = 0, 100, 1 do
--	for x = 0, 100, 1 do
--		entities[(y * 100) + x] = entityHandler:addEntity(textureManager:getRef("oga"))
--		entities[(y * 100) + x].pos.x = 2 * x
--		entities[(y * 100) + x].pos.y = 2 * y
--		print(entities[(y * 100) + x].id)
--	end
--end
