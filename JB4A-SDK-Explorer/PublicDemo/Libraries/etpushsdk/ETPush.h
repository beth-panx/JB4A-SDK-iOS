//
//  etpushsdk.h
//  etpushsdk
//
//  Created by Eddie Roger on 9/7/12.
//  Copyright © 2015 Salesforce Marketing Cloud. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "ETLocationManager.h"
#import "PushConstants.h"
#import <Availability.h>


/**
 Supporting protocol for OpenDirect, part of the Salesforce 2013-02 release. 
 
 Implementation of this delegate is not required for OpenDirect to function, but it is provided as a convenience to developers who do not with to parse the push payload on their own. 
 
 All OpenDirect data is passed down as a JSON String, so you get it as an NSString. Please remember to parse it appropriately from there. Also, please remember to fail gracefully if you can't take action on the message. 
 
 Also, please note that setting an OpenDirect Delegate will negate the automatic webpage loading feature added to MobilePush recently. This is deliberately to not stomp on your URLs and deep links. 
 */
@protocol ExactTargetOpenDirectDelegate <NSObject>

/**
 Method called when an OpenDirect payload is received from MobilePush. 
 
 @param payload The contents of the payload as received from MobilePush.
 @return Doesn't return a value.
 */
-(void)didReceiveOpenDirectMessageWithContents:(NSString *)payload;

@optional

/**
 Allows you to define the behavior of OpenDirect based on application state. 
 
 If set to YES, the OpenDirect delegate will be called if a push with an OpenDirect payload is received and the application state is running. This is counter to normal push behavior, so the default is NO.

 Consider that if you set this to YES, and the user is running the app when a push comes in, the app will start doing things that they didn't prompt it to do. This is bad user experience since it's confusing to the user. Along these lines, iOS won't present a notification if one is received while the app is running. 
 
 @return BOOL representing whether or not you want action to be taken.
 */
-(BOOL)shouldDeliverOpenDirectMessageIfAppIsRunning;

@end

/** 
 This is the main interface to the Salesforce MobilePush SDK. It is meant to handle a lot of the heavy lifting with regards to sending data back to Salesforce.
 
 Please note that this is a singleton object, and you should reference it as [ETPush pushManager].
 */
@interface ETPush : NSObject
{
    NSDate *_sessionStart;
    NSString *_messageID;
    
    BOOL _showLocalAlert;
    
    // OpenDirect Delegate stuff
    id<ExactTargetOpenDirectDelegate> _openDirectDelegate;
        
}

#pragma mark - Configure the App for ETPush
/**---------------------------------------------------------------------------------------
 * @name Configuring the app for ETPush
 *  ---------------------------------------------------------------------------------------
 */

/**
 Returns (or initializes) the shared pushManager instance.
 
 @return The singleton instance of an ETPush pushManager.
 */
+(ETPush *)pushManager;
-(id)init;

/**
 This is the former main configuration for the MobilePush SDK. As of version 3.0, it is succeeded by configureSDKWithAppID:andAccessToken:withAnalytics:andLocationServices:andCloudPages:. It will continue to function, but calls it's successor with YES for all parameters. This may provide undesired results, so you are encouraged to switch your configuration method to the new one in your next release.
 
 @deprecated
 @param etAppID The App ID generated by Code@ExactTarget to identify the consumer app
 @param accessToken The designed token given to you by Code@ExactTarget that allows you access to the API
 @return Doesn't return a value
 */
-(void)configureSDKWithAppID:(NSString *)etAppID andAccessToken:(NSString *)accessToken;

/**
  This is the main configuration method, responsible for setting credentials needed to communicate with Salesforce. If you are unsure of your accessToken or environment, please visit Code@ExactTarget
 
 Each of the flags in the method are used to control various aspects of the MobilePush SDK. The act as global on/off switches, meaning that if you disable one here, it is off eveywhere.
 
 @param etAppID The App ID generated by Code@ExactTarget to identify the consumer app
 @param accessToken The designed token given to you by Code@ExactTarget that allows you access to the API
 @param analyticsState Whether or not to send analytic data back to Salesforce
 @param locState Whether or not to use Location Services
 @param cpState Whether or not to use Cloud Pages
 @return Doesn't return a value
 */
-(void)configureSDKWithAppID:(NSString *)etAppID
              andAccessToken:(NSString *)accessToken
               withAnalytics:(BOOL)analyticsState
         andLocationServices:(BOOL)locState
                andCloudPages:(BOOL)cpState;


/**
 Sets the OpenDirect delegate.
 
 @param delegate The object you wish to be called when an OpenDirect message is delivered. 
 @return Doesn't return a value.
 */
-(void)setOpenDirectDelegate:(id<ExactTargetOpenDirectDelegate>)delegate;

/**
 Returns the OpenDirect delegate.
 
 @return delegate The named OpenDirect delegate, or nil if there isn't one. 
 */
-(id<ExactTargetOpenDirectDelegate>)openDirectDelegate;

/**
 Triggers a data send to Salesforce. Mostly used internally, and rarely should be called by client code.
 
 */
-(void)updateET;


#pragma mark - Push Lifecycle
/**---------------------------------------------------------------------------------------
 * @name Push Lifecycle
 *
 * These methods are required to make push function on iOS, and to enable the ET SDK to utilize it. All of these methods are required.
 *  ---------------------------------------------------------------------------------------
 */

// Refer to Availability.h for the reasoning behind why the following #if's are used.
// Basically, this will allow the code to be compiled for different IPHONEOS_DEPLOYMENT_TARGET values to
// maintain backward compatibility for running on IOS 6.0 and up as well allowing for using different versions
// of the IOS SDK compiled using XCode 5.X, XCode 6.X and up without getting depricated warnings or undefined warnings.

// IPHONEOS_DEPLOYMENT_TARGET = 6.X or 7.X
#if __IPHONE_OS_VERSION_MIN_REQUIRED < 80000
// Supports IOS SDK 8.X (i.e. XCode 6.X and up)
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 80000
/**
 Wrapper for iOS' application:registerForRemoteNotification; call. It will check that push is allowed, and if so, register with Apple for a token. If push is not enabled, it will notify Salesforce that push is disabled.
 
 @return Doesn't return a value
 */
-(void)registerForRemoteNotifications;

/**
 Wrapper for iOS' isRegisteredForRemoteNotifications; call.
 
 @return BOOL
 */
- (BOOL)isRegisteredForRemoteNotifications;

/**
 Wrapper for iOS' application:registerUserNotificationSettings; call.
 
 @param notificationSettings The UIUserNotificationSettings object that the application would like to use for push. These are pipe-delimited, and use Apple's native values
 @return Doesn't return a value
 */
- (void)registerUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings;

/**
 Wrapper for iOS' currentUserNotificationSettings; call.
 
 @return Doesn't return a value
 */
- (UIUserNotificationSettings *)currentUserNotificationSettings;

/**
 Wrapper for iOS' didRegisterUserNotificationSettings; callback.
 
 @return Returns the current UIUserNotificationSettings object
 */
- (void)didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings;

/**
 Wrapper for iOS' application:registerForRemoteNotificationTypes; call. It will check that push is allowed, and if so, register with Apple for a token. If push is not enabled, it will notify Salesforce that push is disabled.
 
@deprecated in IOS8 - use registerUserNotificationSettings instead
@param types The UIRemoteNotificationTypes that the application would like to use for push. These are pipe-delimited, and use Apple's native values
 @return Doesn't return a value
 */
-(void)registerForRemoteNotificationTypes:(UIRemoteNotificationType)types;
#else
// Supports IOS SDKs < 8.X (i.e. XCode 5.X or less)
/**
 Wrapper for iOS' application:registerForRemoteNotificationTypes; call. It will check that push is allowed, and if so, register with Apple for a token. If push is not enabled, it will notify Salesforce that push is disabled.
 
 @param types The UIRemoteNotificationTypes that the application would like to use for push. These are pipe-delimited, and use Apple's native values
 @return Doesn't return a value
 */
-(void)registerForRemoteNotificationTypes:(UIRemoteNotificationType)types;
#endif
#else
// IPHONEOS_DEPLOYMENT_TARGET >= 8.X
// Supports IOS SDK 8.X (i.e. XCode 6.X and up)
/**
 Wrapper for iOS' application:registerForRemoteNotification; call. It will check that push is allowed, and if so, register with Apple for a token. If push is not enabled, it will notify Salesforce that push is disabled.
 
 @return Doesn't return a value
 */
-(void)registerForRemoteNotifications;

/**
 Wrapper for iOS' isRegisteredForRemoteNotifications; call.
 
 @return BOOL
 */
- (BOOL)isRegisteredForRemoteNotifications;

/**
 Wrapper for iOS' application:registerUserNotificationSettings; call.
 
 @param notificationSettings The UIUserNotificationSettings object that the application would like to use for push. These are pipe-delimited, and use Apple's native values
 @return Doesn't return a value
 */
- (void)registerUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings;

/**
 Wrapper for iOS' currentUserNotificationSettings; call.
 
 @return Returns the current UIUserNotificationSettings object
 */
- (UIUserNotificationSettings *)currentUserNotificationSettings;

/**
 Wrapper for iOS' didRegisterUserNotificationSettings; callback.
 
 @return Doesn't return a value
 */
- (void)didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings;
#endif

/**
 Responsible for sending a received token back to Salesforce. It marks the end of the token registration flow. If it is unable to reach ET server, it will save the token and try again later.
 
 This method is necessary to implementation of ET Push.
 
 @param deviceToken Token as received from Apple, still an NSData object
 @return Doesn't return a value
 */
-(void)registerDeviceToken:(NSData *)deviceToken;

/**
 Returns the device token as a NSString. As requested via GitHub (Issue #3).
 
 @return A stringified version of the Device Token
 */
-(NSString *)deviceToken;

/**
 Handles a registration failure. 
 
 @param error The error returned to the application on a registration failure
 @return Doesn't return a value
 */
-(void)applicationDidFailToRegisterForRemoteNotificationsWithError:(NSError *)error;

/**
 Reset the application's badge number to zero (aka, remove it) and let the push servers know that it should zero the count.
 
 @return Doesn't return a value
 */
-(void)resetBadgeCount;

/**
 Tell the SDK to display a UIAlertView if a push is received while the app is already running. Default behavior is set to NO.
 
 Please note that all push notifications received by the application will be processed, but iOS will *not* present an alert to the user if the app is running when the alert is received. If you set this value to true (YES), then the SDK will generate and present the alert for you. It will not play a sound, though.
 
 @param desiredState YES/NO if you want to display an alert view while the app is running. 
 @return Doesn't return a value
 */
-(void)shouldDisplayAlertViewIfPushReceived:(BOOL)desiredState;


#pragma mark - Application Lifecycle
/**---------------------------------------------------------------------------------------
 * @name Application Lifecycle
 *
 * These methods are not necessary for the Push lifecycle, but are required to make the ET Push SDK perform as expected
 *  ---------------------------------------------------------------------------------------
 */

/**
 Notifies the ET SDK of an app launch, including the dictionary sent to the app by iOS. The launchOptions dictionary is necessary because it will include the APNS dictionary, necessary for processing opens and other analytic information. 
 
 @param launchOptions The dictionary passed to the application by iOS on launch.
 @return Doesn't return a value
 */
-(void)applicationLaunchedWithOptions:(NSDictionary *)launchOptions;

/**
 Notifies the ET SDK of an app termination. Internally, this method does a lot of cleanup. 
 
 @return Doesn't return a value (but how could it - the app terminated)
 */
-(void)applicationTerminated;

/**
 Handles a push notification received by the app when the application is already running. 
 
 This method must be implemented in [[UIApplication sharedApplication] didReceiveRemoteNotification:userInfo].
 
 Sometimes, when a push comes in, the application will already be running (it happens). This method rises to the occasion of handing that notification, displaying an Alert (if the SDK is configured to do so), and calling all of the analytic methods that wouldn't be called otherwise. 
 
 @param userInfo The dictionary containing the push notification
 @param applicationState State of the application at time of notification
 @return Doesn't return a value.
 */
-(void)handleNotification:(NSDictionary *)userInfo forApplicationState:(UIApplicationState)applicationState;

/** 
 Handles a local notification received by the application. 
 
 Sometimes the SDK will use local notifications to indicate something to the user. These are handled differently by iOS, and as such, need to be implemented differently in the SDK. Sorry about that. 
 
 @param notification The received UILocalNotification
 @return Doesn't return a value
 */
-(void)handleLocalNotification:(UILocalNotification *)notification;


#pragma mark - Data Interaction
/**---------------------------------------------------------------------------------------
 * @name Data Interaction
 *  ---------------------------------------------------------------------------------------
 */

/**
 Accepts and sets the Subscriber Key for the device's user.
 
 @param subscriberKey The subscriber key to attribute to the user. 
 @return Doesn't return a value.
 */
-(void)setSubscriberKey:(NSString *)subscriberKey;

/**
 Returns the subscriber key for the active user, in case you need it.

 @return subscriberKey The code-set subscriber key.
 */
-(NSString *)getSubscriberKey;

/**
 Adds the provided Tag (NSString) to the list of tags.
 
 @param tag A string to add to the list of tags
 */
-(void)addTag:(NSString *)tag;

/**
 Removes the provided Tag (NSString) from the list of tags.
 
 @param tag A string to remove from the list of tags
 
 @return tag Echoes the tag back on successful removal, or nil if something failed.
 */
-(NSString *)removeTag:(NSString *)tag;

/**
 Returns the list of tags for this device.
 
 @return All tags associated.
 */
-(NSSet *)allTags;

/**
 Adds an attribute to the data set sent to Salesforce.
 
 @param name The name of the attribute you wish to send. This will be the key of the pair.
 @param value The value to set for thid data pair.
 */
- (void)addAttributeNamed:(NSString*)name value:(NSString*)value;

/**
 Removes the provided attributef rom the data set to send to Salesforce.
 
 @param name The name of the attribute you wish to remove. 
 @return Returns the value that was set. It will no longer be sent back to Salesforce.
 */
- (NSString*)removeAttributeNamed:(NSString*)name;

/**
 Returns a read-only copy of the Attributes dictionary as it is right now. 
 
 @return All attributes currently set
 */
-(NSDictionary *)allAttributes;


#pragma mark - ETPush Convenience Methods
/**
 @name ETPush Convenience Methods
 */

/**
 Gets the Apple-safe, unique Device Identifier that ET will later use to identify the device.
 
 Note that this method is compliant with Apple's compliance rules, but may not be permanent.
 */
+(NSString *)safeDeviceIdentifier;

/**
 Returns the hardware identification string, like "iPhone1,1". Salesforce uses this data for segmentation.
 
 @return A string of the hardware identification. 
 */
+(NSString *)hardwareIdentifier;

/**
 Returns the state of Push based on logic reflected at Salesforce. 
 
 As of this release, Push is considered enabled if the application is able to present an alert (banner, alert) to the user per Settings. Nothing else will be considered.
 */
+(BOOL)isPushEnabled;

/**
 @name Listeners for UIApplication events
 */

/**
 Sets up the listeners.
 */
-(void)startListeningForApplicationNotifications;

/**
 Drops the listeners.
 */
-(void)stopListeningForApplicationNotifications;

/**
 Responds to the UIApplicationDidBecomeActiveNotification notification
 */
-(void)applicationDidBecomeActiveNotificationReceived; // UIApplicationDidBecomeActiveNotification

/**
 Responds to the UIApplicationDidEnterBackgroundNotification notification
 */
-(void)applicationDidEnterBackgroundNotificationReceived; // UIApplicationDidEnterBackgroundNotification

/**
 Set the Log Level
 */

+(void)setETLoggerToRequiredState:(BOOL)state;

/**
 To Log the string whenever [ETPush setETLoggerToState:YES]
 */
+(void) ETLogger: (NSString *) stringToBeLogged;

@end
