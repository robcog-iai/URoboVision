URoboVision
=====

### UE Version: **5**

Plugin with a CameraActor that captures RGB-D data and sends it out over TCP.

Usage
=====

- Add the plugin to your project (e.g `MyProject/Plugins/URoboVision`)  
    
- Drag and drop `RGBDCamera` to your level

- Set parameters in the `RGB-D Settings`

- Use the [ROS bridge](https://github.com/sherpa-eu/unreal_vision_bridge) for publishing the topics
	

Credits
====

Based on the [UnrealCV](http://unrealcv.org/) project, modified by Thiemo Wiedemeyer to be able to send RGB-D data over the network, accesable by [RoboSherlock](http://robosherlock.org/).
