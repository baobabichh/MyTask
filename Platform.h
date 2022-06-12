#ifndef INCLUDE_PLATFORM_H
#define INCLUDE_PLATFORM_H

#define LINUX_PLATFORM

#ifdef WINDOWS_PLATFORM
#define ROOT "C:\\"
#else
#ifdef LINUX_PLATFORM
#define ROOT "/home"
#endif
#endif // WINDOWS_PLATFORM

#endif // !INCLUDE_PLATFORM_H



