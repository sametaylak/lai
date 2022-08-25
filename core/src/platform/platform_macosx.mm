// clang-format Language: Cpp

#include "platform/platform.h"
#include "base/log.h"

#ifdef LAI_PLATFORM_MACOSX

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <mach/mach_time.h>
#include <crt_externs.h>

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

@class ApplicationDelegate;
@class WindowDelegate;
@class ContentView;

struct internal_state {
  ApplicationDelegate* app_delegate;
  WindowDelegate* window_delegate;
  NSWindow* window;
  ContentView* view;
  CAMetalLayer* layer;
  bool quit_flagged;
};

@interface WindowDelegate : NSObject <NSWindowDelegate> {
    internal_state* state;
}

- (instancetype)initWithState:(internal_state*)init_state;

@end // WindowDelegate

@implementation WindowDelegate

- (instancetype)initWithState:(internal_state*)init_state {
    self = [super init];
    
    if (self != nil) {
        state = init_state;
        state->quit_flagged = false;
    }
    
    return self;
}

- (BOOL)windowShouldClose:(id)sender {
    state->quit_flagged = true;

    // event_context data = {};
    // event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

    return YES;
}

- (void)windowDidResize:(NSNotification *)notification {
    // TODO: handle resize
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
    // TODO: handle minimize

    [state->window miniaturize:nil];
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
    // TODO: handle de-minimize

    [state->window deminiaturize:nil];
}

@end // WindowDelegate

@interface ContentView : NSView <NSTextInputClient> {
    NSWindow* window;
    NSTrackingArea* trackingArea;
    NSMutableAttributedString* markedText;
}

- (instancetype)initWithWindow:(NSWindow*)initWindow;

@end // ContentView

@implementation ContentView

- (instancetype)initWithWindow:(NSWindow*)initWindow {
    self = [super init];
    if (self != nil) {
        window = initWindow;
    }

    return self;
}

- (BOOL)canBecomeKeyView {
    return YES;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (BOOL)wantsUpdateLayer {
    return YES;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)event {
    return YES;
}

- (void)mouseDown:(NSEvent *)event {
  // TODO: handle mouse down
}

- (void)mouseDragged:(NSEvent *)event {
  [self mouseMoved:event];
}

- (void)mouseUp:(NSEvent *)event {
  // TODO: handle mouse up
}

- (void)mouseMoved:(NSEvent *)event {
  // TODO: handle mouse move
}

- (void)rightMouseDown:(NSEvent *)event {
  // TODO: handle mouse right down
}

- (void)rightMouseDragged:(NSEvent *)event  {
  [self mouseMoved:event];
}

- (void)rightMouseUp:(NSEvent *)event {
  // TODO: handle mouse right up
}

- (void)otherMouseDown:(NSEvent *)event {
  // TODO: handle middle mouse down
}

- (void)otherMouseDragged:(NSEvent *)event {
  [self mouseMoved:event];
}

- (void)otherMouseUp:(NSEvent *)event {
  // TODO: handle middle mouse up
}

- (void)keyDown:(NSEvent *)event {
  // TODO: handle key down
  [self interpretKeyEvents:@[event]];
}

- (void)keyUp:(NSEvent *)event {
  // TODO: handle key up
}

- (void)scrollWheel:(NSEvent *)event {
  // TODO: handle mouse scroll
}

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange {}

- (void)setMarkedText:(id)string selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange {}

- (void)unmarkText {}

// Defines a constant for empty ranges in NSTextInputClient
static const NSRange kEmptyRange = { NSNotFound, 0 };

- (NSRange)selectedRange {return kEmptyRange;}

- (NSRange)markedRange {return kEmptyRange;}

- (BOOL)hasMarkedText {return false;}

- (nullable NSAttributedString *)attributedSubstringForProposedRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange {return nil;}

- (NSArray<NSAttributedStringKey> *)validAttributesForMarkedText {return [NSArray array];}

- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange {return NSMakeRect(0, 0, 0, 0);}

- (NSUInteger)characterIndexForPoint:(NSPoint)point {return 0;}

@end // ContentView

@interface ApplicationDelegate : NSObject <NSApplicationDelegate> {}

@end // ApplicationDelegate

@implementation ApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    // Posting an empty event at start
    @autoreleasepool {

    NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
    [NSApp postEvent:event atStart:YES];

    } // autoreleasepool

    [NSApp stop:nil];
}

@end // ApplicationDelegate

bool platform_startup(platform_state* plat_state, const char* name, i32 x, i32 y, i32 width, i32 height) {
  plat_state->internal_state = malloc(sizeof(internal_state));
  internal_state *state = (internal_state *)plat_state->internal_state;

  @autoreleasepool {
    [NSApplication sharedApplication];

    // App delegate creation
    state->app_delegate = [[ApplicationDelegate alloc] init];
    if (!state->app_delegate) {
        LAI_LOG_ERROR("Failed to create application delegate");
        return false;
    }
    [NSApp setDelegate:state->app_delegate];

    // Window delegate creation
    // Pass internal state
    state->window_delegate = [[WindowDelegate alloc] initWithState:state];
    if (!state->window_delegate) {
        LAI_LOG_ERROR("Failed to create window delegate");
        return false;
    }

    // Window creation
    state->window = [[NSWindow alloc]
        initWithContentRect:NSMakeRect(x, y, width, height)
        styleMask:NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable
        backing:NSBackingStoreBuffered
        defer:NO];
    if (!state->window) {
        LAI_LOG_ERROR("Failed to create window");
        return false;
    }

    // Layer creation
    state->layer = [CAMetalLayer layer];
    if (!state->layer) {
        LAI_LOG_ERROR("Failed to create layer for view");
    }

    // View creation
    state->view = [[ContentView alloc] initWithWindow:state->window];
    [state->view setLayer:state->layer];
    [state->view setWantsLayer:YES];

    // Setting window properties
    [state->window setLevel:NSNormalWindowLevel];
    [state->window setContentView:state->view];
    [state->window makeFirstResponder:state->view];
    [state->window setTitle:@(name)];
    [state->window setDelegate:state->window_delegate];
    [state->window setAcceptsMouseMovedEvents:YES];
    [state->window setRestorable:NO];

    if (![[NSRunningApplication currentApplication] isFinishedLaunching])
        [NSApp run];

    // Making the app a proper UI app since we're unbundled
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // Putting window in front on launch
    [NSApp activateIgnoringOtherApps:YES];
    [state->window makeKeyAndOrderFront:nil];

    return true;
  } // autoreleasepool
}

void platform_shutdown(platform_state* plat_state) {
  internal_state *state = (internal_state *)plat_state->internal_state;
  if (state) {
    @autoreleasepool {
      [state->window orderOut:nil];

      [state->window setDelegate:nil];
      [state->window_delegate release];

      [state->view release];
      state->view = nil;

      [state->window close];
      state->window = nil;

      [NSApp setDelegate:nil];
      [state->app_delegate release];
      state->app_delegate = nil;
    } // autoreleasepool
    state = nullptr;
  }
}

bool platform_pump_messages(platform_state *plat_state) {
  internal_state *state = (internal_state *)plat_state->internal_state;
    if (state) {
        @autoreleasepool {
        NSEvent* event;
        for (;;) {
            event = [NSApp 
                nextEventMatchingMask:NSEventMaskAny
                untilDate:[NSDate distantPast]
                inMode:NSDefaultRunLoopMode
                dequeue:YES];
            if (!event)
                break;
          [NSApp sendEvent:event];
        }
      } // autoreleasepool
      return !state->quit_flagged;
    }
    return true;
}

void* platform_allocate(u64 size, bool aligned) {
    return malloc(size);
}

void platform_free(void *block, bool aligned) {
    free(block);
}

void* platform_zero_memory(void *block, u64 size) {
    return memset(block, 0, size);
}

void* platform_copy_memory(void *destination, const void *source, u64 size) {
    return memcpy(destination, source, size);
}

void* platform_set_memory(void *destination, i32 value, u64 size) {
    return memset(destination, value, size);
}

void platform_console_write(const char *message, u8 color) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* color_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m\n", color_strings[color], message);
}

void platform_console_write_error(const char *message, u8 color) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* color_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m\n", color_strings[color], message);
}

f64 platform_get_absolute_time() {
    return mach_absolute_time();
}

void platform_sleep(u64 milliseconds) {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000 * 1000;
    nanosleep(&ts, 0);
#else
    if (milliseconds >= 1000) {
        sleep(milliseconds / 1000);
    }
    usleep((milliseconds % 1000) * 1000);
#endif
}

#endif