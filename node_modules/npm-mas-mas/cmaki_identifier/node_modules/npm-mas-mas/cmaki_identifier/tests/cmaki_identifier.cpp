#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <boost/predef.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#ifdef _WIN32

// problems with variadic in windows
std::string get_environment(const char* varname, const char* default_)
{
	char* varname_str = getenv(varname);
	std::string value_str; 
	if(varname_str == NULL)
		value_str = default_;
	else
		value_str = varname_str;
	return value_str;
}

#else

template <typename T>
std::string get_environment(T default_)
{
    return default_;
}

template <typename T, typename ... Args>
std::string get_environment(T varname, Args ... others)
{
	char* varname_str = getenv(varname);
	std::string value_str; 
	if(varname_str == NULL)
		value_str = get_environment(others...);
	else
		value_str = varname_str;
	return value_str;
}

#endif

int main()
{
#ifdef __EMSCRIPTEN__
	#define OPERATIVE_SYSTEM "javascript"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_WINDOWS
	#define OPERATIVE_SYSTEM "windows"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_ANDROID
	#define OPERATIVE_SYSTEM "android"
	#define OPERATIVE_RESTRICTION "_api_" STR(__ANDROID_API__)
#elif BOOST_OS_LINUX
	#define OPERATIVE_SYSTEM "linux"
	#ifdef __GLIBC__
		#define OPERATIVE_RESTRICTION "_glibc_" STR(__GLIBC__) "." STR(__GLIBC_MINOR__)
	#else
		#define OPERATIVE_RESTRICTION ""
	#endif
#elif BOOST_OS_MACOS
	#define OPERATIVE_SYSTEM "macos"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_AIX
	#define OPERATIVE_SYSTEM "aix"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_AMIGAOS
	#define OPERATIVE_SYSTEM "amigaos"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_BEOS
	#define OPERATIVE_SYSTEM "beos"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_BSD
	#if BOOST_OS_BSD_DRAGONFLY
		#define OPERATIVE_SYSTEM "dragonfly_bsd"
		#define OPERATIVE_RESTRICTION ""
	#elif BOOST_OS_BSD_FREE
		#define OPERATIVE_SYSTEM "freebsd"
		#define OPERATIVE_RESTRICTION ""
	#elif BOOST_OS_BSD_BSDI
		#define OPERATIVE_SYSTEM "bsdios"
		#define OPERATIVE_RESTRICTION ""
	#elif BOOST_OS_BSD_NET
		#define OPERATIVE_SYSTEM "netbsd"
		#define OPERATIVE_RESTRICTION ""
	#elif BOOST_OS_BSD_OPEN
		#define OPERATIVE_SYSTEM "openbsd"
		#define OPERATIVE_RESTRICTION ""
	#endif
#elif BOOST_OS_CYGWIN
	#define OPERATIVE_SYSTEM "cygwin"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_HPUX
	#define OPERATIVE_SYSTEM "hpux"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_IRIX
	#define OPERATIVE_SYSTEM "irix"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_OS400
	#define OPERATIVE_SYSTEM "os400"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_QNX
	#define OPERATIVE_SYSTEM "qnx"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_SOLARIS
	#define OPERATIVE_SYSTEM "solaris"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_UNIX
	#define OPERATIVE_SYSTEM "unix"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_SVR4
	#define OPERATIVE_SYSTEM "svr4"
	#define OPERATIVE_RESTRICTION ""
#elif BOOST_OS_VMS
	#define OPERATIVE_SYSTEM "vms"
	#define OPERATIVE_RESTRICTION ""
#else
	#define OPERATIVE_SYSTEM "unknown_so"
	#define OPERATIVE_RESTRICTION ""
#endif

#if BOOST_ARCH_X86
	#if BOOST_ARCH_X86_32
		#define ARCHITECTURE "32"
	#elif BOOST_ARCH_X86_64
		#define ARCHITECTURE "64"
	#else
		#define ARCHITECTURE "unknown_arch"
	#endif
#elif BOOST_ARCH_ARM
	#define ARCHITECTURE "arm"
#elif BOOST_ARCH_ALPHA
	#define ARCHITECTURE "alpha"
#elif BOOST_ARCH_BLACKFIN
	#define ARCHITECTURE "blackfin"
#elif BOOST_ARCH_CONVEX
	#define ARCHITECTURE "convex"
#elif BOOST_ARCH_IA64
	#define ARCHITECTURE "ia64"
#elif BOOST_ARCH_M68K
	#define ARCHITECTURE "m68k"
#elif BOOST_ARCH_MIPS
	#define ARCHITECTURE "mips"
#elif BOOST_ARCH_PARISK
	#define ARCHITECTURE "parisk"
#elif BOOST_ARCH_PPC
	#define ARCHITECTURE "ppc"
#elif BOOST_ARCH_PYRAMID
	#define ARCHITECTURE "pyramid"
#elif BOOST_ARCH_RS6000
	#define ARCHITECTURE "rs6000"
#elif BOOST_ARCH_SPARC
	#define ARCHITECTURE "sparc"
#elif BOOST_ARCH_SH
	#define ARCHITECTURE "sh"
#elif BOOST_ARCH_SYS370
	#define ARCHITECTURE "sys370"
#elif BOOST_ARCH_SYS390
	#define ARCHITECTURE "sys390"
#elif BOOST_ARCH_Z
	#define ARCHITECTURE "z"
#else
	#define ARCHITECTURE "unknown_arch"
#endif
	
#if BOOST_COMP_MSVC
	#define COMPILER "msvc"
	#if _MSC_VER == 1911
		#define COMPILER_RESTRICTION "_2017"
	#elif _MSC_VER == 1910
		#define COMPILER_RESTRICTION "_2017"
	#elif _MSC_VER == 1900
		#define COMPILER_RESTRICTION "_2015"
	#elif _MSC_VER == 1800
		#define COMPILER_RESTRICTION "_2013"
	#elif _MSC_VER == 1700
		#define COMPILER_RESTRICTION "_2012"
	#elif _MSC_VER == 1600
		#define COMPILER_RESTRICTION "_2010"
	#elif _MSC_VER == 1500
		#define COMPILER_RESTRICTION "_2008"
	#elif _MSC_VER == 1400
		#define COMPILER_RESTRICTION "_2005"
	#elif _MSC_VER == 1310
		#define COMPILER_RESTRICTION "_2003"
	#else
		#define COMPILER_RESTRICTION "_msc_ver_" STR(_MSC_VER)
	#endif
#elif BOOST_COMP_GNUC
	#define COMPILER "gcc"
	#define COMPILER_RESTRICTION "_" STR(__GNUC__)
#elif BOOST_COMP_CLANG
	#define COMPILER "clang"
	#define COMPILER_RESTRICTION "_" STR(__clang_major__)
#elif BOOST_COMP_BORLAND
	#define COMPILER "borland"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_COMO
	#define COMPILER "comeau"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_DEC
	#define COMPILER "dec"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_DIAB
	#define COMPILER "diab"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_DMC
	#define COMPILER "dmc"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_SYSC
	#define COMPILER "sysc"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_EDG
	#define COMPILER "edg"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_PATH
	#define COMPILER "path"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_GCCXML
	#define COMPILER "gccxml"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_GHS
	#define COMPILER "ghs"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_HPACC
	#define COMPILER "hpacc"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_IAR
	#define COMPILER "iar"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_IBM
	#define COMPILER "ibm"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_INTEL
	#define COMPILER "intel"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_KCC
	#define COMPILER "kcc"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_LLVM
	#define COMPILER "llvm"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_HIGHC
	#define COMPILER "highc"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_MWERKS
	#define COMPILER "mwerks"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_MRI
	#define COMPILER "mri"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_MPW
	#define COMPILER "mrw"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_PALM
	#define COMPILER "palm"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_PGI
	#define COMPILER "pgi"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_SGI
	#define COMPILER "sgi"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_SUNPRO
	#define COMPILER "sunpro"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_TENDRA
	#define COMPILER "tendra"
	#define COMPILER_RESTRICTION ""
#elif BOOST_COMP_WATCOM
	#define COMPILER "watcom"
	#define COMPILER_RESTRICTION ""
#else
	#define COMPILER "unknown_compiler"
	#define COMPILER_RESTRICTION ""
#endif
	
	// structure (3 chunks joined with "-"):
	// 1. platform (2 or 3 chunks joined with "_")
	// 	1.1. operative system (string but forbidden "_" and "-")
	//	1.2. architecture (string but forbidden "_" and "-")
	//	1.3. (optional) operative system restriction (is explanation and version joined with "_")
	//		1.3.1. what is this restriction (string but forbidden "_" and "-")
	//		1.3.2. version (1-4 chunks joined with ".") 
	// 2. compiler (1 or 2 chunks joined with "_")
	//	2.1. compiler (string but forbidden "_" and "-")
	//	2.2. (optional) compiler restriction (is version)
	//		2.2.1. version (1-4 chunks joined with ".")
	// 3. build mode (1 or 2 chunks joined with "_")
	//	3.1. build_mode (string but forbidden "_" and "-")
	//	3.2. (optional) build mode restrictions
	
	std::string build_mode = get_environment("MODE", "Debug");
	std::string cmaki_entropy = get_environment("CMAKI_ENTROPY", "");
	std::string cmaki_info = get_environment("CMAKI_INFO", "ALL");
	
	std::transform(build_mode.begin(), build_mode.end(), build_mode.begin(), ::tolower);
	std::transform(cmaki_entropy.begin(), cmaki_entropy.end(), cmaki_entropy.begin(), ::tolower);
	
	// TODO: mas consultas
	// Arquitectura, sólo el numero: 32 o 64
	// Compilador: COMPILER + COMPILER_RESTRICTION
	// Todo: OPERATIVE_SYSTEM + "_" + ARCHITECTURE + OPERATIVE_RESTRICTION + "-" + COMPILER + COMPILER_RESTRICTION + "-" + build_mode + cmaki_entropy
	if(cmaki_info == "OS")
	{
		std::cout 	<< OPERATIVE_SYSTEM 
				<< std::endl;
	}
	else if(cmaki_info == "ARCH")
	{
		std::cout 	<< ARCHITECTURE 
				<< std::endl;
	}
	else if(cmaki_info == "COMPILER")
	{
		std::cout 	<< COMPILER 
				<< COMPILER_RESTRICTION
				<< std::endl;
	}
	else // if(cmaki_info == "ALL")
	{
		std::cout 	<< OPERATIVE_SYSTEM 
				<< "_" << ARCHITECTURE 
				<< OPERATIVE_RESTRICTION 
				<< "-" << COMPILER 
				<< COMPILER_RESTRICTION
				<< "-" << build_mode;
		if(cmaki_entropy.length() > 0)
		{
			std::cout << "-" << cmaki_entropy;
		}
		std::cout << std::endl;
	}
}
