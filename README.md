vdr-plugin-boblight
===================

This is a "plugin" for the Video Disk Recorder (VDR).

Boblight with data from softhddevice

Needs libboblight.so and boblightd configured and running (https://code.google.com/p/boblight)


Priority [128]			Every boblight client has a priority, the higher the lower		
Updaterate [15]			Updaterate in Hz, 15 => 15 Updates per second from softhddevice. Boblightd has it's own updaterate which "smoothenings" the output. Too high values have a massive impact on cpu load!
Detect cinema bars
Show mainmenu
Log level

Threshold [20]			0-255 Blacklevel detection, Black => LED's off
Gamma [10]				0-100 Gamma corretion (divided by 10, 10 = 1.0)
Value [80]				0-200 Saturation and value are multipliers for HSV color space (divided by 10, 10 = 1.0)
Saturation [30]			0-200 Saturation and value are multipliers for HSV color space (divided by 10, 10 = 1.0)
Speed [60]				0-100 Speed is a factor for a first order lowpass filter, the higher you set it the faster the lights react.
Autospeed [0]			0-100 Autospeed adjusts the speed on top of that based on how fast the colors are changing.
Interpolation [true]    Interpolation is a setting for a boblightd output device, 
						when it's on it will interpolate between the last two writes of a client. (Off might reduce load)

It's recommended to add the following option to your boblight.conf
```
[device]
allowsync off
```
