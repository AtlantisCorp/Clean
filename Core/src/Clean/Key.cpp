/** \file Core/Key.cpp
**/

#include "Key.h"

namespace Clean 
{
    bool KeyIsApple(Key key) 
    { 
        return key >= kKeyAppleCommand && key <= kKeyAppleHelp; 
    }
    
    bool KeyIsAlnum(Key key) 
    { 
        return key >= kKeyA && key <= kKey0; 
    }
    
    bool KeyIsAlpha(Key key) 
    { 
        return key >= kKeyA && key <= kKeyZ; 
    }
    
    bool KeyIsNum(Key key) 
    { 
        return key >= kKey1 && key <= kKey0; 
    }
}
