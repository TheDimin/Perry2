#pragma once

// Define a macro to enable/disable timing code based on DEBUG mode


#define ENABLE_TIMING

// TIMER MACRO
#ifdef ENABLE_TIMING
#define START_TIMER(func_name) auto start_time_##func_name = std::chrono::high_resolution_clock::now()
#else
#define START_TIMER(func_name) 
#endif

#ifdef ENABLE_TIMING
#define END_TIMER(func_name) \
        do { \
	        auto end_time_##func_name = std::chrono::high_resolution_clock::now(); \
            auto duration_##func_name = std::chrono::duration<float>(end_time_##func_name - start_time_##func_name); \
 			printf("%s - %.3f ms \n", #func_name, duration_##func_name.count() * 1000.f); \
        } while (false)

#define END_TIMER_MSG(func_name, msg, ...) \
        do { \
	        auto end_time_##func_name = std::chrono::high_resolution_clock::now(); \
            auto duration_##func_name = std::chrono::duration<float>(end_time_##func_name - start_time_##func_name); \
			printf(msg, __VA_ARGS__); \
 			printf(" - %.3f ms \n", duration_##func_name.count() * 1000.f); \
        } while (false)

#else
#define END_TIMER(func_name)
#define END_TIMER_MSG(func_name, msg, ...)
#endif
// END TIMER MACRO