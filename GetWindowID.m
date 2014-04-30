#include <Cocoa/Cocoa.h>
#include <CoreGraphics/CGWindow.h>

int main(int argc, char **argv)
{
	NSArray *windows = (NSArray *)CGWindowListCopyWindowInfo(kCGWindowListExcludeDesktopElements,kCGNullWindowID);
	for(NSDictionary *window in windows)
		if ([[window objectForKey:(NSString *)kCGWindowOwnerName] isEqualToString:[NSString stringWithUTF8String:argv[1]]])
			if ([[window objectForKey:(NSString *)kCGWindowName] rangeOfString:[NSString stringWithUTF8String:argv[2]]].location != NSNotFound)
			{
				printf("%d ", [[window objectForKey:(NSString *)kCGWindowNumber] intValue]);
				NSString *name = [window objectForKey:@"kCGWindowName" ];
				CGRect bounds;
				CGRectMakeWithDictionaryRepresentation((CFDictionaryRef)[window objectForKey:@"kCGWindowBounds"], &bounds);
				//NSLog(@" %@",NSStringFromRect(bounds)); 
				printf("%d %d %d %d\n", (int) bounds.origin.x, (int)bounds.origin.y, (int)bounds.size.width, (int)bounds.size.height);
				return 0;
			}
}
