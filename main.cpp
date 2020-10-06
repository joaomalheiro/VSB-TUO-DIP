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

cv::Mat convert(const cv::Mat &image, bool unchar_8bit_to_32)
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

cv::Mat gammaCorretion(const cv::Mat &image, float gamma_value) 
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

cv::Mat contrast(const cv:: Mat &image) 
{
    double min, max; 
    minMaxLoc(image, &min, &max);

    cv::Mat stretchImage(image.rows, image.cols, CV_32FC1);

    double newMax = max - min;
    double value = 1.0/newMax;

    for (int x = 0; x < image.rows; x++)
    {
        for (int y = 0; y < image.cols; y++)
        {   
            stretchImage.at<float>(x,y)= (image.at<float>(x, y) - min) * value;
        }
    }

    return stretchImage;
}

cv::Mat convolution(cv:: Mat image) {
    cv::Mat newImage = image.clone();
    double value = 1.0/16.0;
    double gaussian[3][3] = {{value* 1.0, value*2.0, value*1.0}, {value*2.0,value * 4.0, value* 2.0}, {value*1-0, value*2.0, value*1.0}};
    
    for (int x = 1; x < newImage.rows - 1; x++)
    {
        for (int y = 1; y < newImage.cols - 1; y++)
        {   
            double sum = 0;

            for(int a = -1; a < 2; a++) {

                for(int b = -1; b < 2; b++) {
                    sum = sum + newImage.at<float>(x + a, y + b) * gaussian[a + 1][b + 1];
                }
            }

            newImage.at<float>(x,y) = sum;
        }
    }
    
    return newImage;
    
}

cv::Mat anisotropic(const cv:: Mat &image){
    
    cv::Mat newImage = image.clone(); 

    for(int k = 0 ; k<1000 ; k++) {

        for (int i = 1;i<image.rows-1;i++){
            for (int j = 1;j<image.cols-1;j++){
                float cN, cS, cE, cW;
                float deltacN, deltacS, deltacE, deltacW;

                deltacN = newImage.at<float>(i,j-1) - newImage.at<float>(i,j);
                deltacS = newImage.at<float>(i,j+1) - newImage.at<float>(i,j);
                deltacE = newImage.at<float>(i+1,j) - newImage.at<float>(i,j);
                deltacW = newImage.at<float>(i-1,j) - newImage.at<float>(i,j);

                cN = abs(exp(-1 * (deltacN * deltacN / (0.015 * 0.015) )));
                cS = abs(exp(-1 * (deltacS * deltacS / (0.015 * 0.015) )));
                cE = abs(exp(-1 * (deltacE * deltacE / (0.015 * 0.015) )));
                cW = abs(exp(-1 * (deltacW * deltacW / (0.015 * 0.015) )));

                newImage.at<float>(i,j) =  newImage.at<float>(i,j) * ( 1 - 0.1 *(cN + cS + cE + cW)  )+
                        0.1 *( cN * newImage.at<float>(i,j-1) + cS * newImage.at<float>(i,j+1)
                            + cE * newImage.at<float>(i+1,j) + cW * newImage.at<float>(i-1,j) );
            }
        }
    }

    return newImage;
   
}

int main()
{
    cv::Mat src_8bit_unchar_img = inputImage("valve.png");

    cv::Mat gray_8bit_unchar_img = grayscale(src_8bit_unchar_img);
    cv::Mat gray_32bit_float_img = convert(gray_8bit_unchar_img, true);

    cv::Mat correctedImage_32bit = gammaCorretion(gray_32bit_float_img, 0.75);

    cv::Mat contrastImage_32bit = contrast(gray_32bit_float_img);

   // cv::Mat convolutionImage_32bit = convolution(gray_32bit_float_img);
    cv::Mat anisotropicImage_32bit = anisotropic(gray_32bit_float_img);
    //cv::Mat correctedImage_8bit = convert(correctedImage_32bit, false);

    // diplay images
    // cv::imshow("Gradient", gradient_8uc1_img);
    cv::imshow("gray 32f", gray_32bit_float_img);
    // cv::imshow("corrected 32f", correctedImage_32bit);
    // cv::imshow("contrast 32f", contrastImage_32bit);
    // cv::imshow("convolution 32f", convolutionImage_32bit);
    cv::imshow("anisotropic 32f", anisotropicImage_32bit);


    cv::waitKey(0); // wait until keypressed

    return 0;
}
