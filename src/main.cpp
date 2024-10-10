#include <chrono>
#include "MEA.h"
#include <thread>

MEA_Params params(64, 64, 32, 64, 512);

MEA_Info meaInfo(params);

void refreshPoints(MEA& mea) {
    while (1) {
        mea.generateNextPoint();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        //MEA_Params params = static_cast<MEA_Info*>(userdata)->params;
        // Left button click
        std::cout << "Mouse click at: (" << x << ", " << y << ")" << std::endl;
        int indexImage = ::getImageIndex(x, y, params.numImages, params.width, params.height);
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

    std::thread t(refreshPoints, std::ref(meaInfo.mea));

    auto previousTime = std::chrono::high_resolution_clock::now();

    while (true) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        cv::Mat image = meaInfo.mea.plotGrid();

        cv::Mat heatmap = meaInfo.mea.plotHeatmap(32);

        if (numberOfWindows > 0){
            std::vector<cv::Mat> selectedImage = meaInfo.mea.plotSelectedSignal();

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

        int key = cv::waitKey(20);

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

        if (key == 27) {
            break;
        }

        if (std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - previousTime).count() >= 1000) {
            previousTime = frameEnd;
        }
    }

    cv::destroyAllWindows();
    t.detach();
    return 0;
}