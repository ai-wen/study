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

-(NSString*) getotp:(int)ty argkey:(NSString*)seckey;

@end


#endif /* otpm_h */
