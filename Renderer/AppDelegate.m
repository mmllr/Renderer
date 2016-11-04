#import "AppDelegate.h"
#import "WindowController.h"

@interface AppDelegate ()

@property (weak) IBOutlet WindowController *mainWindowController;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	if (!self.mainWindowController.isWindowLoaded) {
		[self.mainWindowController loadWindow];
	}
	
	NSWindow *mainWindow = [self.mainWindowController window];
	if (![mainWindow isVisible]) {
		[self.mainWindowController showWindow:self];
	}
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
	return YES;
}

@end
