//
//  harlequin_OSX10_7Tests.m
//  harlequin_OSX10.7Tests
//
//  Created by david allen on 10/25/14.
//
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

@interface harlequin_OSX10_7Tests : XCTestCase

@end

@implementation harlequin_OSX10_7Tests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testExample {
    // This is an example of a functional test case.
    XCTAssert(YES, @"Pass");
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
