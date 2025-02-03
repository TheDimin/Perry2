#pragma once
#define GLOBAL_SCOPE_LOCK() static std::mutex __FILE__##GLOCK##__LINE__ {}; std::scoped_lock __LINE__##SLOCK (__FILE__##GLOCK##__LINE__); 
#define SCOPE_LOCK(Lock) std::scoped_lock __LINE__##SLOCK (Lock); 