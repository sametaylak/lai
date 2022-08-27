// clang-format Language: Cpp

#include "platform/platform.h"
#include "renderer/vulkan/vulkan_platform.h"
#include "containers/darray.h"
#include "base/log.h"
#include "base/event.h"
#include "base/input.h"

#ifdef LAI_PLATFORM_MACOSX

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <mach/mach_time.h>
#include <crt_externs.h>

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

// For surface creation
#define VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan.h>
#include "renderer/vulkan/vulkan_types.inl"

@class ApplicationDelegate;
@class WindowDelegate;
@class ContentView;

struct internal_state {
  ApplicationDelegate* app_delegate;
  WindowDelegate* window_delegate;
  NSWindow* window;
  ContentView* view;
  CAMetalLayer* layer;
  VkSurfaceKHR surface;
  bool quit_flagged;
};

keys translate_keycode(u32 ns_keycode);

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

    event_context data = {};
    event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

    // We close on platform shutdown
    return FALSE;
}

- (void)windowDidResize:(NSNotification *)notification {
    event_context context;
    const NSRect contentRect = [state->view frame];
    const NSRect framebufferRect = [state->view convertRectToBacking:contentRect];
    context.data.u16[0] = (u16)framebufferRect.size.width;
    context.data.u16[1] = (u16)framebufferRect.size.height;
    event_fire(EVENT_CODE_RESIZED, 0, context);
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
    event_context context;
    context.data.u16[0] = 0;
    context.data.u16[1] = 0;
    event_fire(EVENT_CODE_RESIZED, 0, context);

    [state->window miniaturize:nil];
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
    event_context context;
    const NSRect contentRect = [state->view frame];
    const NSRect framebufferRect = [state->view convertRectToBacking:contentRect];
    context.data.u16[0] = (u16)framebufferRect.size.width;
    context.data.u16[1] = (u16)framebufferRect.size.height;
    event_fire(EVENT_CODE_RESIZED, 0, context);

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
  input_process_button(BUTTON_LEFT, true);
}

- (void)mouseDragged:(NSEvent *)event {
  [self mouseMoved:event];
}

- (void)mouseUp:(NSEvent *)event {
  input_process_button(BUTTON_LEFT, false);
}

- (void)mouseMoved:(NSEvent *)event {
  const NSPoint pos = [event locationInWindow];
  input_process_mouse_move((i16)pos.x, (i16)pos.y);
}

- (void)rightMouseDown:(NSEvent *)event {
  input_process_button(BUTTON_RIGHT, true);
}

- (void)rightMouseDragged:(NSEvent *)event  {
  [self mouseMoved:event];
}

- (void)rightMouseUp:(NSEvent *)event {
  input_process_button(BUTTON_RIGHT, false);
}

- (void)otherMouseDown:(NSEvent *)event {
  input_process_button(BUTTON_MIDDLE, true);
}

- (void)otherMouseDragged:(NSEvent *)event {
  [self mouseMoved:event];
}

- (void)otherMouseUp:(NSEvent *)event {
  input_process_button(BUTTON_MIDDLE, false);
}

- (void)keyDown:(NSEvent *)event {
  keys key = translate_keycode((u32)[event keyCode]);
  input_process_key(key, true);

  [self interpretKeyEvents:@[event]];
}

- (void)keyUp:(NSEvent *)event {
  keys key = translate_keycode((u32)[event keyCode]);
  input_process_key(key, false);
}

- (void)scrollWheel:(NSEvent *)event {
  input_process_mouse_wheel((i8)[event scrollingDeltaY]);
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

    // send first resize event
    event_context context;
    const NSRect contentRect = [state->view frame];
    const NSRect framebufferRect = [state->view convertRectToBacking:contentRect];
    LAI_LOG_INFO("%i", (u16)framebufferRect.size.width);
    context.data.u16[0] = (u16)framebufferRect.size.width;
    context.data.u16[1] = (u16)framebufferRect.size.height;
    event_fire(EVENT_CODE_RESIZED, 0, context);

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
  if (state != nullptr) {
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

bool platform_create_vulkan_surface(struct platform_state *plat_state, struct vulkan_context *context) {
  internal_state *state = (internal_state *)plat_state->internal_state;

  VkMetalSurfaceCreateInfoEXT create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
  create_info.pLayer = state->layer;

  VkResult result = vkCreateMetalSurfaceEXT(
    context->instance, 
    &create_info,
    context->allocator,
    &state->surface);

  if (result != VK_SUCCESS) {
      LAI_LOG_FATAL("Vulkan surface creation failed.");
      return false;
  }

  context->surface = state->surface;
  return true;
}

void platform_get_required_extension_names(const char ***names_darray) {
  darray_push(*names_darray, "VK_EXT_metal_surface");
  darray_push(*names_darray, "VK_KHR_get_physical_device_properties2");
  darray_push(*names_darray, "VK_KHR_portability_enumeration");
}

keys translate_keycode(u32 ns_keycode) { 
    switch (ns_keycode) {
        case 0x1D:
            return KEY_NUMPAD0;
        case 0x12:
            return KEY_NUMPAD1;
        case 0x13:
            return KEY_NUMPAD2;
        case 0x14:
            return KEY_NUMPAD3;
        case 0x15:
            return KEY_NUMPAD4;
        case 0x17:
            return KEY_NUMPAD5;
        case 0x16:
            return KEY_NUMPAD6;
        case 0x1A:
            return KEY_NUMPAD7;
        case 0x1C:
            return KEY_NUMPAD8;
        case 0x19:
            return KEY_NUMPAD9;

        case 0x00:
            return KEY_A;
        case 0x0B:
            return KEY_B;
        case 0x08:
            return KEY_C;
        case 0x02:
            return KEY_D;
        case 0x0E:
            return KEY_E;
        case 0x03:
            return KEY_F;
        case 0x05:
            return KEY_G;
        case 0x04:
            return KEY_H;
        case 0x22:
            return KEY_I;
        case 0x26:
            return KEY_J;
        case 0x28:
            return KEY_K;
        case 0x25:
            return KEY_L;
        case 0x2E:
            return KEY_M;
        case 0x2D:
            return KEY_N;
        case 0x1F:
            return KEY_O;
        case 0x23:
            return KEY_P;
        case 0x0C:
            return KEY_Q;
        case 0x0F:
            return KEY_R;
        case 0x01:
            return KEY_S;
        case 0x11:
            return KEY_T;
        case 0x20:
            return KEY_U;
        case 0x09:
            return KEY_V;
        case 0x0D:
            return KEY_W;
        case 0x07:
            return KEY_X;
        case 0x10:
            return KEY_Y;
        case 0x06:
            return KEY_Z;

        case 0x27:
            return KEYS_MAX_KEYS; // Apostrophe
        case 0x2A:
            return KEYS_MAX_KEYS; // Backslash
        case 0x2B:
            return KEY_COMMA;
        case 0x18:
            return KEYS_MAX_KEYS; // Equal
        case 0x32:
            return KEY_GRAVE;
        case 0x21:
            return KEYS_MAX_KEYS; // Left bracket
        case 0x1B:
            return KEY_MINUS;
        case 0x2F:
            return KEY_PERIOD;
        case 0x1E:
            return KEYS_MAX_KEYS; // Right bracket
        case 0x29:
            return KEY_SEMICOLON;
        case 0x2C:
            return KEY_SLASH;
        case 0x0A:
            return KEYS_MAX_KEYS; // ?

        case 0x33:
            return KEY_BACKSPACE;
        case 0x39:
            return KEY_CAPITAL;
        case 0x75:
            return KEY_DELETE;
        case 0x7D:
            return KEY_DOWN;
        case 0x77:
            return KEY_END;
        case 0x24:
            return KEY_ENTER;
        case 0x35:
            return KEY_ESCAPE;
        case 0x7A:
            return KEY_F1;
        case 0x78:
            return KEY_F2;
        case 0x63:
            return KEY_F3;
        case 0x76:
            return KEY_F4;
        case 0x60:
            return KEY_F5;
        case 0x61:
            return KEY_F6;
        case 0x62:
            return KEY_F7;
        case 0x64:
            return KEY_F8;
        case 0x65:
            return KEY_F9;
        case 0x6D:
            return KEY_F10;
        case 0x67:
            return KEY_F11;
        case 0x6F:
            return KEY_F12;
        case 0x69:
            return KEY_PRINT;
        case 0x6B:
            return KEY_F14;
        case 0x71:
            return KEY_F15;
        case 0x6A:
            return KEY_F16;
        case 0x40:
            return KEY_F17;
        case 0x4F:
            return KEY_F18;
        case 0x50:
            return KEY_F19;
        case 0x5A:
            return KEY_F20;
        case 0x73:
            return KEY_HOME;
        case 0x72:
            return KEY_INSERT;
        case 0x7B:
            return KEY_LEFT;
        case 0x3A:
            return KEY_LALT;
        case 0x3B:
            return KEY_LCONTROL;
        case 0x38:
            return KEY_LSHIFT;
        case 0x37:
            return KEY_LWIN;
        case 0x6E:
            return KEYS_MAX_KEYS; // Menu
        case 0x47:
            return KEY_NUMLOCK;
        case 0x79:
            return KEYS_MAX_KEYS; // Page down
        case 0x74:
            return KEYS_MAX_KEYS; // Page up
        case 0x7C:
            return KEY_RIGHT;
        case 0x3D:
            return KEY_RALT;
        case 0x3E:
            return KEY_RCONTROL;
        case 0x3C:
            return KEY_RSHIFT;
        case 0x36:
            return KEY_RWIN;
        case 0x31:
            return KEY_SPACE;
        case 0x30:
            return KEY_TAB;
        case 0x7E:
            return KEY_UP;

        case 0x52:
            return KEY_NUMPAD0;
        case 0x53:
            return KEY_NUMPAD1;
        case 0x54:
            return KEY_NUMPAD2;
        case 0x55:
            return KEY_NUMPAD3;
        case 0x56:
            return KEY_NUMPAD4;
        case 0x57:
            return KEY_NUMPAD5;
        case 0x58:
            return KEY_NUMPAD6;
        case 0x59:
            return KEY_NUMPAD7;
        case 0x5B:
            return KEY_NUMPAD8;
        case 0x5C:
            return KEY_NUMPAD9;
        case 0x45:
            return KEY_ADD;
        case 0x41:
            return KEY_DECIMAL;
        case 0x4B:
            return KEY_DIVIDE;
        case 0x4C:
            return KEY_ENTER;
        case 0x51:
            return KEY_NUMPAD_EQUAL;
        case 0x43:
            return KEY_MULTIPLY;
        case 0x4E:
            return KEY_SUBTRACT;

        default:
            return KEYS_MAX_KEYS;
    }
}

#endif