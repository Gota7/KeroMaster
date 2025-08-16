#include "platform.h"
#include <filesystem>
#include <objc/objc.h>
#include <objc/message.h>
#include <objc/NSObjCRuntime.h>
#include <Foundation/Foundation.h>

namespace fs = std::filesystem;

std::filesystem::path GetKMUserPath() 
{
	@autoreleasepool {
		NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
		if ([paths count] > 0) {
			NSString* appSupport = [paths objectAtIndex:0];
			NSString* keroPath = [appSupport stringByAppendingPathComponent:@"KeroMaster"];
			return fs::path([keroPath UTF8String]);
		}
	}
	throw std::runtime_error("Failed to get user path");
}