#ifndef ConverterPixelToQsc_HPP
#define ConverterPixelToQsc_HPP

#include <QDebug>

//qsc means: Qt scene coordinates (system)
struct S_ParametersForPixelResizeInvariance {
    double _qsc_gripSquareWidth;
    double _qsc_gripSquareHeight;

};

class ConverterPixelToQsc {

public:
    ConverterPixelToQsc():
        _x_mulFactor_pixelToqsc(1.0),
        _y_mulFactor_pixelToqsc(1.0) {
    }

    void setWidthRatioFrom(double qscWidth, double pixelWidth) {
        if (pixelWidth < 1.0) {
            qDebug() << __FUNCTION__ << "pixelWidth is invalid, set to 1 as invalid but safe value";
            pixelWidth = 1.0;
        }
        _x_mulFactor_pixelToqsc = qscWidth / pixelWidth;
        //qDebug() << __FUNCTION__ << "_x_mulFactor_pixelToqsc set to:" << _x_mulFactor_pixelToqsc;
    }

    void setHeightRatioFrom(double qscHeight, double pixelHeight) {
        if (pixelHeight < 1.0) {
            qDebug() << __FUNCTION__ << "pixelHeight is invalid, set to 1 as invalid but safe value";
            pixelHeight = 1.0;
        }
        _y_mulFactor_pixelToqsc = qscHeight / pixelHeight;
    }

    double pixel_x_to_qsc(double pixelX) const {
        //qDebug() << __FUNCTION__ << "_x_mulFactor_pixelToqsc is:" << _x_mulFactor_pixelToqsc;
        return(pixelX * _x_mulFactor_pixelToqsc);
    }

    double pixel_y_to_qsc(double pixelY) const {
        return(pixelY * _y_mulFactor_pixelToqsc);
    }

    double qsc_x_to_pixel(double qscX) const {
        return(qscX / _x_mulFactor_pixelToqsc);
    }

    double qsc_y_to_pixel(double qscY) const {
        return(qscY / _y_mulFactor_pixelToqsc);
    }

private:
    double _x_mulFactor_pixelToqsc;
    double _y_mulFactor_pixelToqsc;
};

#endif //ConverterPixelToQcs_HPP
