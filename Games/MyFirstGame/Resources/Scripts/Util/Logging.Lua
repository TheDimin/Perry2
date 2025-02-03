--This api Will most likely be moved to C++ to intergrate it into a Console window


return {

    Warning = function (message)
        print("[LUA][WARNING]: "..message)
    end,

    Error = function (message)
        print("[LUA][ERROR]: "..message)
    end,

    Debug = function (message)
        --This we don't care about often so we can disable it here :)
        print("[LUA][Debug]: "..message)
    end
}


