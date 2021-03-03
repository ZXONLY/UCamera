#pragma once

struct USizei {
    int nWidth;
    int nHeight;

    USizei() : nWidth(0), nHeight(0) {

    }

    USizei(const int _nWidth, const int _nHeight)
            : nWidth(_nWidth), nHeight(_nHeight) {

    }

    USizei(const USizei& size) {
        nWidth = size.nWidth;
        nHeight = size.nHeight;
    }

    USizei&operator=(const USizei& size) {
        nWidth = size.nWidth;
        nHeight = size.nHeight;
        return *this;
    }

    void set(const int _nWidth, const int _nHeight) {
        nWidth = _nWidth;
        nHeight = _nHeight;
    }

    void set(const USizei& size) {
        nWidth = size.nWidth;
        nHeight = size.nHeight;
    }

    bool isValid() const {
        return nWidth > 0 && nHeight > 0;
    }

    inline int getSize() const {
        return nWidth * nHeight;
    }

    inline float getRatio() const {
        return nWidth * 1.0f / nHeight;
    }

    inline bool operator<(const USizei& size) const {
        return getSize() < size.getSize();
    }

    inline bool operator==(const USizei& size) const {
        return nWidth == size.nWidth && nHeight == size.nHeight;
    }

    inline bool operator!=(const USizei& size) const {
        return nWidth != size.nWidth || nHeight != size.nHeight;
    }

    inline int getRGBASize() const {
        return nWidth * nHeight * 4;
    }

};

