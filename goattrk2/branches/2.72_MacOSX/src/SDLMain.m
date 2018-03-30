
#import <SDL/SDL.h>
#import "SDLMain.h"
#import <sys/param.h> /* for MAXPATHLEN */
#import <unistd.h>

#import "goattrk2.h"

static int    gArgc;
static char** gArgv;
static BOOL   gFinderLaunch;


#if !SDL_USE_NIB_FILE

static NSString *getApplicationName(void)
{
    NSDictionary *dict;
    NSString *appName = 0;
	
    // Determine the application name
    dict = (NSDictionary *)CFBundleGetInfoDictionary(CFBundleGetMainBundle());
    if (dict)
        appName = [dict objectForKey: @"CFBundleName"];
    
    if (![appName length])
        appName = [[NSProcessInfo processInfo] processName];
	
    return appName;
}

#endif



@implementation SDLApplication


/* Invoked from the Quit menu item */
// ----------------------------------------------------------------------------
- (void) terminate:(id)sender
// ----------------------------------------------------------------------------
{
	NSAlert* alert = [NSAlert alertWithMessageText:@"Do you really want to quit GoatTracker?"
									 defaultButton:@"Don't Quit"
								   alternateButton:@"Quit"
									   otherButton:nil
						 informativeTextWithFormat:@"Unsaved song and instrument data might be lost."];
	
	NSArray* buttons = [alert buttons];
	NSEnumerator* enumerator = [buttons objectEnumerator];
	NSButton* button = nil;
	while ((button = [enumerator nextObject]))
	{
		if ([button tag] == NSAlertAlternateReturn)
		{
			[button setKeyEquivalent:@"q"];
			[button setKeyEquivalentModifierMask:NSCommandKeyMask];
		}
		else if ([button tag] == NSAlertDefaultReturn)
		{
			[button setKeyEquivalent:@"d"];
			[button setKeyEquivalentModifierMask:NSCommandKeyMask];
		}
	}
	
	int result = [alert runModal];
	
	if (result == NSAlertDefaultReturn)
		return;
    
	[[NSUserDefaults standardUserDefaults] synchronize];
	
	SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}



// ----------------------------------------------------------------------------
- (NSEvent *)nextEventMatchingMask:(unsigned int)mask untilDate:(NSDate *)expiration inMode:(NSString *)mode dequeue:(BOOL)flag
// ----------------------------------------------------------------------------
{
	NSEvent* event = [super nextEventMatchingMask:mask untilDate:expiration inMode:mode dequeue:flag];
	
	if (mode != NSDefaultRunLoopMode)
		return event;
	
	NSWindow* window = [event window];
	//if (window != nil)
    //	NSLog(@"event window: %@\n", window);
		
	if ([event type] == NSKeyDown)
	{
		virtualkeycode = [event keyCode];
		//NSLog(@"event key: %d for character: %@\n", [event keyCode], [event characters]);
	}
	else if ([event type] == NSKeyDown)
		virtualkeycode = 0xff;
		
	if (window == nil || ([event modifierFlags] & NSCommandKeyMask) || ([window class] != [SDL_QuartzWindow class]))
	{
		[self sendEvent:event];
		return nil;
	}
	else
		return event;
}

@end



/* The main class of the application, the application's delegate */
@implementation SDLMain

/* Set the working directory to the .app's parent directory */
// ----------------------------------------------------------------------------
- (void) setupWorkingDirectory:(BOOL)shouldChdir
// ----------------------------------------------------------------------------
{
    if (shouldChdir)
    {
        char parentdir[MAXPATHLEN];
		CFURLRef url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
		CFURLRef url2 = CFURLCreateCopyDeletingLastPathComponent(0, url);
		if (CFURLGetFileSystemRepresentation(url2, true, (UInt8 *)parentdir, MAXPATHLEN)) {
	        assert ( chdir (parentdir) == 0 );   /* chdir to the binary app's parent */
		}
		CFRelease(url);
		CFRelease(url2);
	}

}

#if SDL_USE_NIB_FILE


// ----------------------------------------------------------------------------
- (void) awakeFromNib
// ----------------------------------------------------------------------------
{
	fileToOpen = nil;
	quickGuideDocumentInitialized = NO;
	helpWindowInitialized = NO;
}


/* Fix menu to contain the real app name instead of "SDL App" */
// ----------------------------------------------------------------------------
- (void)fixMenu:(NSMenu *)aMenu withAppName:(NSString *)appName
// ----------------------------------------------------------------------------
{
    NSRange aRange;
    NSEnumerator *enumerator;
    NSMenuItem *menuItem;

    aRange = [[aMenu title] rangeOfString:@"SDL App"];
    if (aRange.length != 0)
        [aMenu setTitle: [[aMenu title] stringByReplacingRange:aRange with:appName]];

    enumerator = [[aMenu itemArray] objectEnumerator];
    while ((menuItem = [enumerator nextObject]))
    {
        aRange = [[menuItem title] rangeOfString:@"SDL App"];
        if (aRange.length != 0)
            [menuItem setTitle: [[menuItem title] stringByReplacingRange:aRange with:appName]];
        if ([menuItem hasSubmenu])
            [self fixMenu:[menuItem submenu] withAppName:appName];
    }
    [ aMenu sizeToFit ];
}


// ----------------------------------------------------------------------------
- (IBAction) openSong:(id)sender
// ----------------------------------------------------------------------------
{
	NSOpenPanel* openPanel = [NSOpenPanel openPanel];
	[openPanel setTitle:@"Open Song"];
	NSArray* fileTypes = [NSArray arrayWithObject:@"sng"];
	
	int result = [openPanel runModalForDirectory:nil file:nil types:fileTypes];
	
	if (result == NSOKButton)
	{
        NSArray* filesToOpen = [openPanel filenames];
        NSString* path = [filesToOpen objectAtIndex:0];
		NSString* file = [path lastPathComponent];
		path = [path stringByDeletingLastPathComponent];
		
		[file getCString:songfilename maxLength:MAX_FILENAME encoding:NSUTF8StringEncoding];
		[path getCString:songpath maxLength:MAX_FILENAME encoding:NSUTF8StringEncoding];
		chdir(songpath);
		
		loadsong();
    }
}


// ----------------------------------------------------------------------------
- (IBAction) saveSong:(id)sender
// ----------------------------------------------------------------------------
{
	if (strlen(songfilename) == 0)
		[self saveSongAs:sender];
	else
		savesong();
}


// ----------------------------------------------------------------------------
- (IBAction) saveSongAs:(id)sender
// ----------------------------------------------------------------------------
{
	NSString* file = [NSString stringWithCString:songfilename encoding:NSUTF8StringEncoding];
	
	NSSavePanel* savePanel = [NSSavePanel savePanel];
	[savePanel setTitle:@"Save Song"];
	[savePanel setRequiredFileType:@"sng"];
	[savePanel setCanSelectHiddenExtension:YES];
	
	int result = [savePanel runModalForDirectory:nil file:file];
	
	if (result == NSOKButton)
	{
        NSString* path = [savePanel filename];
		NSString* file = [path lastPathComponent];
		path = [path stringByDeletingLastPathComponent];
		
		[file getCString:songfilename maxLength:MAX_FILENAME encoding:NSUTF8StringEncoding];
		[path getCString:songpath maxLength:MAX_FILENAME encoding:NSUTF8StringEncoding];
		chdir(songpath);
		
		savesong();
    }
}


// ----------------------------------------------------------------------------
- (IBAction) openInstrument:(id)sender
// ----------------------------------------------------------------------------
{
	NSOpenPanel* openPanel = [NSOpenPanel openPanel];
	[openPanel setTitle:@"Open Instrument"];
	NSArray* fileTypes = [NSArray arrayWithObject:@"ins"];
	
	int result = [openPanel runModalForDirectory:nil file:nil types:fileTypes];
	
	if (result == NSOKButton)
	{
        NSArray* filesToOpen = [openPanel filenames];
		NSString* path = [filesToOpen objectAtIndex:0];
		NSString* file = [path lastPathComponent];
		path = [path stringByDeletingLastPathComponent];
		
		[file getCString:instrfilename maxLength:MAX_FILENAME encoding:NSUTF8StringEncoding];
		[path getCString:instrpath maxLength:MAX_FILENAME encoding:NSUTF8StringEncoding];
		chdir(instrpath);
		
		loadinstrument();
    }
}


// ----------------------------------------------------------------------------
- (IBAction) saveInstrument:(id)sender
// ----------------------------------------------------------------------------
{
	if (strlen(instrfilename) == 0)
		[self saveInstrumentAs:sender];
	else
		saveinstrument();
	
}


// ----------------------------------------------------------------------------
- (IBAction) saveInstrumentAs:(id)sender
// ----------------------------------------------------------------------------
{
	NSString* file = [NSString stringWithCString:instrfilename encoding:NSUTF8StringEncoding];
	
	NSSavePanel* savePanel = [NSSavePanel savePanel];
	[savePanel setTitle:@"Save Instrument"];
	[savePanel setRequiredFileType:@"ins"];
	[savePanel setCanSelectHiddenExtension:YES];
	
	int result = [savePanel runModalForDirectory:nil file:file];
	
	if (result == NSOKButton)
	{
        NSString* path = [savePanel filename];
		NSString* file = [path lastPathComponent];
		path = [path stringByDeletingLastPathComponent];
		
		[file getCString:instrfilename maxLength:MAX_FILENAME encoding:NSUTF8StringEncoding];
		[path getCString:instrpath maxLength:MAX_FILENAME encoding:NSUTF8StringEncoding];
		chdir(instrpath);
		
		saveinstrument();
    }
}


// ----------------------------------------------------------------------------
- (IBAction) commandAction:(id)sender
// ----------------------------------------------------------------------------
{
	SDL_Event event;
    event.type = SDL_KEYDOWN;
	event.key.keysym.unicode = 0;
	
	switch ([sender tag])
	{
		case 1:
			event.key.keysym.sym = KEY_F1;
			break;
		case 2:
			event.key.keysym.sym = KEY_F2;
			break;
		case 3:
			event.key.keysym.sym = KEY_F3;
			break;
		case 4:
			event.key.keysym.sym = KEY_F4;
			break;
		case 5:
			event.key.keysym.sym = KEY_F5;
			break;
		case 6:
			event.key.keysym.sym = KEY_F6;
			break;
		case 7:
			event.key.keysym.sym = KEY_F7;
			break;
		case 8:
			event.key.keysym.sym = KEY_F8;
			break;
		case 9:
			event.key.keysym.sym = KEY_F9;
			break;
	}

    SDL_PushEvent(&event);
}


// ----------------------------------------------------------------------------
- (IBAction) alternateCommandAction:(id)sender
// ----------------------------------------------------------------------------
{
	switch ([sender tag])
	{
		case 1:
			initsong(esnum, PLAY_BEGINNING);
			followplay = 1;
			break;
		case 2:
			initsong(esnum, PLAY_POS);
			followplay = 1;
			break;
		case 3:
			initsong(esnum, PLAY_PATTERN);
			followplay = 1;
			break;
	}
}


// ----------------------------------------------------------------------------
- (IBAction) togglePolyphonicJamMode:(id)sender
// ----------------------------------------------------------------------------
{
	SetPolyphonicMode(!GetPolyphonicMode());
	
	[sender setState:GetPolyphonicMode() ? NSOnState : NSOffState];
}


// ----------------------------------------------------------------------------
- (IBAction) showHelp:(id)sender
// ----------------------------------------------------------------------------
{
	if ([helpWindow isVisible])
		[helpWindow orderOut:self];
	else
		[helpWindow orderFront:self];
}


// ----------------------------------------------------------------------------
- (IBAction) showQuickGuide:(id)sender
// ----------------------------------------------------------------------------
{
	if (!quickGuideDocumentInitialized)
	{
		NSString* quickGuidePath = [NSString stringWithFormat:@"%@%@", [[NSBundle mainBundle] resourcePath], @"/goat_tracker_commands.pdf"];
		PDFDocument* document = [[PDFDocument alloc] initWithURL:[NSURL fileURLWithPath:quickGuidePath]];
		[quickGuidePDFView setDocument:document];
		quickGuideDocumentInitialized = YES;
	}
	
	if ([quickGuideWindow isVisible])
		[quickGuideWindow orderOut:self];
	else
		[quickGuideWindow orderFront:self];
}


// ----------------------------------------------------------------------------
- (IBAction) showBuiltInHelp:(id)sender
// ----------------------------------------------------------------------------
{
	onlinehelp(0, 0);
}


#else

// ----------------------------------------------------------------------------
static void setApplicationMenu(void)
// ----------------------------------------------------------------------------
{
    /* warning: this code is very odd */
    NSMenu *appleMenu;
    NSMenuItem *menuItem;
    NSString *title;
    NSString *appName;
    
    appName = getApplicationName();
    appleMenu = [[NSMenu alloc] initWithTitle:@""];
    
    /* Add menu items */
    title = [@"About " stringByAppendingString:appName];
    [appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];

    [appleMenu addItem:[NSMenuItem separatorItem]];

    title = [@"Hide " stringByAppendingString:appName];
    [appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];

    menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
    [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

    [appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];

    [appleMenu addItem:[NSMenuItem separatorItem]];

    title = [@"Quit " stringByAppendingString:appName];
    [appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];

    
    /* Put menu into the menubar */
    menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:appleMenu];
    [[NSApp mainMenu] addItem:menuItem];

    /* Tell the application object that this is now the application menu */
    [NSApp setAppleMenu:appleMenu];

    /* Finally give up our references to the objects */
    [appleMenu release];
    [menuItem release];
}


/* Create a window menu */
// ----------------------------------------------------------------------------
static void setupWindowMenu(void)
// ----------------------------------------------------------------------------
{
    NSMenu      *windowMenu;
    NSMenuItem  *windowMenuItem;
    NSMenuItem  *menuItem;

    windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    
    /* "Minimize" item */
    menuItem = [[NSMenuItem alloc] initWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
    [windowMenu addItem:menuItem];
    [menuItem release];
    
    /* Put menu into the menubar */
    windowMenuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
    [windowMenuItem setSubmenu:windowMenu];
    [[NSApp mainMenu] addItem:windowMenuItem];
    
    /* Tell the application object that this is now the window menu */
    [NSApp setWindowsMenu:windowMenu];

    /* Finally give up our references to the objects */
    [windowMenu release];
    [windowMenuItem release];
}


#endif


// ----------------------------------------------------------------------------
- (BOOL) application:(NSApplication *)theApplication openFile:(NSString *)filename
// ----------------------------------------------------------------------------
{
	fileToOpen = filename;
	[fileToOpen retain];

	NSString* file = [filename lastPathComponent];
	NSString* path = [filename stringByDeletingLastPathComponent];
	
	[file getCString:songfilename maxLength:MAX_FILENAME encoding:NSUTF8StringEncoding];
	[path getCString:songpath maxLength:MAX_FILENAME encoding:NSUTF8StringEncoding];
	chdir(songpath);
	
	loadsong();
	
	return YES;
}


/* Called when the internal event loop has just started running */
// ----------------------------------------------------------------------------
- (void) applicationDidFinishLaunching: (NSNotification *) note
// ----------------------------------------------------------------------------
{
    int status;

    /* Set the working directory to the .app's parent directory */
	[self setupWorkingDirectory:gFinderLaunch];

	if (gFinderLaunch && fileToOpen != nil)
	{
		char* filename = malloc(MAX_PATHNAME);
		[fileToOpen getCString:filename maxLength:MAX_PATHNAME encoding:NSUTF8StringEncoding];
		gArgv[1] = filename;
		gArgc = 2;
	}
	
    /* Hand off to main application code */
    status = SDL_main(gArgc, gArgv);

    /* We're done, thank you for playing */
    exit(status);
}

@end


@implementation NSString (ReplaceSubString)

// ----------------------------------------------------------------------------
- (NSString *)stringByReplacingRange:(NSRange)aRange with:(NSString *)aString
// ----------------------------------------------------------------------------
{
    unsigned int bufferSize;
    unsigned int selfLen = [self length];
    unsigned int aStringLen = [aString length];
    unichar *buffer;
    NSRange localRange;
    NSString *result;

    bufferSize = selfLen + aStringLen - aRange.length;
    buffer = NSAllocateMemoryPages(bufferSize*sizeof(unichar));
    
    /* Get first part into buffer */
    localRange.location = 0;
    localRange.length = aRange.location;
    [self getCharacters:buffer range:localRange];
    
    /* Get middle part into buffer */
    localRange.location = 0;
    localRange.length = aStringLen;
    [aString getCharacters:(buffer+aRange.location) range:localRange];
     
    /* Get last part into buffer */
    localRange.location = aRange.location + aRange.length;
    localRange.length = selfLen - localRange.location;
    [self getCharacters:(buffer+aRange.location+aStringLen) range:localRange];
    
    /* Build output string */
    result = [NSString stringWithCharacters:buffer length:bufferSize];
    
    NSDeallocateMemoryPages(buffer, bufferSize);
    
    return result;
}

@end



#ifdef main
#  undef main
#endif


/* Main entry point to executable - should *not* be SDL_main! */
// ----------------------------------------------------------------------------
int main(int argc, char **argv)
// ----------------------------------------------------------------------------
{
	//setenv("SDL_ENABLEAPPEVENTS", "1", 1);
	
    /* Copy the arguments into a global variable */
    /* This is passed if we are launched by double-clicking */
    if ( argc >= 2 && strncmp (argv[1], "-psn", 4) == 0 ) {
        gArgv = (char **) SDL_malloc(sizeof (char *) * 2);
        gArgv[0] = argv[0];
        gArgv[1] = NULL;
        gArgc = 1;
        gFinderLaunch = YES;
    } else {
        int i;
        gArgc = argc;
        gArgv = (char **) SDL_malloc(sizeof (char *) * (argc+1));
        for (i = 0; i <= argc; i++)
            gArgv[i] = argv[i];
        gFinderLaunch = NO;
    }

    [SDLApplication poseAsClass:[NSApplication class]];
    NSApplicationMain(argc, (const char**) argv);
    return 0;
}

