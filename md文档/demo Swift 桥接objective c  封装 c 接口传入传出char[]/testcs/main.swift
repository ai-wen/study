//
//  main.swift
//  testcs
//
//  Created by mac on 1/19/21.
//  Copyright © 2021 mac. All rights reserved.
//

import Foundation

print("Hello, World!")

let otp = OTPOC()

var key = "1234790"

let strOut = otp.getotp(0, argkey:key)

var str = strOut as! String

print(str)

