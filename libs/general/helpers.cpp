#include<string>

#if defined(__linux__)
#elif defined(_Windows)
#include<windows.h>
#endif

#include"helpers.hpp"
#include"chardef/chardef_consts.hpp"
namespace handfont{
	std::string to_hex(int num){
		std::stringstream result;
		result<<std::hex<<num;
		return result.str();
	}
	std::string to_hex(int num,int width,const char fillchar){
		std::stringstream result;
		result<<std::setw(width)<<std::setfill(fillchar)<<std::hex<<num;
		return result.str();
	}
	stdfsys::path get_self_dir(){
		stdfsys::path result;
#if defined(__linux__)
		result = stdfsys::read_symlink(stdfsys::path("/proc/self/exe")).parent_path();
#elif defined(_Windows) //not tested
		char selfpath[MAX_PATH+1]
		result = stdfsys::path(GetModuleFileName(nullptr,selfpath,MAX_PATH)).parent_path();
#endif
		return result;
}
}
