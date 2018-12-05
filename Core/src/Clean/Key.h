/** \file Core/Key.h
**/

#ifndef CLEAN_KEY_H
#define CLEAN_KEY_H

#include <cstddef>
#include <cstdint>

namespace Clean 
{
    //! @brief A Key represents only the real key that is pressed/released on a Keyboard. This means 
    //! key press and release are not related to any virtual keyboard event. No modifiers are available
    //! here, only the physical keys pressed on the keyboard. For now those keys are based upon the US 
    //! layout. A Key translator may uses the default US layout to translate into the current keyboard
    //! layout, based on the current locale. 
    typedef std::uint16_t Key;
    
    static constexpr const std::uint16_t kKeyNull = 0;
    static constexpr const std::uint16_t kKeyInvalid = 0;
    
    // Groups Alpha keys. 
    static constexpr const std::uint16_t kKeyA = 1;
    static constexpr const std::uint16_t kKeyB = 2;
    static constexpr const std::uint16_t kKeyC = 3;
    static constexpr const std::uint16_t kKeyD = 4;
    static constexpr const std::uint16_t kKeyE = 5;
    static constexpr const std::uint16_t kKeyF = 6;
    static constexpr const std::uint16_t kKeyG = 7;
    static constexpr const std::uint16_t kKeyH = 8;
    static constexpr const std::uint16_t kKeyI = 9;
    static constexpr const std::uint16_t kKeyJ = 10;
    static constexpr const std::uint16_t kKeyK = 11;
    static constexpr const std::uint16_t kKeyL = 12;
    static constexpr const std::uint16_t kKeyM = 13;
    static constexpr const std::uint16_t kKeyN = 14;
    static constexpr const std::uint16_t kKeyO = 15;
    static constexpr const std::uint16_t kKeyP = 16;
    static constexpr const std::uint16_t kKeyQ = 17;
    static constexpr const std::uint16_t kKeyR = 18;
    static constexpr const std::uint16_t kKeyS = 19;
    static constexpr const std::uint16_t kKeyT = 20;
    static constexpr const std::uint16_t kKeyU = 21;
    static constexpr const std::uint16_t kKeyV = 22;
    static constexpr const std::uint16_t kKeyW = 23;
    static constexpr const std::uint16_t kKeyX = 24;
    static constexpr const std::uint16_t kKeyY = 25;
    static constexpr const std::uint16_t kKeyZ = 26;
    
    // Groups Numeric keys. 
    static constexpr const std::uint16_t kKey1 = 27;
    static constexpr const std::uint16_t kKey2 = 28;
    static constexpr const std::uint16_t kKey3 = 29;
    static constexpr const std::uint16_t kKey4 = 30;
    static constexpr const std::uint16_t kKey5 = 31;
    static constexpr const std::uint16_t kKey6 = 32;
    static constexpr const std::uint16_t kKey7 = 33;
    static constexpr const std::uint16_t kKey8 = 34;
    static constexpr const std::uint16_t kKey9 = 35;
    static constexpr const std::uint16_t kKey0 = 36;
    
    // Groups some control keys. 
    static constexpr const std::uint16_t kKeyReturn = 37;
    static constexpr const std::uint16_t kKeySpace = 38;
    static constexpr const std::uint16_t kKeyEscape = 39;
    static constexpr const std::uint16_t kKeyShift = 40;
    static constexpr const std::uint16_t kKeyShiftRight = 41;
    static constexpr const std::uint16_t kKeyControl = 42;
    static constexpr const std::uint16_t kKeyControlRight = 43;
    
    // Groups APPLE specific keys (Command, option, fn)
    static constexpr const std::uint16_t kKeyAppleCommand = 44;
    static constexpr const std::uint16_t kKeyAppleOption = 45;
    static constexpr const std::uint16_t kKeyAppleFn = 46;
    static constexpr const std::uint16_t kKeyAppleHelp = 47;
    
    // Other control keys
    static constexpr const std::uint16_t kKeyDelete = 48;
    static constexpr const std::uint16_t kKeyF1 = 49;
    static constexpr const std::uint16_t kKeyF2 = 50;
    static constexpr const std::uint16_t kKeyF3 = 51;
    static constexpr const std::uint16_t kKeyF4 = 52;
    static constexpr const std::uint16_t kKeyF5 = 53;
    static constexpr const std::uint16_t kKeyF6 = 54;
    static constexpr const std::uint16_t kKeyF7 = 55;
    static constexpr const std::uint16_t kKeyF8 = 56;
    static constexpr const std::uint16_t kKeyF9 = 57;
    static constexpr const std::uint16_t kKeyF10 = 58;
    static constexpr const std::uint16_t kKeyF11 = 59;
    static constexpr const std::uint16_t kKeyF12 = 60;
    static constexpr const std::uint16_t kKeyTab = 61;
    
    // Arrows
    static constexpr const std::uint16_t kKeyArrowLeft = 62;
    static constexpr const std::uint16_t kKeyArrowRight = 62;
    static constexpr const std::uint16_t kKeyArrowUp = 62;
    static constexpr const std::uint16_t kKeyArrowDown = 62;
    
    // [...] TODO Do all other keys. It's long.
    
    /*! @brief Returns true if key is a specific Apple control key (command, function or option). */
    bool KeyIsApple(Key key);
    
    /*! @brief Returns true if key is alpha numeric. */
    bool KeyIsAlnum(Key key);
    
    /*! @brief Returns true if key is alphabetic. */
    bool KeyIsAlpha(Key key);
    
    /*! @brief Returns true if key is numeric. */
    bool KeyIsNum(Key key);
}

#endif // CLEAN_KEY_H
