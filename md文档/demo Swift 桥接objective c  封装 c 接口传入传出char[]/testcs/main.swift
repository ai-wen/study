//
//  main.swift
//  testcs
//
//  Created by mac on 1/19/21.
//  Copyright © 2021 mac. All rights reserved.
///Volumes/Core_new/xxx.txt

import Foundation


let otp = OTPOC()

var key = "WE5PUDRHTXUCDBBX57WZRWA743VPRT2Z"


//-(NSString*) getotp:(int)ty hasAlg:(int)halg C:(long long)mf D:(int)digist argkey:(NSString*)seckey;

let strOut = otp.getotp(0,alg:0,c:10,d:6,argkey:key)

var str = strOut as! String

print(str)

