//
//  otpm.m
//  testcs
//
//  Created by mac on 1/19/21.
//  Copyright © 2021 mac. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "otpc.h"
#include "otpm.h"

@implementation OTPOC

-(NSString*) getotp:(int)ty argkey:(NSString*)seckey
{
    NSString *out;
    char code[50]={0};
    //printf("code=%s\n",code);
    const char * a =[seckey UTF8String];
    
    //printf("a=%s\n",a);
    if(0 == getotpc(a, code))
    {
        //printf("code=%s\n",code);
        out = [NSString stringWithCString:code  encoding:NSUTF8StringEncoding];
    }
    
    return out;
}

@end
