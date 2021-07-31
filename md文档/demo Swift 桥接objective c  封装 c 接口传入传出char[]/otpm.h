//
//  otpm.h
//  testcs
//
//  Created by mac on 1/19/21.
//  Copyright © 2021 mac. All rights reserved.
//

#ifndef otpm_h
#define otpm_h

#import <Foundation/Foundation.h>

@interface OTPOC : NSObject

-(NSString*) getotp:(int)ty alg:(int)halg c:(int64_t)mf d:(int)digist argkey:(NSString*)seckey;
//参数标示必须小写 alg： c: d: argkey

@end


#endif /* otpm_h */
