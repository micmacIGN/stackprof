#ifndef GEOCRSSELECT_INHERITQATABLEMODEL_H
#define GEOCRSSELECT_INHERITQATABLEMODEL_H

#include <QAbstractTableModel>
#include <QItemSelection>

#include "../../georef/Proj6_crs_to_crs.h"

enum e_geoCRSTableViewColumn {
    e_GCRSTVC_vectIdx = 0,
    e_GCRSTVC_EPSGCode,
    e_GCRSTVC_name,
    //e_GCRSTVC_areaName,

    e_GCRSTVC_columnCount,
    e_GCRSTVC_lastColumn = e_GCRSTVC_columnCount - 1,
};

class geoCRSSelect_inheritQATableModel : public QAbstractTableModel {

    Q_OBJECT

public:
    geoCRSSelect_inheritQATableModel(QObject *parent = nullptr);

    bool loadEPSGDataListFromProjDB(const QString& strPathProjDB);

    bool checkStrEPSGCodeisKnown(const QString& strEPSGCode) const;
    bool checkStrEPSGCodeisKnown(const QString& strEPSGCode, QString& strAssociatedNameToEPSGCode) const;

    //void getModelIndex_TL_BR(QModelIndex& qmiTopLeft, QModelIndex& qmiBottomRight);

    //---------------------------------
    //QAbstractTableModel side --------
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
    //virtual void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

    void slot_searchFromKeywords(QStringList strListKeyword);
    void slot_keywordListCleared();

    void slot_selectionChangedInTableView(int index);

signals:
    void signal_searchFromKeywords_resultCount(int resultCount/*, bool bMaxToSearchReached*/);
    void signal_EPSGDataAboutSelection(const QString& strEPSGCode, const QString& strName);

private:
    void removeAllFromTable();
    bool fillTablefromQvectQStringList();

signals:

private:
    Proj6_crs_to_crs _proj6crsTocrs;

    QHash<QString, S_dataForAKnownEPSGCode_fromProjDB> _qhash_str_SdfakEPSGCode;
    QMultiMap<QString, S_dataForAZoneName_fromProjDB> _qMMap_str_SdfaZoneName;

    QVector<QStringList> _qvectQStringList_EPSGDataFoundFromKeywordSearch;
    int _sizeOf_qvectQStringList_EPSGData;

    int _sizeOf_qvectQStringList_EPSGData_tableFilled;
    int _indexOfTopItemForViewFeed;

    int _maxLinePerPage;
};

#endif //GEOCRSSELECT_INHERITQATABLEMODEL_H
