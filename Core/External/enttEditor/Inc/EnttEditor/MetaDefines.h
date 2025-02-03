#pragma once
#include <entt/entt.hpp>

// When this is defined we ignore p_Internal
// #define ShowInternal

using entt::literals::operator""_hs;

constexpr entt::hashed_string T_ReflectedObject = "ReflectedObject"_hs;
constexpr entt::hashed_string T_ReflectedComponent = "ReflectedComponent"_hs;
constexpr entt::hashed_string T_ReflectedEnum = "ReflectedEnum"_hs;

constexpr entt::hashed_string p_DisplayName = "DisplayName"_hs;

// This metaFunction is used to draw element types (int,float,vec3..ect)
// When used on classes its considerd that user handles the method of drawing this class
constexpr entt::hashed_string f_Inspect = "Inspect"_hs;

constexpr entt::hashed_string f_AddComponent = "AddComponent"_hs;
constexpr entt::hashed_string f_TryAddComponent = "TryAddComponent"_hs;
constexpr entt::hashed_string f_PatchComponent = "PatchComponentn"_hs;
constexpr entt::hashed_string f_RemoveComponent = "RemoveComponent"_hs;
constexpr entt::hashed_string f_ContainsComponent = "ContainsComponent"_hs;

// PropertyDefines
constexpr entt::hashed_string p_HideFromInspector = "HideFromInspector"_hs;
constexpr entt::hashed_string p_ToolTip = "ToolTip"_hs;
constexpr entt::hashed_string p_ReadOnlyInInspector = "ReadOnly"_hs;
constexpr entt::hashed_string p_Normalized = "Normalized"_hs;
// For glm::Vec3/4 draw it as color picker
constexpr entt::hashed_string p_DrawAsColor = "drawAsColor"_hs;

constexpr entt::hashed_string p_ValueMin = "ValueMin"_hs;
constexpr entt::hashed_string p_ValueMax = "ValueMax"_hs;
constexpr entt::hashed_string p_SliderSpeed = "sliderSpeed"_hs;

// This function is shown in the Editor as a button
constexpr entt::hashed_string p_ShownInEditor = "showInEditor"_hs;

// Internals
// If a anything that is drawn is marked with this, Reflect will ignore it under all cases.
// Intend is that we do something special do with this and we want the systems to ignore it.)
// For instance we have some components that we create behind the scene and users should never know about or are shown in a
// custom manner (Namecomponent or HierarchyComponent)
constexpr entt::hashed_string p_Internal = "Internal"_hs;


constexpr entt::hashed_string p_Scriptcallable = "PScriptCallable"_hs;
constexpr entt::hashed_string p_NotSerializable = "pNOSerialize"_hs;
constexpr entt::hashed_string p_SaveRevision = "pRevisionNumber"_hs;
constexpr entt::hashed_string p_Scriptable  = "PScriptable"_hs;//TODO(DAMIAN) what is difference between this and `p_Scriptcallable`?
constexpr entt::hashed_string p_LuaHiddenType  = "PLuaHIdden"_hs; // shouldn't anything that is not scriptcallable/scriptable be hidden already ??

constexpr entt::hashed_string p_LuaPull= "LuaPush"_hs;
constexpr entt::hashed_string p_LuaPush= "LuaPull"_hs;

constexpr entt::hashed_string f_AddComponentSelf = "f_AddComponentSelf"_hs;
constexpr entt::hashed_string f_Serialize = "f_Serialize"_hs;
constexpr entt::hashed_string f_Deserialize = "f_Deserialize"_hs;
