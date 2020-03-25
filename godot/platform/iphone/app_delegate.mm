/*************************************************************************/
/*  app_delegate.mm                                                      */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#import "app_delegate.h"

#include "core/project_settings.h"
#include "drivers/coreaudio/audio_driver_coreaudio.h"
#import "gl_view.h"
#include "main/main.h"
#include "os_iphone.h"

#import "GameController/GameController.h"
#import <AudioToolbox/AudioServices.h>

#define kFilteringFactor 0.1
#define kRenderingFrequency 60
#define kAccelerometerFrequency 100.0 // Hz

Error _shell_open(String);
void _set_keep_screen_on(bool p_enabled);

Error _shell_open(String p_uri) {
	NSString *url = [[NSString alloc] initWithUTF8String:p_uri.utf8().get_data()];

	if (![[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:url]])
		return ERR_CANT_OPEN;

	printf("opening url %ls\n", p_uri.c_str());
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
	[url release];
	return OK;
};

void _set_keep_screen_on(bool p_enabled) {
	[[UIApplication sharedApplication] setIdleTimerDisabled:(BOOL)p_enabled];
};

void _vibrate() {
	AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
};

@implementation AppDelegate

@synthesize window;

extern int gargc;
extern char **gargv;
extern int iphone_main(int, int, int, char **, String);
extern void iphone_finish();

CMMotionManager *motionManager;
bool motionInitialised;

static ViewController *mainViewController = nil;
+ (ViewController *)getViewController {
	return mainViewController;
}

NSMutableDictionary *ios_joysticks = nil;
NSMutableArray *pending_ios_joysticks = nil;

- (GCControllerPlayerIndex)getFreePlayerIndex {
	bool have_player_1 = false;
	bool have_player_2 = false;
	bool have_player_3 = false;
	bool have_player_4 = false;

	if (ios_joysticks == nil) {
		NSArray *keys = [ios_joysticks allKeys];
		for (NSNumber *key in keys) {
			GCController *controller = [ios_joysticks objectForKey:key];
			if (controller.playerIndex == GCControllerPlayerIndex1) {
				have_player_1 = true;
			} else if (controller.playerIndex == GCControllerPlayerIndex2) {
				have_player_2 = true;
			} else if (controller.playerIndex == GCControllerPlayerIndex3) {
				have_player_3 = true;
			} else if (controller.playerIndex == GCControllerPlayerIndex4) {
				have_player_4 = true;
			};
		};
	};

	if (!have_player_1) {
		return GCControllerPlayerIndex1;
	} else if (!have_player_2) {
		return GCControllerPlayerIndex2;
	} else if (!have_player_3) {
		return GCControllerPlayerIndex3;
	} else if (!have_player_4) {
		return GCControllerPlayerIndex4;
	} else {
		return GCControllerPlayerIndexUnset;
	};
};

void _ios_add_joystick(GCController *controller, AppDelegate *delegate) {
	// get a new id for our controller
	int joy_id = OSIPhone::get_singleton()->get_unused_joy_id();
	if (joy_id != -1) {
		// assign our player index
		if (controller.playerIndex == GCControllerPlayerIndexUnset) {
			controller.playerIndex = [delegate getFreePlayerIndex];
		};

		// tell Godot about our new controller
		OSIPhone::get_singleton()->joy_connection_changed(
				joy_id, true, [controller.vendorName UTF8String]);

		// add it to our dictionary, this will retain our controllers
		[ios_joysticks setObject:controller
						  forKey:[NSNumber numberWithInt:joy_id]];

		// set our input handler
		[delegate setControllerInputHandler:controller];
	} else {
		printf("Couldn't retrieve new joy id\n");
	};
}

static void on_focus_out(ViewController *view_controller, bool *is_focus_out) {
	if (!*is_focus_out) {
		*is_focus_out = true;
		if (OS::get_singleton()->get_main_loop())
			OS::get_singleton()->get_main_loop()->notification(
					MainLoop::NOTIFICATION_WM_FOCUS_OUT);

		[view_controller.view stopAnimation];
		if (OS::get_singleton()->native_video_is_playing()) {
			OSIPhone::get_singleton()->native_video_focus_out();
		}

		AudioDriverCoreAudio *audio = dynamic_cast<AudioDriverCoreAudio *>(AudioDriverCoreAudio::get_singleton());
		if (audio)
			audio->stop();
	}
}

static void on_focus_in(ViewController *view_controller, bool *is_focus_out) {
	if (*is_focus_out) {
		*is_focus_out = false;
		if (OS::get_singleton()->get_main_loop())
			OS::get_singleton()->get_main_loop()->notification(
					MainLoop::NOTIFICATION_WM_FOCUS_IN);

		[view_controller.view startAnimation];
		if (OSIPhone::get_singleton()->native_video_is_playing()) {
			OSIPhone::get_singleton()->native_video_unpause();
		}

		AudioDriverCoreAudio *audio = dynamic_cast<AudioDriverCoreAudio *>(AudioDriverCoreAudio::get_singleton());
		if (audio)
			audio->start();
	}
}

- (void)controllerWasConnected:(NSNotification *)notification {
	// create our dictionary if we don't have one yet
	if (ios_joysticks == nil) {
		ios_joysticks = [[NSMutableDictionary alloc] init];
	};

	// get our controller
	GCController *controller = (GCController *)notification.object;
	if (controller == nil) {
		printf("Couldn't retrieve new controller\n");
	} else if ([[ios_joysticks allKeysForObject:controller] count] != 0) {
		printf("Controller is already registered\n");
	} else if (frame_count > 1) {
		_ios_add_joystick(controller, self);
	} else {
		if (pending_ios_joysticks == nil)
			pending_ios_joysticks = [[NSMutableArray alloc] init];
		[pending_ios_joysticks addObject:controller];
	};
};

- (void)controllerWasDisconnected:(NSNotification *)notification {
	if (ios_joysticks != nil) {
		// find our joystick, there should be only one in our dictionary
		GCController *controller = (GCController *)notification.object;
		NSArray *keys = [ios_joysticks allKeysForObject:controller];
		for (NSNumber *key in keys) {
			// tell Godot this joystick is no longer there
			int joy_id = [key intValue];
			OSIPhone::get_singleton()->joy_connection_changed(joy_id, false, "");

			// and remove it from our dictionary
			[ios_joysticks removeObjectForKey:key];
		};
	};
};

- (int)getJoyIdForController:(GCController *)controller {
	if (ios_joysticks != nil) {
		// find our joystick, there should be only one in our dictionary
		NSArray *keys = [ios_joysticks allKeysForObject:controller];
		for (NSNumber *key in keys) {
			int joy_id = [key intValue];
			return joy_id;
		};
	};

	return -1;
};

- (void)setControllerInputHandler:(GCController *)controller {
	// Hook in the callback handler for the correct gamepad profile.
	// This is a bit of a weird design choice on Apples part.
	// You need to select the most capable gamepad profile for the
	// gamepad attached.
	if (controller.extendedGamepad != nil) {
		// The extended gamepad profile has all the input you could possibly find on
		// a gamepad but will only be active if your gamepad actually has all of
		// these...
		controller.extendedGamepad.valueChangedHandler = ^(
				GCExtendedGamepad *gamepad, GCControllerElement *element) {
			int joy_id = [self getJoyIdForController:controller];

			if (element == gamepad.buttonA) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_BUTTON_0,
						gamepad.buttonA.isPressed);
			} else if (element == gamepad.buttonB) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_BUTTON_1,
						gamepad.buttonB.isPressed);
			} else if (element == gamepad.buttonX) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_BUTTON_2,
						gamepad.buttonX.isPressed);
			} else if (element == gamepad.buttonY) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_BUTTON_3,
						gamepad.buttonY.isPressed);
			} else if (element == gamepad.leftShoulder) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_L,
						gamepad.leftShoulder.isPressed);
			} else if (element == gamepad.rightShoulder) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_R,
						gamepad.rightShoulder.isPressed);
			} else if (element == gamepad.leftTrigger) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_L2,
						gamepad.leftTrigger.isPressed);
			} else if (element == gamepad.rightTrigger) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_R2,
						gamepad.rightTrigger.isPressed);
			} else if (element == gamepad.dpad) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_UP,
						gamepad.dpad.up.isPressed);
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_DOWN,
						gamepad.dpad.down.isPressed);
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_LEFT,
						gamepad.dpad.left.isPressed);
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_RIGHT,
						gamepad.dpad.right.isPressed);
			};

			InputDefault::JoyAxis jx;
			jx.min = -1;
			if (element == gamepad.leftThumbstick) {
				jx.value = gamepad.leftThumbstick.xAxis.value;
				OSIPhone::get_singleton()->joy_axis(joy_id, JOY_ANALOG_LX, jx);
				jx.value = -gamepad.leftThumbstick.yAxis.value;
				OSIPhone::get_singleton()->joy_axis(joy_id, JOY_ANALOG_LY, jx);
			} else if (element == gamepad.rightThumbstick) {
				jx.value = gamepad.rightThumbstick.xAxis.value;
				OSIPhone::get_singleton()->joy_axis(joy_id, JOY_ANALOG_RX, jx);
				jx.value = -gamepad.rightThumbstick.yAxis.value;
				OSIPhone::get_singleton()->joy_axis(joy_id, JOY_ANALOG_RY, jx);
			} else if (element == gamepad.leftTrigger) {
				jx.value = gamepad.leftTrigger.value;
				OSIPhone::get_singleton()->joy_axis(joy_id, JOY_ANALOG_L2, jx);
			} else if (element == gamepad.rightTrigger) {
				jx.value = gamepad.rightTrigger.value;
				OSIPhone::get_singleton()->joy_axis(joy_id, JOY_ANALOG_R2, jx);
			};
		};
	} else if (controller.gamepad != nil) {
		// gamepad is the standard profile with 4 buttons, shoulder buttons and a
		// D-pad
		controller.gamepad.valueChangedHandler = ^(GCGamepad *gamepad,
				GCControllerElement *element) {
			int joy_id = [self getJoyIdForController:controller];

			if (element == gamepad.buttonA) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_BUTTON_0,
						gamepad.buttonA.isPressed);
			} else if (element == gamepad.buttonB) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_BUTTON_1,
						gamepad.buttonB.isPressed);
			} else if (element == gamepad.buttonX) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_BUTTON_2,
						gamepad.buttonX.isPressed);
			} else if (element == gamepad.buttonY) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_BUTTON_3,
						gamepad.buttonY.isPressed);
			} else if (element == gamepad.leftShoulder) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_L,
						gamepad.leftShoulder.isPressed);
			} else if (element == gamepad.rightShoulder) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_R,
						gamepad.rightShoulder.isPressed);
			} else if (element == gamepad.dpad) {
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_UP,
						gamepad.dpad.up.isPressed);
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_DOWN,
						gamepad.dpad.down.isPressed);
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_LEFT,
						gamepad.dpad.left.isPressed);
				OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_RIGHT,
						gamepad.dpad.right.isPressed);
			};
		};
#ifdef ADD_MICRO_GAMEPAD // disabling this for now, only available on iOS 9+,
		// while we are setting that as the minimum, seems our
		// build environment doesn't like it
	} else if (controller.microGamepad != nil) {
		// micro gamepads were added in OS 9 and feature just 2 buttons and a d-pad
		controller.microGamepad.valueChangedHandler =
				^(GCMicroGamepad *gamepad, GCControllerElement *element) {
					int joy_id = [self getJoyIdForController:controller];

					if (element == gamepad.buttonA) {
						OSIPhone::get_singleton()->joy_button(joy_id, JOY_BUTTON_0,
								gamepad.buttonA.isPressed);
					} else if (element == gamepad.buttonX) {
						OSIPhone::get_singleton()->joy_button(joy_id, JOY_BUTTON_2,
								gamepad.buttonX.isPressed);
					} else if (element == gamepad.dpad) {
						OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_UP,
								gamepad.dpad.up.isPressed);
						OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_DOWN,
								gamepad.dpad.down.isPressed);
						OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_LEFT,
								gamepad.dpad.left.isPressed);
						OSIPhone::get_singleton()->joy_button(joy_id, JOY_DPAD_RIGHT,
								gamepad.dpad.right.isPressed);
					};
				};
#endif
	};

	///@TODO need to add support for controller.motion which gives us access to
	/// the orientation of the device (if supported)

	///@TODO need to add support for controllerPausedHandler which should be a
	/// toggle
};

- (void)initGameControllers {
	// get told when controllers connect, this will be called right away for
	// already connected controllers
	[[NSNotificationCenter defaultCenter]
			addObserver:self
			   selector:@selector(controllerWasConnected:)
				   name:GCControllerDidConnectNotification
				 object:nil];

	// get told when controllers disconnect
	[[NSNotificationCenter defaultCenter]
			addObserver:self
			   selector:@selector(controllerWasDisconnected:)
				   name:GCControllerDidDisconnectNotification
				 object:nil];
};

- (void)deinitGameControllers {
	[[NSNotificationCenter defaultCenter]
			removeObserver:self
					  name:GCControllerDidConnectNotification
					object:nil];
	[[NSNotificationCenter defaultCenter]
			removeObserver:self
					  name:GCControllerDidDisconnectNotification
					object:nil];

	if (ios_joysticks != nil) {
		[ios_joysticks dealloc];
		ios_joysticks = nil;
	};

	if (pending_ios_joysticks != nil) {
		[pending_ios_joysticks dealloc];
		pending_ios_joysticks = nil;
	};
};

OS::VideoMode _get_video_mode() {
	int backingWidth;
	int backingHeight;
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
			GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
			GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);

	OS::VideoMode vm;
	vm.fullscreen = true;
	vm.width = backingWidth;
	vm.height = backingHeight;
	vm.resizable = false;
	return vm;
};

static int frame_count = 0;
- (void)drawView:(GLView *)view;
{

	switch (frame_count) {
		case 0: {
			OS::get_singleton()->set_video_mode(_get_video_mode());

			if (!OS::get_singleton()) {
				exit(0);
			};
			++frame_count;

			NSString *locale_code = [[NSLocale currentLocale] localeIdentifier];
			OSIPhone::get_singleton()->set_locale(
					String::utf8([locale_code UTF8String]));

			NSString *uuid;
			if ([[UIDevice currentDevice]
						respondsToSelector:@selector(identifierForVendor)]) {
				uuid = [UIDevice currentDevice].identifierForVendor.UUIDString;
			} else {
				// before iOS 6, so just generate an identifier and store it
				uuid = [[NSUserDefaults standardUserDefaults]
						objectForKey:@"identiferForVendor"];
				if (!uuid) {
					CFUUIDRef cfuuid = CFUUIDCreate(NULL);
					uuid = (__bridge_transfer NSString *)CFUUIDCreateString(NULL, cfuuid);
					CFRelease(cfuuid);
					[[NSUserDefaults standardUserDefaults]
							setObject:uuid
							   forKey:@"identifierForVendor"];
				}
			}

			OSIPhone::get_singleton()->set_unique_id(String::utf8([uuid UTF8String]));

		}; break;

		case 1: {

			Main::setup2();
			++frame_count;

			if (pending_ios_joysticks != nil) {
				for (GCController *controller in pending_ios_joysticks) {
					_ios_add_joystick(controller, self);
				}
				[pending_ios_joysticks dealloc];
				pending_ios_joysticks = nil;
			}

			// this might be necessary before here
			NSDictionary *dict = [[NSBundle mainBundle] infoDictionary];
			for (NSString *key in dict) {
				NSObject *value = [dict objectForKey:key];
				String ukey = String::utf8([key UTF8String]);

				// we need a NSObject to Variant conversor

				if ([value isKindOfClass:[NSString class]]) {
					NSString *str = (NSString *)value;
					String uval = String::utf8([str UTF8String]);

					ProjectSettings::get_singleton()->set("Info.plist/" + ukey, uval);

				} else if ([value isKindOfClass:[NSNumber class]]) {

					NSNumber *n = (NSNumber *)value;
					double dval = [n doubleValue];

					ProjectSettings::get_singleton()->set("Info.plist/" + ukey, dval);
				};
				// do stuff
			}

		}; break;

		case 2: {

			Main::start();
			++frame_count;

		}; break; // no fallthrough

		default: {
			if (OSIPhone::get_singleton()) {
				// OSIPhone::get_singleton()->update_accelerometer(accel[0], accel[1],
				// accel[2]);
				if (motionInitialised) {
					// Just using polling approach for now, we can set this up so it sends
					// data to us in intervals, might be better. See Apple reference pages
					// for more details:
					// https://developer.apple.com/reference/coremotion/cmmotionmanager?language=objc

					// Apple splits our accelerometer date into a gravity and user movement
					// component. We add them back together
					CMAcceleration gravity = motionManager.deviceMotion.gravity;
					CMAcceleration acceleration =
							motionManager.deviceMotion.userAcceleration;

					///@TODO We don't seem to be getting data here, is my device broken or
					/// is this code incorrect?
					CMMagneticField magnetic =
							motionManager.deviceMotion.magneticField.field;

					///@TODO we can access rotationRate as a CMRotationRate variable
					///(processed date) or CMGyroData (raw data), have to see what works
					/// best
					CMRotationRate rotation = motionManager.deviceMotion.rotationRate;

					// Adjust for screen orientation.
					// [[UIDevice currentDevice] orientation] changes even if we've fixed
					// our orientation which is not a good thing when you're trying to get
					// your user to move the screen in all directions and want consistent
					// output

					///@TODO Using [[UIApplication sharedApplication] statusBarOrientation]
					/// is a bit of a hack. Godot obviously knows the orientation so maybe
					/// we
					// can use that instead? (note that left and right seem swapped)

					switch ([[UIApplication sharedApplication] statusBarOrientation]) {
						case UIDeviceOrientationLandscapeLeft: {
							OSIPhone::get_singleton()->update_gravity(-gravity.y, gravity.x,
									gravity.z);
							OSIPhone::get_singleton()->update_accelerometer(
									-(acceleration.y + gravity.y), (acceleration.x + gravity.x),
									acceleration.z + gravity.z);
							OSIPhone::get_singleton()->update_magnetometer(
									-magnetic.y, magnetic.x, magnetic.z);
							OSIPhone::get_singleton()->update_gyroscope(-rotation.y, rotation.x,
									rotation.z);
						}; break;
						case UIDeviceOrientationLandscapeRight: {
							OSIPhone::get_singleton()->update_gravity(gravity.y, -gravity.x,
									gravity.z);
							OSIPhone::get_singleton()->update_accelerometer(
									(acceleration.y + gravity.y), -(acceleration.x + gravity.x),
									acceleration.z + gravity.z);
							OSIPhone::get_singleton()->update_magnetometer(
									magnetic.y, -magnetic.x, magnetic.z);
							OSIPhone::get_singleton()->update_gyroscope(rotation.y, -rotation.x,
									rotation.z);
						}; break;
						case UIDeviceOrientationPortraitUpsideDown: {
							OSIPhone::get_singleton()->update_gravity(-gravity.x, gravity.y,
									gravity.z);
							OSIPhone::get_singleton()->update_accelerometer(
									-(acceleration.x + gravity.x), (acceleration.y + gravity.y),
									acceleration.z + gravity.z);
							OSIPhone::get_singleton()->update_magnetometer(
									-magnetic.x, magnetic.y, magnetic.z);
							OSIPhone::get_singleton()->update_gyroscope(-rotation.x, rotation.y,
									rotation.z);
						}; break;
						default: { // assume portrait
							OSIPhone::get_singleton()->update_gravity(gravity.x, gravity.y,
									gravity.z);
							OSIPhone::get_singleton()->update_accelerometer(
									acceleration.x + gravity.x, acceleration.y + gravity.y,
									acceleration.z + gravity.z);
							OSIPhone::get_singleton()->update_magnetometer(magnetic.x, magnetic.y,
									magnetic.z);
							OSIPhone::get_singleton()->update_gyroscope(rotation.x, rotation.y,
									rotation.z);
						}; break;
					};
				}

				bool quit_request = OSIPhone::get_singleton()->iterate();
			};

		}; break;
	};
};

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
	if (OS::get_singleton()->get_main_loop()) {
		OS::get_singleton()->get_main_loop()->notification(
				MainLoop::NOTIFICATION_OS_MEMORY_WARNING);
	}
};

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	CGRect rect = [[UIScreen mainScreen] bounds];

	is_focus_out = false;

	[application setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
	// disable idle timer
	// application.idleTimerDisabled = YES;

	// Create a full-screen window
	window = [[UIWindow alloc] initWithFrame:rect];

	OS::VideoMode vm = _get_video_mode();

	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
			NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];

	int err = iphone_main(vm.width, vm.height, gargc, gargv, String::utf8([documentsDirectory UTF8String]));
	if (err != 0) {
		// bail, things did not go very well for us, should probably output a message on screen with our error code...
		exit(0);
		return FALSE;
	};

	// WARNING: We must *always* create the GLView after we have constructed the
	// OS with iphone_main. This allows the GLView to access project settings so
	// it can properly initialize the OpenGL context
	GLView *glView = [[GLView alloc] initWithFrame:rect];
	glView.delegate = self;

	view_controller = [[ViewController alloc] init];
	view_controller.view = glView;
	window.rootViewController = view_controller;

	_set_keep_screen_on(bool(GLOBAL_DEF("display/window/energy_saving/keep_screen_on", true)) ? YES : NO);
	glView.useCADisplayLink =
			bool(GLOBAL_DEF("display.iOS/use_cadisplaylink", true)) ? YES : NO;
	printf("cadisaplylink: %d", glView.useCADisplayLink);
	glView.animationInterval = 1.0 / kRenderingFrequency;
	[glView startAnimation];

	// Show the window
	[window makeKeyAndVisible];

	// Configure and start accelerometer
	if (!motionInitialised) {
		motionManager = [[CMMotionManager alloc] init];
		if (motionManager.deviceMotionAvailable) {
			motionManager.deviceMotionUpdateInterval = 1.0 / 70.0;
			[motionManager startDeviceMotionUpdatesUsingReferenceFrame:
								   CMAttitudeReferenceFrameXMagneticNorthZVertical];
			motionInitialised = YES;
		};
	};

	[self initGameControllers];

	[[NSNotificationCenter defaultCenter]
			addObserver:self
			   selector:@selector(onAudioInterruption:)
				   name:AVAudioSessionInterruptionNotification
				 object:[AVAudioSession sharedInstance]];

	// OSIPhone::screen_width = rect.size.width - rect.origin.x;
	// OSIPhone::screen_height = rect.size.height - rect.origin.y;

	mainViewController = view_controller;

	// prevent to stop music in another background app
	[[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:nil];

	return TRUE;
};

- (void)onAudioInterruption:(NSNotification *)notification {
	if ([notification.name isEqualToString:AVAudioSessionInterruptionNotification]) {
		if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] isEqualToNumber:[NSNumber numberWithInt:AVAudioSessionInterruptionTypeBegan]]) {
			NSLog(@"Audio interruption began");
			on_focus_out(view_controller, &is_focus_out);
		} else if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] isEqualToNumber:[NSNumber numberWithInt:AVAudioSessionInterruptionTypeEnded]]) {
			NSLog(@"Audio interruption ended");
			on_focus_in(view_controller, &is_focus_out);
		}
	}
};

- (void)applicationWillTerminate:(UIApplication *)application {
	[self deinitGameControllers];

	if (motionInitialised) {
		///@TODO is this the right place to clean this up?
		[motionManager stopDeviceMotionUpdates];
		[motionManager release];
		motionManager = nil;
		motionInitialised = NO;
	};

	iphone_finish();
};

// When application goes to background (e.g. user switches to another app or presses Home),
// then applicationWillResignActive -> applicationDidEnterBackground are called.
// When user opens the inactive app again,
// applicationWillEnterForeground -> applicationDidBecomeActive are called.

// There are cases when applicationWillResignActive -> applicationDidBecomeActive
// sequence is called without the app going to background. For example, that happens
// if you open the app list without switching to another app or open/close the
// notification panel by swiping from the upper part of the screen.

- (void)applicationWillResignActive:(UIApplication *)application {
	on_focus_out(view_controller, &is_focus_out);
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	on_focus_in(view_controller, &is_focus_out);
}

- (void)dealloc {
	[window release];
	[super dealloc];
}

@end
