![](Documentation/Img/UVisionLogger.gif)

# URoboVision

Unreal plugin with a CameraActor that captures Color,Mask,Depth and Normal Images. And it can also save all images in [MongoDB](https://www.mongodb.com/) and [Bson](http://bsonspec.org/) file.

# Usage
*  Add the plugin to your project (e.g `MyProject/Plugins/UVisionLogger`)
*  Add [libmongo](https://github.com/robcog-iai/libmongo) to your project (e.g `MyProject/Plugins/libmongo`)
*  Drag and drop `UVCamera` to your level.

    ![](Documentation/Img/UVCamera.PNG)
    
*  Set parameters in the [`Vision Settings`](Documentation/VisionSetting.md).

    ![](Documentation/Img/Setting.PNG)
    
*  Drag multiple `UVCamera` to capture different views simultaneously.

# Demo result

* After running, all results are saved in folder **Vision Logger**. 
* Each Camera will create a folder of itself. 
* All images captured will be saved in folder **viewport**.
* When mask image is captured, corresponding mask colors are saved as bson file in folder **Mask Color**. 
* When **Save as Bson file** is chosen, it will create a folder **Bson File** to save all images in bson file.
![](Documentation/Img/Demo_result.gif)

