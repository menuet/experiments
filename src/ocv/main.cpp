
#include <opencv2/opencv.hpp>
#include <cstdio>

int main(int argc, char** argv)
{
    const auto image_path = argc < 2 ? "assets/meuh.jpg" : argv[1];

    std::printf("Loading image\n");

    cv::Mat image = cv::imread(image_path, 1);
    if (!image.data)
    {
        std::printf("No image data\n");
        return -1;
    }

    std::printf("Displaying image\n");

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", image);
    cv::waitKey(0);

    return 0;
}
