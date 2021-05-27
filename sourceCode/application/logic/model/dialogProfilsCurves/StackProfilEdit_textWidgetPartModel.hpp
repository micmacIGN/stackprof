#ifndef STACKPROFILEDIT_TEXTWIDGETPART_HPP
#define STACKPROFILEDIT_TEXTWIDGETPART_HPP

#include <QObject>

#include "../core/leftRight_sideAccess.hpp"
#include "../core/ComputationCore_structures.h"

#include "../StackedProfilInEdition.h"

class StackProfilEdit_textWidgetPartModel : public QObject {

    Q_OBJECT

    public:
        StackProfilEdit_textWidgetPartModel(QObject *parent=nullptr);

        //feed with a S_StackedProfilWithMeasurements
        bool feed(e_LayersAcces ieLayerAccess,

                  S_StackedProfilWithMeasurements *stackedProfilWithMeasurementsPtr,
                  qreal xGraphMin,
                  qreal xGraphMax,
                  const QString& qstrGraphicTitle);
        //set _leftRightSides_linearRegressionModel according to the main measure or the secondary selected measure

        bool setOnMeasure(const S_MeasureIndex& measureIndex);

        double get_xAdjusterPos(e_LRsideAccess eLRsA_locationRelativeToX0CentralAxis, e_LRsideAccess eLRSA_adjuster);
        double get_offsetAtX0();
        double get_sigmaSum();
        QString get_strTitle();

        double get_X0();

        //from ui:
        bool get_warningFlagByUser(bool& bWarningFlagByUser);
        bool set_warningFlagByUser_UiModelFromUi(bool bWarningFlagByUser, bool bAlternativeModeActivated);

        //from text model of ui:
        bool set_warningFlagByUser(/*const S_MeasureIndex& measureIndex,*/bool bWarningFlagByUser);

        bool validateEditedAdjustersSetPositionValues_evaluatingConsistency(
            const double a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues[2][2],
                            bool a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[2][2]);

        bool validateEditedX0CenterSetPositionValue_evaluatingConsistency(

            const double doubleValueOfQLineEditTextContent_X0PosValue,
                            bool& evaluatedConsistencyFlag_X0PosValue);

        bool linearModelIsAvailable();
signals:
        void signal_adjustersSetPositionValues_changed_fromTextEdition(
                uint ieComponent,
                S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]);


        void signal_X0SetPositionValue_fromTextEdition(
                uint ieComponent,
                double X0PosValue);

        void signal_feed_stackProfilEdit_textWidgets();

        void signal_setWarnFlag_fromTextEdition(uint ieComponent, bool bWarnFlag, bool bAlternativeModeActivated);

        void signal_setWarnFlag_forUi(uint ieComponent, bool bWarnFlag);

    private:

        bool xPos_isAPossibleValue(double xPos, e_LRsideAccess eLRsA_locationRelativeToX0CentralAxis, e_LRsideAccess eLRSA_adjuster);

        bool checkPosition_isInsideTheAccordingRange(double xPos, e_LRsideAccess eLRsA_locationRelativeToX0CentralAxis);

        bool checkRelativePositionInsideAdjustersCouple(double xPos, e_LRsideAccess eLRSA_adjuster,
                                                        const S_XRangeForLinearRegressionComputation& xRangeForLinearRegressionComputation);

        void clear();
        void clear_onMeasure();

        bool _bFeed_stackedProfilWithMeasurements;

        e_LayersAcces _eLA_layer;
        double _xGraphMin;
        double _xGraphMax;

        S_StackedProfilInEdition _stackedProfilInEdition;

        bool _bFeed_measure;
        S_LeftRightSides_linearRegressionModel _leftRightSides_linearRegressionModel;
        bool _bWarningFlagByUser;

        static constexpr double _invalidNumericValue = 999999.999;

        QString _qstrGraphicTitle;

};

#endif // STACKPROFILEDIT_TEXTWIDGETPART_HPP
