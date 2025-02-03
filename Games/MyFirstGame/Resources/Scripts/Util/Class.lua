require "Util.Tables"
local Log = require "Util.Logging"

--Storage of all Class "MetaTables"


local objectMetaTable ={
    __newindex = function (table, key,value)
        if(type(rawget(table,"_Class")) == "nil")then
            return nil
        end

        local class= table._Class

        if type(table._pointer) ~= "nil" then
            if type(class.variableSet[key]) ~="nil" then
                return class.variableSet[key](table,value)
            end
        else
            print("Null object")
        end
    end,

    __index = function (table,key)
        if(type(rawget(table,"_Class")) == "nil")then
            print("_Class not found....")
            return nil
        end
    
        local class= table._Class

        if type(class.staticFunctions[key]) ~="nil" then
            return class.staticFunctions[key]
        end

        --Are we an actual class instance or just the type ?
        if type(table._pointer) ~= "nil" then
            if type(class.memberFunctions[key]) ~="nil" then
                return class.memberFunctions[key]
            end

            if type(class.variableGet[key]) ~="nil" then
                -- variableGet are special, As we need to get it from C++.. we do some magic trics
                return class.variableGet[key](table)
            end
        else
            print("Null object")
        end

        Log.Error("'"..table._Class.name.."' does not contain field "..key)

        return table._Class:__index(table,key)
    end
}

 local  Classes = {
        __newindex = function() print("Classes new index got called") end,--Don't allow new classes to be registerd (use rawset)

        types = {},

        --[[This function is caleld by the C api to register new types to lua, Please don't call from script]]
        NewType = function(self,ClassName,CRegisterCallback,typeGlobalAccesible)
            if type( self.types[ClassName]) ~= "nil" then
                Log.Error(ClassName.." Is already a class type....")
                return;
            end

            local newClass = {
                --This table Contains all member functions
                staticFunctions = {},
                memberFunctions = {},
                --This table contains all variableGet (GetAndSet...TODO only get/set)
                variableGet = {},
                variableSet = {},
                name = ClassName,

                instanceMetaTable = shallowCopy(objectMetaTable),

                __newindex = function (table, key)
                    --Not allowed to add new index on Type
                end,

                __index = function (table,key)
                   -- rPrint(table)
                    return table.staticFunctions[key]
                end
            }

            -- User can inject functions and variable getters.
            CRegisterCallback(newClass)

            setmetatable(newClass,newClass)
            rawset(self.types,ClassName,newClass)

            if typeGlobalAccesible then
                _G[ClassName] = self.types[ClassName]
            end

        end,

        CNew = function(self,ClassName,Pointer)
            
          if type(self.types[ClassName]) == "nil" then
            --Log.Error("Cnew invalid class name: "..ClassName)
            --rPrint(self.types)
            return
           -- rPrint(self.types)
          end

            Object = {
                _Class = self.types[ClassName],
                _pointer = Pointer,

                __index = function (table,key)
                    
                end
            }
            
            setmetatable(Object,Object._Class.instanceMetaTable)
            return Object
        end
    }
    setmetatable(Classes,Classes)

return Classes