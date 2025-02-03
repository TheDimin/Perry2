#pragma once
//*****************************************
//***			COMPONENT               ***
//*****************************************

// property name shown to the user in the inspector
#define PROP_DISPLAYNAME(NAME) .prop(p_DisplayName, std::string(NAME))
#define PROP_DESCRIPTION(NAME) .prop(p_ToolTip, std::string(NAME))

// Only used for GLM::vec3
#define PROP_COLORPICKER .prop(p_DrawAsColor)
// Information shown to the user when hovering over the element
#define PROP_TOOLTIP(TEXT) .prop(p_ToolTip, std::string(TEXT))
// Clamping the value
#define PROP_MINMAX(MIN, MAX) .prop(p_ValueMin, MIN) .prop(p_ValueMax, MAX)
// Hide this property from the inspector
#define PROP_HIDE .prop(p_HideFromInspector)
// User can see but not editor the value in the inspector
#define PROP_READONLY .prop(p_ReadOnlyInInspector)

#define PROP_DRAGSPEED(SPEED) .prop(p_SliderSpeed, SPEED)

#define PROP_NORMALIZED .prop(p_Normalized)
#define PROP_SHOWNINEDITOR .prop(p_ShownInEditor)

#define PROP_SCRIPTCALLABLE .prop(p_Scriptcallable)
#define PROP_NOT_SERIALIZABLE .prop(p_NotSerializable)

// Variadic template version

#ifdef ENABLE_MARKERS
#define _REFLECT_ADD_CONSTRUCTORS\
	public:\
	static void* operator new(size_t size);\
	template<typename ... Args>\
	static void* operator new(size_t size,const Args&&... args){return ::operator new(size,std::forward<Args>(args)...);};\
    template<typename ... Args>\
    static void* operator new(size_t size,Args&&... args){return ::operator new(size,std::forward<Args>(args)...);};\
	static void* operator new(size_t size,std::align_val_t vt,std::nothrow_t& nt){return ::operator new(size,vt,nt);};\

#else
#define _REFLECT_ADD_CONSTRUCTORS
#endif


//	static void operator delete(void* ptr);\

// Implementation details for these macros are defined in MetaReflectImplement.h
#ifdef __clang__
#define REFLECT()                 \
private:                          \
    static int InitTypeReflect(); \
    __attribute__((constructor)) static void initialize(void) { InitTypeReflect(); };
#else
#define REFLECT()\
    _REFLECT_ADD_CONSTRUCTORS \
private:                          \
    static int InitTypeReflect(); \
    static inline int id = InitTypeReflect();
#endif

#ifdef __clang__
#define REFLECT_ENUM(TYPE)                                                                \
    namespace ENUMREFLECTS::TYPE_##TYPE                                                   \
    {                                                                                     \
        int InitTypeReflect();                                                            \
        __attribute__((constructor)) static void initialize(void) { InitTypeReflect(); }; \
    }
#else
#define REFLECT_ENUM(TYPE)                 \
    namespace ENUMREFLECTS::TYPE_##TYPE    \
    {                                      \
        int InitTypeReflect();             \
        static int id = InitTypeReflect(); \
    }
#endif

#ifdef __clang__
// TODO
#define META_INIT(TYPE)                           \
    namespace TYPE_REFLECTS::TYPE_##TYPE          \
    {                                             \
        int InitTypeReflect();                    \
        static inline int id = InitTypeReflect(); \
    }
#else

#define META_INIT(TYPE)                           \
    namespace TYPE_REFLECTS::TYPE_##TYPE          \
    {                                             \
        int InitTypeReflect();                    \
        static inline int id = InitTypeReflect(); \
    }
#endif


META_INIT(void);