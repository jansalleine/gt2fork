
#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>


#define	SDL_USE_NIB_FILE 1

@interface SDL_QuartzWindow : NSObject
{

}

@end


@interface SDLMain : NSObject
{
	NSString* fileToOpen;
	
	BOOL helpWindowInitialized;
	BOOL quickGuideDocumentInitialized;
	
	IBOutlet NSWindow* helpWindow;

	IBOutlet NSWindow* quickGuideWindow;
	IBOutlet PDFView* quickGuidePDFView;
}

- (IBAction) openSong:(id)sender;
- (IBAction) saveSong:(id)sender;
- (IBAction) saveSongAs:(id)sender;

- (IBAction) openInstrument:(id)sender;
- (IBAction) saveInstrument:(id)sender;
- (IBAction) saveInstrumentAs:(id)sender;

- (IBAction) commandAction:(id)sender;
- (IBAction) alternateCommandAction:(id)sender;

- (IBAction) togglePolyphonicJamMode:(id)sender;

- (IBAction) showHelp:(id)sender;
- (IBAction) showQuickGuide:(id)sender;
- (IBAction) showBuiltInHelp:(id)sender;

@end


@interface SDLApplication : NSApplication


@end


@interface NSApplication(SDL_Missing_Methods)

- (void)setAppleMenu:(NSMenu *)menu;

@end


#if SDL_USE_NIB_FILE

/* A helper category for NSString */
@interface NSString (ReplaceSubString)

- (NSString *)stringByReplacingRange:(NSRange)aRange with:(NSString *)aString;

@end

#endif

