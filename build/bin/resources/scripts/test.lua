textureManager:loadTexture("oga", selfPath .. "/resources/spritesheets/oga_preview.png")

local e1 = entityHandler:addEntity(textureManager:getRef("oga"))
e1.pos.x = 60
e1.pos.y = 60

local e2 = entityHandler:addEntity(textureManager:getRef("oga"))
e2.pos.x = 60 * 2
e2.pos.y = 60

local e3 = entityHandler:addEntity(textureManager:getRef("oga"))
e3.pos.x = 60 * 3
e3.pos.y = 60

local e4 = entityHandler:addEntity(textureManager:getRef("oga"))
e4.pos.x = 60 * 4
e4.pos.y = 60

local e5 = entityHandler:addEntity(textureManager:getRef("oga"))
e5.pos.x = 60 * 5
e5.pos.y = 60

local e6 = entityHandler:addEntity(textureManager:getRef("oga"))
e6.pos.x = 60 * 6
e6.pos.y = 60

local entities = {}

local limit = -1

for y = 0, limit, 1 do
	for x = 0, limit, 1 do
		entities[(y * limit) + x] = entityHandler:addEntity(textureManager:getRef("oga"))
		entities[(y * limit) + x].pos.x = x * x
		entities[(y * limit) + x].pos.y = y * y
		print(entities[(y * limit) + x].id)
	end
end
