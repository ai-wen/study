//
//  otpc.c
//  testcs
//
//  Created by mac on 1/19/21.
//  Copyright © 2021 mac. All rights reserved.
//

#include "otpc.h"

#include <string.h>

int getotpc(const char* sc, char* out)
{
    if(!sc || !out)
        return 1;
    
    //strcpy(out, sc);
    
    memcpy(out, sc, strlen(sc));
    
    return 0;
}
