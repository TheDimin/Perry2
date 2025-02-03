#pragma once

#ifdef MEM_DEBUG
#define IMPLEMENT_MEM_DEBUG(TYPE) \
	void* operator new(size_t size){\
		void* object = ::operator new(size);ImHexMemTracker::Mark(object,#TYPE,size); return object;}\
	template<typename ... Args>\
	static void* operator new(size_t size,const Args&&... args){\
		void* object = ::operator new(size,std::forward<Args>(args)...);ImHexMemTracker::Mark(object,#TYPE,size); return object;}\
    template<typename ... Args>\
    static void* operator new(size_t size,Args&&... args){\
		void* object = ::operator new(size,std::forward<Args>(args)...);ImHexMemTracker::Mark(object,#TYPE,size); return object;};\
	static void* operator new(size_t size,std::align_val_t vt,std::nothrow_t& nt){\
		void* object = ::operator new(size,vt,nt);ImHexMemTracker::Mark(object,#TYPE,size); return object;};\
	static void* operator new[](std::size_t count){\
		void* object = ::operator new(count); ImHexMemTracker::Mark(object,#TYPE,count*sizeof(TYPE));return object;}\
	void operator delete(void* ptr){ ImHexMemTracker::Forget(ptr);::operator delete(ptr); }

#else
#define IMPLEMENT_MEM_DEBUG(TYPE)
#endif


 class ImHexMemTracker* GetTracker();

class ImHexMemTracker
{
public:
	struct markerInfo
	{
		std::string name;
		std::size_t size;
	};

	static void Mark(void* object,const char* name, std::size_t size);

	static void Forget(void* object);

	static std::string Dump();

	std::map<void*, markerInfo> markers;
};