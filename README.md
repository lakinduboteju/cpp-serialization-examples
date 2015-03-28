# Serialize C++ objects

Play around with different serialization APIs for C++. Not production-level code. Compiled and run only on Linux.

## Summary
Read a large image file (2288 x 1712 .jpg 809.6 kB) from disk. Store image data on a Image (user-defined class) object. Serialize the Image object into a std::string. De-serialize into a new Image object from the string. Measure time taken to serialize + de-serialize 1 large Image object.

### 1. serialization_test_v1
C++ eclipse project. Serialization using Boost's Serialization API. Need to install [Boost](http://www.boost.org/) library.

### 2. serialization_test_v2
C++ eclipse project. Serialization using Google Protocol Buffer (GPB) API. Need to install [GPB](https://developers.google.com/protocol-buffers/) library.

### Output
https://youtu.be/L7YXWHGuoYY
