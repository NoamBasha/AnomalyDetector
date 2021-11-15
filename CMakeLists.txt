cmake_minimum_required(VERSION 3.17)
project(SimpleAnomalyDetector)

set(CMAKE_CXX_STANDARD 11)

add_executable(SimpleAnomalyDetector MainTrain.cpp anomaly_detection_util.cpp timeseries.cpp SimpleAnomalyDetector.cpp minCircle.cpp)