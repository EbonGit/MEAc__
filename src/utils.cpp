#include <numeric>
#include "utils.h"

std::vector<Point> floatsToPoints(std::vector<float> floats, int width, int height, float max, float min) {
    std::vector<Point> points;
    int step = width / floats.size();
    if (std::isnan(max)) {
        max = *std::max_element(floats.begin(), floats.end());
    }
    if (std::isnan(min)) {
        min = *std::min_element(floats.begin(), floats.end());
    }

    float range = std::max(std::abs(max), std::abs(min));

    for (int i = 0; i < floats.size(); i++) {
        // Normalize the value to the [0, 1] range based on min and max
        float normalizedY = (floats[i] + range) / (2 * range);
        float InversedNormalizedY = 1 - normalizedY;
        int y = std::floor((height - 1) * InversedNormalizedY);

        points.push_back({i * step, y});
    }
    return points;
}

std::vector<std::vector<Point>> vecFloatsToVecPoints(std::vector<std::vector<float>> floats, int width, int height) {
    std::vector<std::vector<Point>> points;
    float globalMax = -INFINITY;
    float globalMin = INFINITY;

    for (auto & i : floats) {
        float max = *std::max_element(i.begin(), i.end(), [](float a, float b) { return a < b; });
        if (max > globalMax) {
            globalMax = max;
        }
        float min = *std::min_element(i.begin(), i.end(), [](float a, float b) { return a < b; });
        if (min < globalMin) {
            globalMin = min;
        }
    }

    for (const auto & i : floats) {
        points.push_back(floatsToPoints(i, width, height, globalMax, globalMin));
    }
    return points;
}

std::vector<float> selectEnd(std::vector<float> floats, int nbPoints) {
    int count = std::min(static_cast<int>(floats.size()), nbPoints);
    return std::vector<float>(floats.end() - count, floats.end());
}

// Helper function to set pixel color in the OpenCV Mat (optimized)
void setPixel(cv::Mat& image, int x, int y, const Color& color) {
    if (x >= 0 && x < image.cols && y >= 0 && y < image.rows) {
        // Directly access the pixel using a pointer for faster access
        cv::Vec3b* pixel = image.ptr<cv::Vec3b>(y, x);
        (*pixel)[0] = color.b;
        (*pixel)[1] = color.g;
        (*pixel)[2] = color.r;
    }
}

// Linear interpolation between two points (optimized)
void drawLine(cv::Mat& image, Point p1, Point p2, Color color, int offsetX, int offsetY) {
    // Offset points
    p1.x += offsetX;
    p1.y += offsetY;
    p2.x += offsetX;
    p2.y += offsetY;

    if ((p1.x < 0 && p2.x < 0) || (p1.x >= image.cols && p2.x >= image.cols) ||
        (p1.y < 0 && p2.y < 0) || (p1.y >= image.rows && p2.y >= image.rows)) {
        return;
    }

    // Bresenham's line algorithm
    int dx = std::abs(p2.x - p1.x);
    int dy = std::abs(p2.y - p1.y);
    int sx = (p1.x < p2.x) ? 1 : -1;
    int sy = (p1.y < p2.y) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        setPixel(image, p1.x, p1.y, color);  // Set pixel

        if (p1.x == p2.x && p1.y == p2.y) break;  // End if we reach the destination

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            p1.x += sx;
        }
        if (e2 < dx) {
            err += dx;
            p1.y += sy;
        }
    }
}

void drawAxes(cv::Mat& image, Color axesColor, int height, int width) {
    int axisThickness = 1;

    // Draw the X-axis (horizontal line in the middle)
    cv::line(image, cv::Point(0, height / 2), cv::Point(width, height / 2),
             cv::Scalar(axesColor.b, axesColor.g, axesColor.r), axisThickness);
    // Draw the Y-axis (vertical line in the middle)
    cv::line(image, cv::Point(width / 2, 0), cv::Point(width / 2, height),
             cv::Scalar(axesColor.b, axesColor.g, axesColor.r), axisThickness);
    // Draw ticks on the axes (optional)
    int tickLength = 5;
    int numTicks = 10;  // Adjust the number of ticks
    for (int i = 1; i <= numTicks; ++i) {
        // X-axis ticks
        int xTickPos = i * (width / (numTicks + 1));
        cv::line(image, cv::Point(xTickPos, height / 2 - tickLength), cv::Point(xTickPos, height / 2 + tickLength), cv::Scalar(axesColor.b, axesColor.g, axesColor.r), 1);

        // Y-axis ticks
        int yTickPos = i * (height / (numTicks + 1));
        cv::line(image, cv::Point(width / 2 - tickLength, yTickPos), cv::Point(width / 2 + tickLength, yTickPos), cv::Scalar(axesColor.b, axesColor.g, axesColor.r), 1);
    }
}

void drawLabel(cv::Mat& image, const std::string& label, Point position, Color color, double fontScale, int thickness, bool isCentered) {
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    int baseline = 0;
    cv::Size textSize = cv::getTextSize(label, fontFace, fontScale, thickness, &baseline);

    cv::Point textPosition(position.x, position.y);
    if (isCentered) {
        textPosition.x -= textSize.width / 2;
        textPosition.y += textSize.height / 2;
    }

    cv::putText(image, label, textPosition, fontFace, fontScale, cv::Scalar(color.b, color.g, color.r), thickness);
}

cv::Mat plotPoints(const std::vector<Point>& points, int width, int height, bool randomColor, Color color, Color bgColor, bool axes, cv::Mat baseImage) {
    // Initialize a black image using OpenCV Mat
    cv::Mat image;
    if (baseImage.empty()) {
        image = cv::Mat(height, width, CV_8UC3, cv::Scalar(bgColor.b, bgColor.g, bgColor.r));
    }
    else {
        image = baseImage.clone();
    }

    if (axes) {
        Color axesColor = {100, 100, 100};
        drawAxes(image, axesColor, height, width);
    }

    for (size_t i = 1; i < points.size(); ++i) {
        if (randomColor) {
            color = Color{rand() % 256, rand() % 256, rand() % 256};
        }
        drawLine(image, points[i - 1], points[i], color);
    }

    return image;
}

float generatePoint(float lastPoint) {
    if (lastPoint != -1) {
        auto variation = rand() % 10;
        if (rand() % 2 == 0) {
            variation *= -1;
        }
        int newValue = static_cast<int>(lastPoint) + variation;

        return (float)((newValue % 4096 + 4096) % 4096);
    }
    return (float)(rand() % 4096);
}

float generateSinusoidalPoint(long t, float amplitude) {
    return (std::sin(t / 10.f)) * amplitude;
}

float generateSquareWavePoint(long t, float amplitude) {
    return generateSinusoidalPoint(t, amplitude) > 0 ? amplitude : -amplitude;
}

float generateSpikePoint(long t, float amplitude) {
    return (t % 100 == 0 or (t+1) % 100 == 0) ? amplitude : 0;
}

float addNoise(float value, float noise) {
    if (rand() % 2 == 0) {
        return value - (rand() % 100) * noise;
    }
    return value + (rand() % 100) * noise;
}

std::vector<float> generatePoints(int numPoints, int width) {
    std::vector<float> points;
    int step = width / numPoints;
    for (int i = 0; i < numPoints; i++) {
        float random = generatePoint();
        points.push_back(random);
    }
    return points;
}

cv::Mat concatenateImages(std::vector<cv::Mat> images) {
    int n = images.size();
    int sqrt = std::ceil(std::sqrt(n));
    int missingImages = sqrt * sqrt - n;

    for (uint8_t i = 0; i < missingImages; i++) {
        images.push_back(cv::Mat::zeros(images[0].size(), images[0].type()));
    }

    int imgWidth = images[0].cols;
    int imgHeight = images[0].rows;

    cv::Mat concatenatedImage(imgHeight * sqrt, imgWidth * sqrt, images[0].type());

    cv::parallel_for_(cv::Range(0, sqrt), [&](const cv::Range& range) {
        for (int i = range.start; i < range.end; ++i) {
            for (int j = 0; j < sqrt; ++j) {
                cv::Mat roi = concatenatedImage(cv::Rect(j * imgWidth, i * imgHeight, imgWidth, imgHeight));
                images[i * sqrt + j].copyTo(roi);
            }
        }
    });

    for (int i = 1; i < sqrt; ++i) {
        cv::line(concatenatedImage, cv::Point(0, i * imgHeight),
                 cv::Point(concatenatedImage.cols, i * imgHeight),
                 cv::Scalar(255, 255, 255), 1);  // Horizontal white line

        cv::line(concatenatedImage, cv::Point(i * imgWidth, 0),
                 cv::Point(i * imgWidth, concatenatedImage.rows),
                 cv::Scalar(255, 255, 255), 1);  // Vertical white line
    }

    return concatenatedImage;
}

cv::Vec3b valueToColor(float value, float min, float max) {
    float distance_to_min = std::abs(value - min);
    float distance_to_max = std::abs(value - max);

    float range = std::abs(min) + std::abs(max);

    int blue = 255 * std::pow(distance_to_min / range, 4);
    int red = 255 * std::pow(distance_to_max / range, 4);


    return cv::Vec3b(blue, 0, red);
}

cv::Mat generateHeatmap(const std::vector<float>& data, int caseSize, float minValue, float maxValue, bool showLabels) {
    int n = std::ceil(std::sqrt(data.size()));  // Get the size of the square matrix (n x n
    int width = n * caseSize;
    int height = n * caseSize;
    double fontSize = 0.25;
    int caseSizeHalf = caseSize / 2;

    std::vector<float> dataCopy = data;

    for (int i = n*n - data.size(); i > 0; i--) {
        dataCopy.push_back(0);
    }

    cv::Mat heatmap = cv::Mat::zeros(height, width, CV_8UC3);  // Create blank image (3 channels for color)

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            // Get the value corresponding to the current cell
            float value = dataCopy[i * n + j];

            // Get color based on value
            cv::Scalar color = valueToColor(value, minValue, maxValue);

            // Draw the rectangle for each cell
            cv::rectangle(heatmap,
                          cv::Point(j * caseSize, i * caseSize),
                          cv::Point((j + 1) * caseSize, (i + 1) * caseSize),
                          color, cv::FILLED);
            int index = i * n + j;
            drawLabel(heatmap, std::to_string((int)index), Point(j * caseSize + caseSizeHalf, i * caseSize + caseSizeHalf), {255, 255, 255}, fontSize, 1, true);
        }
    }

    return heatmap;
}

Point::Point(int i1, int i2) {
    x = i1;
    y = i2;
}

MEA_Params::MEA_Params(int i1, int i2, int i3, int i4, int i5) {
    width = i1;
    height = i2;
    numPoints = i3;
    numImages = i4;
    signalsBufferSize = i5;
}

Window::Window(int i1, int i2, const std::string& s, int i3) {
    width = i1;
    height = i2;
    name = s;
    index = i3;
}

FPS::FPS() {
    frames = {0.0f};
}

void FPS::addFrame(float frame) {
    if (frames.size() >= limit) {
        frames.erase(frames.begin());
    }
    frames.push_back(frame);
}

int FPS::getFPS() {
    float somme = 0.0f;
    for (float frame : frames) {
        somme += frame;
    }
    return static_cast<int>(somme / (float)frames.size());
}

int getImageIndex(int x, int y, int numImages, int width, int height) {
    int nearSqrt = std::ceil(std::sqrt(numImages));
    int totalWidth = width * nearSqrt;
    int totalHeight = height * nearSqrt;

    if (x < 0 || x >= totalWidth || y < 0 || y >= totalHeight) {
        return -1;
    }

    int xCoord = std::floor((float)x / (float)width);
    int yCoord = std::floor((float)y / (float)height);

    return yCoord * nearSqrt + xCoord;
}

double mean(const std::vector<float>& data, int start, int end) {
    return std::accumulate(data.begin() + start, data.begin() + end, 0.0) / (end - start);
}

double stdDev(const std::vector<float>& data, int start, int end) {
    double m = mean(data, start, end);
    double sum = 0.0;
    for (int i = start; i < end; ++i) {
        sum += std::pow(data[i] - m, 2);
    }
    return std::sqrt(sum / (end - start));
}

ThresholdingResult thresholding_algo(const std::vector<float>& y, int lag, double threshold, double influence) {
    int n = y.size();
    ThresholdingResult result;
    result.signals.resize(n, 0);
    result.avgFilter.resize(n, 0.0);
    result.stdFilter.resize(n, 0.0);

    std::vector<float> filteredY = y;

    // Initial mean and std deviation for the first lag elements
    result.avgFilter[lag - 1] = mean(y, 0, lag);
    result.stdFilter[lag - 1] = stdDev(y, 0, lag);

    for (int i = lag; i < n; ++i) {
        if (std::abs(y[i] - result.avgFilter[i - 1]) > threshold * result.stdFilter[i - 1]) {
            result.signals[i] = (y[i] > result.avgFilter[i - 1]) ? 2000.0 : -2000.0;
            filteredY[i] = influence * y[i] + (1 - influence) * filteredY[i - 1];
        } else {
            result.signals[i] = 0;
            filteredY[i] = y[i];
        }

        result.avgFilter[i] = mean(filteredY, i - lag + 1, i + 1);
        result.stdFilter[i] = stdDev(filteredY, i - lag + 1, i + 1);
    }

    return result;
}
