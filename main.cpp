#include <stdio.h>

#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat inputImage(std::string name)
{
    cv::Mat image = cv::imread("images/" + name, cv::IMREAD_COLOR); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)

    if (image.empty())
    {
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
    }

    return image;
}

cv::Mat grayscale(cv::Mat src_8bit_unchar_img)
{

    cv::Mat gray_8uc1_img; // declare variable to hold grayscale version of img variable, gray levels wil be represented using 8 bits (uchar)

    cv::cvtColor(src_8bit_unchar_img, gray_8uc1_img, cv::COLOR_BGR2GRAY); // convert input color image to grayscale one, CV_BGR2GRAY specifies direction of conversion

    return gray_8uc1_img;
}

cv::Mat convert(cv::Mat image, bool unchar_8bit_to_32)
{
    cv::Mat dest_img; // declare variable to hold grayscale version of img variable, gray levels wil be represented using 32 bits (float)

    if (unchar_8bit_to_32)
    {
        image.convertTo(dest_img, CV_32FC1, 1.0 / 255.0); // convert image from 8 bits to 32 bits, resulting values will be in the interval 0.0 - 1.0
    }
    else
    {
        image.convertTo(dest_img, CV_8UC1, 1.0 / 255.0); // convert image from 32 bits to 8 bits, resulting values will be in the interval 0 - 255
    }

    return dest_img;
}

cv::Mat gammaCorretion(cv::Mat image, float gamma_value) 
{
    cv::Mat corrected_image(image.rows, image.cols, CV_32FC1);

    for (int x = 0; x < image.rows; x++)
    {
        for (int y = 0; y < image.cols; y++)
        {   
            corrected_image.at<float>(x,y)= pow(image.at<float>(x, y) , 1/gamma_value);
        }
    }

    return corrected_image;
}

int main()
{
    cv::Mat src_8bit_unchar_img = inputImage("moon.jpg");

    cv::Mat gray_8bit_unchar_img = grayscale(src_8bit_unchar_img);
    cv::Mat gray_32bit_float_img = convert(gray_8bit_unchar_img, true);

    cv::Mat correctedImage_32bit = gammaCorretion(gray_32bit_float_img, 0.75);
    //cv::Mat correctedImage_8bit = convert(correctedImage_32bit, false);

    // diplay images
    // cv::imshow("Gradient", gradient_8uc1_img);
    cv::imshow("gray 32f", gray_32bit_float_img);
    cv::imshow("corrected 32f", correctedImage_32bit);

    cv::waitKey(0); // wait until keypressed

    return 0;
}
