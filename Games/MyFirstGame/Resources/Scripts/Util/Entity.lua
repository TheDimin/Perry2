local params={...}
--0 Entity ID
--1 FunctionTable

if type(Entities) == "nil" then
    Entities = {}
    --TODO treat this as a class ?
end


local Entity = {
    _Entity = params[0],
    __index = require(params[1])
}

Entities[params[0]] = Entity


return Entities[params[0]]