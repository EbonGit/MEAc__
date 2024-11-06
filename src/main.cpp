#include <chrono>
#include "MEA.h"
#include <thread>
#include "config.h"
#include "tcp.h"

MEA_Params params(width, height, numPoints, numImages, signalsBufferSize);

MEA_Info meaInfo(params);

void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        //MEA_Params params = static_cast<MEA_Info*>(userdata)->params;
        // Left button click
        std::cout << "Mouse click at: (" << x << ", " << y << ")" << std::endl;
        int indexImage = ::getImageIndex(x, y, params.numImages, params.width, params.height, meaInfo.mea.pinout);
        std::cout << "Selected signal: " << indexImage << std::endl;
        meaInfo.mea.selectSignal(indexImage);
    }
}

void windowCallback(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        // Left button click
        Window w = *static_cast<Window*>(userdata);
        std::cout << "Mouse click at: (" << x << ", " << y << ") in window " << w.index << std::endl;
        meaInfo.mea.setSelectedWindow(w.index);
    }
}

int main() {
    cv::namedWindow("MEA", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Heatmap", cv::WINDOW_AUTOSIZE);

    FPS fps;

    int numberOfWindows = 0;

    cv::setMouseCallback("MEA", mouseCallback, nullptr);

    auto previousTime = std::chrono::high_resolution_clock::now();

    while (true) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        cv::Mat image = meaInfo.mea.plotGrid();

        cv::Mat heatmap = meaInfo.mea.plotHeatmap(32);

        if (numberOfWindows > 0){
            std::vector<cv::Mat> selectedImage = meaInfo.mea.plotSelectedSignal(2);

            for (int i = 0; i < numberOfWindows; i++) {
                std::string windowName = "Window " + std::to_string(i);
                cv::imshow(windowName, selectedImage[i]);
            }
        }

        cv::imshow("Heatmap", heatmap);

        ::drawLabel(image, "FPS: " + std::to_string(fps.getFPS()), Point(5, 10), {0, 255, 0}, 0.3, 1, false);

        cv::imshow("MEA", image);

        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = frameEnd - frameStart;

        float frames = 1.0f / elapsed.count();
        fps.addFrame(frames);

        int key = cv::waitKeyEx(20);

        if (key == 'a') {
            numberOfWindows++;
            meaInfo.mea.addWindow();
            std::string windowName = "Window " + std::to_string(numberOfWindows-1);
            cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
            std::cout << "Window " << numberOfWindows-1 << " created" << std::endl;
            cv::setMouseCallback(windowName, windowCallback, new Window(64, 64, windowName, numberOfWindows-1));
        }

        if(key == 'p') {
            meaInfo.mea.add_i();
            int sum = meaInfo.mea.print_selected_window();
            std::cout << "i + Selected window = " << sum << std::endl;
        }

        if(key == 't') {
            meaInfo.mea.selectThresholdedSignal();
        }

        switch (key) {
            case 2490368: // Up arrow
                meaInfo.mea.setThreshold(meaInfo.mea.threshold + 1);
                break;
            case 2621440: // Down arrow
                meaInfo.mea.setThreshold(meaInfo.mea.threshold - 1);
                break;
            case 2424832: // Left arrow
                meaInfo.mea.setLag(meaInfo.mea.lag - 1);
                break;
            case 2555904: // Right arrow
                meaInfo.mea.setLag(meaInfo.mea.lag + 1);
                break;
        }

        if (key == 27) {
            break;
        }

        if (std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - previousTime).count() >= 1000) {
            previousTime = frameEnd;
        }
    }

    cv::destroyAllWindows();
    return 0;
}