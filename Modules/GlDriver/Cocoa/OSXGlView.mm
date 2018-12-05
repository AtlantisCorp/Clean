/** \file GlDriver/Cocoa/OSXGlView.h
**/

#include "OSXGlView.h"
#include "OSXGlRenderWindow.h"

#include <Clean/NotificationCenter.h>
#include <Carbon/Carbon.h>
#include <map>

static std::uint16_t OSXCharactersToKey(std::string const& chars)
{
    if (chars.size() == 1)
    {
        char keychar = chars.at(0);
        
        if (keychar >= 'a' && keychar <= 'z')
            return keychar - ((std::uint16_t)'a' - Clean::kKeyA);
        else if (keychar >= 'A' && keychar <= 'Z')
            return keychar - ((std::uint16_t)'A' - Clean::kKeyA);
        else if (keychar >= '0' && keychar <= '9')
            return keychar - ((std::uint16_t)'0' - Clean::kKey0);
    }
    
    return Clean::kKeyNull;
}

/*! @brief Filters if a modifier is pressed or released in the given event.
 *
 * \param[in] event Event we need to watch for a modifier flag.
 * \param[in] flag The flag we are looking for.
 * \param[in] key The Clean::Key associated to this modifier flag.
 * \param[in] keyname For debug only, a name associated to the modifier flag.
 * \param[in] creator The creator of OSXGlView, i.e. its render window.
 * \param[out] flagsState A map where to update the current modifier flag state.
 *
 * This function looks if, in flagsState and in the event, the given flag is present and
 * in the same state. If the modifier's state in the event and in the map differs, the map
 * is updated and a Clean::WindowKeyEvent is launched from creator.
 *
**/
static void OSXFilterModifierFlag(NSEvent* event,
                                  NSEventModifierFlags flag,
                                  std::uint16_t key,
                                  std::string const& keyname,
                                  OSXGlRenderWindow* creator,
                                  std::map < NSEventModifierFlags, bool >& flagsState)
{
    if ([event modifierFlags] & flag)
    {
        auto flagFound = flagsState.find(flag);
        
        if (flagFound != flagsState.end() && !flagFound->second)
        {
            flagFound->second = true;
            Clean::Notification notif = Clean:: BuildNotification(Clean::kNotificationLevelInfo, "OSX: %s modifier has been pressed.", keyname.data());
            Clean::NotificationCenter::GetDefault()->send(notif);
            
            Clean::WindowKeyEvent e;
            e.emitter = creator;
            e.pressed = true;
            e.key = key;
            creator->send(&Clean::WindowListener::onWindowKey, e);
        }
        
        else if (flagFound == flagsState.end())
        {
            flagsState[flag] = true;
            Clean::Notification notif = Clean:: BuildNotification(Clean::kNotificationLevelInfo, "OSX: %s modifier has been pressed.", keyname.data());
            Clean::NotificationCenter::GetDefault()->send(notif);
            
            Clean::WindowKeyEvent e;
            e.emitter = creator;
            e.pressed = true;
            e.key = key;
            creator->send(&Clean::WindowListener::onWindowKey, e);
        }
    }
    
    else
    {
        auto flagFound = flagsState.find(flag);
        
        if (flagFound != flagsState.end() && flagFound->second)
        {
            flagFound->second = false;
            Clean::Notification notif = Clean:: BuildNotification(Clean::kNotificationLevelInfo, "OSX: %s modifier has been released.", keyname.data());
            Clean::NotificationCenter::GetDefault()->send(notif);
            
            Clean::WindowKeyEvent e;
            e.emitter = creator;
            e.pressed = false;
            e.key = key;
            creator->send(&Clean::WindowListener::onWindowKey, e);
        }
        
        else if (flagFound == flagsState.end())
        {
            flagsState[flag] = true;
            Clean::Notification notif = Clean:: BuildNotification(Clean::kNotificationLevelInfo, "OSX: %s modifier has been released.", keyname.data());
            Clean::NotificationCenter::GetDefault()->send(notif);
            
            Clean::WindowKeyEvent e;
            e.emitter = creator;
            e.pressed = false;
            e.key = key;
            creator->send(&Clean::WindowListener::onWindowKey, e);
        }
    }
}

/** @brief Filters a unichar for some common NS defined characters. */
static std::uint16_t OSXFilterCommonCharacters(unichar keychar)
{
    switch (keychar)
    {
        case NSEnterCharacter:
        return Clean::kKeyReturn;
            
        case NSBackspaceCharacter:
        case NSDeleteCharacter:
        return Clean::kKeyDelete;
            
        case NSTabCharacter:
        return Clean::kKeyTab;
            
        default:
        return Clean::kKeyNull;
    }
}

@implementation OSXGlView

- (id) initWithFrame:(NSRect)frame
          andContext:(NSOpenGLContext*)context
          andCreator:(OSXGlRenderWindow*)creator
{
    self = [super initWithFrame:frame];
    
    if (self) {
        context_ = context;
        creator_ = creator;
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(_surfaceNeedsUpdate:)
                                                     name:NSViewGlobalFrameDidChangeNotification
                                                   object:self];
    }
    
    return self;
}

- (NSOpenGLContext*) openGLContext
{
    return context_;
}

- (BOOL) isOpaque
{
    return YES;
}

- (BOOL) canBecomeKeyView 
{
    return YES;
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (void) _surfaceNeedsUpdate:(NSNotification*)notification
{
    [self update];
}

- (void) lockFocus
{
    NSOpenGLContext* context = [self openGLContext];
    
    [super lockFocus];
    if ([context view] != self) {
        [context setView:self];
    }
    
    [context makeCurrentContext];
}

- (void) viewDidMoveToWindow
{
    NSOpenGLContext* context = [self openGLContext];
    
    [super viewDidMoveToWindow];
    if ([self window] == nil) {
        [context clearDrawable];
    }
}

- (void) update
{
    NSOpenGLContext* context = [self openGLContext];
    
    if (![self inLiveResize]) {
        [context update];
    }
}

- (void)keyUp:(NSEvent *)event
{
    Clean::WindowKeyEvent cleanEvent;
    cleanEvent.pressed = false;
    cleanEvent.emitter = creator_;
    
    if ([event modifierFlags] & NSEventModifierFlagNumericPad)
    {
        NSString* theArrow = [event charactersIgnoringModifiers];
        unichar keyChar = 0;
        
        if ([theArrow length] == 0)
            return;
        
        if ([theArrow length] == 1)
        {
            keyChar = [theArrow characterAtIndex:0];
            
            if (keyChar == NSLeftArrowFunctionKey)
                cleanEvent.key = Clean::kKeyArrowLeft;
            
            else if (keyChar == NSRightArrowFunctionKey)
                cleanEvent.key = Clean::kKeyArrowRight;
            
            else if (keyChar == NSUpArrowFunctionKey)
                cleanEvent.key = Clean::kKeyArrowUp;
            
            else if (keyChar == NSDownArrowFunctionKey)
                cleanEvent.key = Clean::kKeyArrowDown;
        }
    }
    
    else
    {
        // Apart from Arrows, we can use characters without modifiers to see which
        // character we are actually using. This character will be converted to its
        // kKey equivalent. NOTES that some character may not be available.
        NSString* characters = [event charactersIgnoringModifiers];
        unichar keyChar = 0;
        
        if ([characters length] == 1)
        {
            keyChar = [characters characterAtIndex:0];
            cleanEvent.key = OSXFilterCommonCharacters(keyChar);
            
            if (cleanEvent.key == Clean::kKeyNull)
            {
                std::string cCharacters([characters UTF8String], [characters lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
                cleanEvent.key = OSXCharactersToKey(cCharacters);
            }
            
            if (cleanEvent.key == Clean::kKeyNull)
            {
                std::string cCharacters([characters UTF8String], [characters lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
                Clean::Notification notif = Clean:: BuildNotification(Clean::kNotificationLevelWarning, "OSX: Characters '%s' are not bound to a virtual key.", cCharacters.data());
                Clean::NotificationCenter::GetDefault()->send(notif);
                return;
            }
        }
    }
    
    creator_->send(&Clean::WindowListener::onWindowKey, cleanEvent);
}

- (void)keyDown:(NSEvent *)event
{
    Clean::WindowKeyEvent cleanEvent;
    cleanEvent.pressed = true;
    cleanEvent.emitter = creator_;
    
    if ([event modifierFlags] & NSEventModifierFlagNumericPad)
    {
        NSString* theArrow = [event charactersIgnoringModifiers];
        unichar keyChar = 0;
        
        if ([theArrow length] == 0)
            return;
        
        if ([theArrow length] == 1)
        {
            keyChar = [theArrow characterAtIndex:0];
            
            if (keyChar == NSLeftArrowFunctionKey)
                cleanEvent.key = Clean::kKeyArrowLeft;
            
            else if (keyChar == NSRightArrowFunctionKey)
                cleanEvent.key = Clean::kKeyArrowRight;
            
            else if (keyChar == NSUpArrowFunctionKey)
                cleanEvent.key = Clean::kKeyArrowUp;
            
            else if (keyChar == NSDownArrowFunctionKey)
                cleanEvent.key = Clean::kKeyArrowDown;
        }
    }
    
    else
    {
        // Apart from Arrows, we can use characters without modifiers to see which
        // character we are actually using. This character will be converted to its
        // kKey equivalent. NOTES that some character may not be available.
        NSString* characters = [event charactersIgnoringModifiers];
        unichar keyChar = 0;
        
        if ([characters length] == 1)
        {
            keyChar = [characters characterAtIndex:0];
            cleanEvent.key = OSXFilterCommonCharacters(keyChar);
            
            if (cleanEvent.key == Clean::kKeyNull)
            {
                std::string cCharacters([characters UTF8String], [characters lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
                cleanEvent.key = OSXCharactersToKey(cCharacters);
            }
            
            if (cleanEvent.key == Clean::kKeyNull)
            {
                std::string cCharacters([characters UTF8String], [characters lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
                Clean::Notification notif = Clean:: BuildNotification(Clean::kNotificationLevelWarning, "OSX: Characters '%s' are not bound to a virtual key.", cCharacters.data());
                Clean::NotificationCenter::GetDefault()->send(notif);
                return;
            }
        }
    }
    
    creator_->send(&Clean::WindowListener::onWindowKey, cleanEvent);
}

- (void)mouseMoved:(NSEvent *)event
{
    Clean::WindowMouseMovedEvent e;
    e.emitter = creator_;
    e.deltaX = [event deltaX];
    e.deltaY = [event deltaY];
    creator_->send(&Clean::WindowListener::onWindowMouseMoved, e);
}

- (void)flagsChanged:(NSEvent *)event
{
    static std::map < NSEventModifierFlags, bool > flagsState;
    OSXFilterModifierFlag(event, NSEventModifierFlagShift, Clean::kKeyShift, "Shift", creator_, flagsState);
    OSXFilterModifierFlag(event, NSEventModifierFlagHelp, Clean::kKeyAppleHelp, "Help", creator_, flagsState);
    OSXFilterModifierFlag(event, NSEventModifierFlagOption, Clean::kKeyAppleOption, "Option", creator_, flagsState);
    OSXFilterModifierFlag(event, NSEventModifierFlagCommand, Clean::kKeyAppleCommand, "Command", creator_, flagsState);
}

@end
