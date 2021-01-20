//
//  otpm.m
//  testcs
//
//  Created by mac on 1/19/21.
//  Copyright © 2021 mac. All rights reserved.
//

#import <Foundation/Foundation.h>
//#include "otpc.h"
#include "otpm.h"
#include "motpdll.h"

@implementation OTPOC

-(NSString*) getotp:(int)ty alg:(int)halg c:(int64_t)mf d:(int)digist argkey:(NSString*)seckey
{
    NSString *out = @"";
    char code[50]={0};

    const char * secret =[seckey UTF8String];

    uint64_t moving_factor = GetUtcTime();
    
   // if(0 == moving_factor%30) //整60秒的时候计算一次
    {
        moving_factor = moving_factor/30;
        
        //printf("sizeof(moving_factor)=%d moving_factor=%u ty=%d halg=%d mf=%ul digist=%d secret=%s\n",sizeof(moving_factor),moving_factor,ty,halg,mf,digist,secret);
        
        int ret = otp_compute_code(ty, halg, secret,moving_factor,digist,code) ;
        if( 0  == ret)
        {
            //printf("code=%s\n",code);
            out = [NSString stringWithCString:code  encoding:NSUTF8StringEncoding];
        }
        else
        {
            //printf("code=%s\n",code);
            printf("ret=%d\n",ret);
        }
    }
    

    
    return out;
}

@end
