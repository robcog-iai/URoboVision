import os
import bson
import numpy as np


def read_bson_file(file_path):
    file_names = []
    if os.path.isdir(file_path):
        for file in os.listdir(file_path):
            print(file)
            if file.endswith(".bson"):
                file_name = file
                file_names.append(file_name)

        if len(file_name):
            print("Read data from bson file")

        else:
            print("Don't contain any bson file in this folder")

    else:
        print("The given folder doesn't exist.Please give correct folder path")
    return file_names


def save_images_from_bson(FilePath, FileNames):
    if len(FileNames):
        for file in FileNames:
            data = bson.decode_file_iter(open(FilePath + file, 'rb'))
            save_dir = "../../Vision Logger/Bson_image_folder/" + file[:-5] + "/"
            if not os.path.exists(save_dir):
                os.makedirs(save_dir)
            while True:
                try:
                    data_dict = next(data)
                    keys = data_dict.keys()
                    camera_id = data_dict["Camera_Id"]
                    Time_stamp = data_dict["Time_Stamp"]
                    if "Color Image" in keys:
                        color_img_dict = data_dict["Color Image"]
                        color_img_data = color_img_dict["Color_Image_Data"]
                        img_type = color_img_dict["type"]
                        color_name = save_dir + str(camera_id) + "_" + str(img_type) + "_" + str(Time_stamp) + ".png"
                        with open(color_name, "wb") as f:
                            f.write(color_img_data)
                    if "Depth Image" in keys:
                        depth_img_dict = data_dict["Depth Image"]
                        depth_img_data = depth_img_dict["Depth_Image_Data"]
                        img_type = depth_img_dict["type"]
                        depth_name = save_dir + str(camera_id) + "_" + str(img_type) + "_" + str(Time_stamp) + ".png"
                        with open(depth_name, "wb") as f:
                            f.write(depth_img_data)
                    if "Mask Image" in keys:
                        mask_img_dict = data_dict["Mask Image"]
                        mask_img_data = mask_img_dict["Mask_Image_Data"]
                        img_type = mask_img_dict["type"]
                        mask_name = save_dir + str(camera_id) + "_" + str(img_type) + "_" + str(Time_stamp) + ".png"
                        with open(mask_name, "wb") as f:
                            f.write(mask_img_data)

                    if "Normal Image" in keys:
                        depth_img_dict = data_dict["Normal Image"]
                        depth_img_data = depth_img_dict["Normal_Image_Data"]
                        img_type = depth_img_dict["type"]
                        depth_name = save_dir + str(camera_id) + "_" + str(img_type) + "_" + str(Time_stamp) + ".png"
                        with open(depth_name, "wb") as f:
                            f.write(depth_img_data)
                    print("Saving image")
                except:
                    print("All images in {} has been saved in local disk".format(file))
                    break
    else:
        print("No bson file is found in this folder")

def Read_Save_Image_in_Bson(file_path):
    filenames = read_bson_file(file_path)
    save_images_from_bson(file_path,filenames)


if __name__ == '__main__':

    ### give the correct bson file folder path here
    file_path = os.path.expanduser("../../Vision Logger/2018_8_28_9_53/Cam01/Bson File/")

    print(os.path.abspath(file_path))

    Read_Save_Image_in_Bson(file_path)