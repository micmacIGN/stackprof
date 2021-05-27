#include <QDebug>

#include <QAbstractTableModel>

#include <QHash>
#include <QMultiMap>

#include "../../georef/Proj6_crs_to_crs.h"

#include "geoCRSSelect_inheritQATableModel.h"

geoCRSSelect_inheritQATableModel::geoCRSSelect_inheritQATableModel(QObject *parent):
    QAbstractTableModel(parent),
    _sizeOf_qvectQStringList_EPSGData(0),
    _sizeOf_qvectQStringList_EPSGData_tableFilled(0),
    _indexOfTopItemForViewFeed(0),
    _maxLinePerPage(10) {

}

bool geoCRSSelect_inheritQATableModel::loadEPSGDataListFromProjDB(const QString& strPathProjDB) {

    _qhash_str_SdfakEPSGCode.clear();
    _qMMap_str_SdfaZoneName.clear();

    _proj6crsTocrs.clear_PJ_PJContext();
    bool bReport = _proj6crsTocrs.createContext(strPathProjDB.toStdString().c_str());
    if (!bReport) {
        qDebug() << __FUNCTION__ << ": error creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    //bReport = _proj6crsTocrs.getDataForAllKnownEPSGCodes(_qhash_str_SdfakEPSGCode);
    bReport = _proj6crsTocrs.getDataForAllKnownEPSGCodes(_qhash_str_SdfakEPSGCode, _qMMap_str_SdfaZoneName);

    /*
    ushort ushort_startingcharForAlphabeticScan = 'A';

    int sumOfCount = 0;

    for (;ushort_startingcharForAlphabeticScan <= 'Z'; ushort_startingcharForAlphabeticScan++) {
        int count = 0;
        QChar qcharStartingcharForAlphabeticScan(ushort_startingcharForAlphabeticScan);
        for (const auto &iter:_qhash_str_SdfakEPSGCode) {
            if (iter.strName.startsWith(qcharStartingcharForAlphabeticScan,Qt::CaseSensitivity::CaseInsensitive)) {
                count++;
            }
        }
        qDebug() << qcharStartingcharForAlphabeticScan << " count: " << count;
        sumOfCount += count;
    }
    qDebug() << "sumOfCount: " << sumOfCount;

    {
        int count = 0;
        for (const auto &iter:_qhash_str_SdfakEPSGCode) {
            if (iter.strName.contains("NAD83",Qt::CaseSensitivity::CaseInsensitive)) {
                count++;
            }
        }
        qDebug() << "NAD83 count: " << count;
    }

    {
        int count = 0;
        for (const auto &iter:_qhash_str_SdfakEPSGCode) {
            if (iter.strName.contains("UTM",Qt::CaseSensitivity::CaseInsensitive)) {
                count++;
            }
        }
        qDebug() << "UTM count: " << count;
    }


    QString qstrChar_forCombination;
    for (ushort ushort_AToZ = 'A'; ushort_AToZ <= 'Z'; ushort_AToZ++) {
        qstrChar_forCombination.append(ushort_AToZ);
    }
    for (ushort ushort_0To9 = '0'; ushort_0To9 <= '9'; ushort_0To9++) {
        qstrChar_forCombination.append(ushort_0To9);
    }

    qstrChar_forCombination.append(" /_-+().°º&',[]\"");

    int size_qstrChar_forCombination = qstrChar_forCombination.count();

    for (int ic1 = 0; ic1 < size_qstrChar_forCombination;ic1++) {
        qDebug() << "ic1:" << qstrChar_forCombination[ic1];

        for (int ic2 = 0; ic2 < size_qstrChar_forCombination;ic2++) {
            qDebug() << "ic1:" << qstrChar_forCombination[ic2];

            for (int ic3 = 0; ic3 < size_qstrChar_forCombination;ic3++) {
                qDebug() << "ic3:" << qstrChar_forCombination[ic3];

                int count_with123 = 0;

                QString startingStringForAlphabeticScan;

                startingStringForAlphabeticScan += qstrChar_forCombination[ic1];
                startingStringForAlphabeticScan += qstrChar_forCombination[ic2];
                startingStringForAlphabeticScan += qstrChar_forCombination[ic3];

                qDebug() << startingStringForAlphabeticScan;

                for (const auto &iter:_qhash_str_SdfakEPSGCode) {
                    if (iter.strName.contains(startingStringForAlphabeticScan,Qt::CaseSensitivity::CaseInsensitive)) {
                        count_with123++;
                    }
                }

                qDebug() << startingStringForAlphabeticScan << " count: " << count_with123;
            }
        }
    }*/
    return(bReport);
}

int geoCRSSelect_inheritQATableModel::rowCount(const QModelIndex &parent) const {
    return(_sizeOf_qvectQStringList_EPSGData_tableFilled);
}


int geoCRSSelect_inheritQATableModel::columnCount(const QModelIndex &parent) const {
    return(e_GCRSTVC_columnCount);
}

QVariant geoCRSSelect_inheritQATableModel::data(const QModelIndex &index, int role) const {

    int indexRow = index.row();
    //'Note: If you do not have a value to return, return an invalid QVariant instead of returning 0.'
    QVariant qvariantEmpty;

    qDebug() << __FUNCTION__ << "(geoCRSSelect_inheritQATableModel) indexRow = " << indexRow;

    int totalRowFromQVect = _sizeOf_qvectQStringList_EPSGData_tableFilled;

    qDebug() << __FUNCTION__ << "(geoCRSSelect_inheritQATableModel) totalRowFromQVect = " << totalRowFromQVect;
    qDebug() << __FUNCTION__ << "(geoCRSSelect_inheritQATableModel) _indexOfTopItemForViewFeed = " << _indexOfTopItemForViewFeed;

    if (role == Qt::DisplayRole) {
        if (indexRow < totalRowFromQVect - _indexOfTopItemForViewFeed) {
            switch(index.column()) {
            case e_GCRSTVC_vectIdx:
                return(QString::number(indexRow+1));
                //break;

            case e_GCRSTVC_EPSGCode:
            case e_GCRSTVC_name:
            //case e_GCRSTVC_areaName:
                return(_qvectQStringList_EPSGDataFoundFromKeywordSearch[indexRow][index.column()-1]);
                //break;

            default:;
            }
        }
    }

    qDebug() << __FUNCTION__ << "(geoCRSSelect_inheritQATableModel) return(qvariantEmpty)";
    return(qvariantEmpty);
}

QVariant geoCRSSelect_inheritQATableModel::headerData(int section, Qt::Orientation orientation, int role) const {

    //Returns the data for the given role and section in the header with the specified orientation.
    //For horizontal headers, the section number corresponds to the column number. Similarly, for vertical headers, the section number corresponds to the row number.
    QVariant qvariantEmpty;
    if (role != Qt::DisplayRole)     { return(qvariantEmpty); }
    if (orientation == Qt::Vertical) { return(qvariantEmpty); }
    //if (section!= 0)               { return(qvariantEmpty); }

    QVector<QString> qvectStrHeaderTitle {        
        "",
        "EPSG Code",
        "Name"
        /*,"Area name"*/
    };

    if (section < qvectStrHeaderTitle.size()) {
        return(qvectStrHeaderTitle[section]);
    }
    return(qvariantEmpty);
}


void geoCRSSelect_inheritQATableModel::slot_searchFromKeywords(QStringList strListKeyword) {

    _qvectQStringList_EPSGDataFoundFromKeywordSearch.clear();
    _sizeOf_qvectQStringList_EPSGData = 0;

    if (!_qMMap_str_SdfaZoneName.count()) {
        emit signal_searchFromKeywords_resultCount(0/*, false*/);
    }

    qDebug() << __FUNCTION__ << strListKeyword;

    int countFound = 0;
    //int maxOutToSearch = 10000;
    //bool bMaxToSearchReached = false;

    QMultiMap<QString, S_dataForAZoneName_fromProjDB>::const_iterator qmmap_constIterator = _qMMap_str_SdfaZoneName.cbegin();
    for (qmmap_constIterator = _qMMap_str_SdfaZoneName.cbegin();
         qmmap_constIterator != _qMMap_str_SdfaZoneName.cend();
         ++qmmap_constIterator) {
        bool bAllFound = true;
        for (const auto &iterKeyworld: strListKeyword) {
            if (!qmmap_constIterator.key().contains(iterKeyworld,Qt::CaseSensitivity::CaseInsensitive)) {
                bAllFound = false;
                break;
            }
        }
        if (bAllFound) {
            countFound++;
            qDebug() << __FUNCTION__ << "found: " << qmmap_constIterator.key() << ": " << qmmap_constIterator.value().strEPSGCode;
            //if (countFound < maxOutToSearch) {
                _qvectQStringList_EPSGDataFoundFromKeywordSearch.push_back({qmmap_constIterator.value().strEPSGCode,
                                                                            qmmap_constIterator.key()/*,
                                                                            qmmap_constIterator.value().strAreaName*/
                                                                           });
            /*} else {
                bMaxToSearchReached = true;
                break;
            }*/
        }
    }
    qDebug() << __FUNCTION__ << "countFound: " << countFound;

    _sizeOf_qvectQStringList_EPSGData = _qvectQStringList_EPSGDataFoundFromKeywordSearch.count();

    //if (!bMaxToSearchReached) {
        removeAllFromTable();
        fillTablefromQvectQStringList();
    //}

    emit signal_searchFromKeywords_resultCount(countFound/*, false*/); //bMaxToSearchReached



}

void geoCRSSelect_inheritQATableModel::slot_keywordListCleared() {
    removeAllFromTable();
}

void geoCRSSelect_inheritQATableModel::removeAllFromTable() {
    qDebug() << __FUNCTION__ << "(geoCRSSelect_inheritQATableModel)";

    qDebug() << __FUNCTION__ << "Be sure to remove also graphics item synced and models feed";

    int qhashBASPWM_size  = _sizeOf_qvectQStringList_EPSGData_tableFilled;
    if (!qhashBASPWM_size) {
        qDebug() << __FUNCTION__ << "_sizeOf_qvectQStringList_EPSGData_tableFilled is zero";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
    }

    if (qhashBASPWM_size) {

        emit layoutAboutToBeChanged();

        QModelIndex qmodelIndexForRemoveAll =  index(0, 0);

        qDebug() << __FUNCTION__ << "wip-dev: qmodelIndexForRemoveAll= " << qmodelIndexForRemoveAll;
        qDebug() << __FUNCTION__ << "wip-dev: qlinkedlist_route_size= " << qhashBASPWM_size;
        qDebug() << __FUNCTION__ << "wip-dev: beginRemoveRows(..., = " << 0 << ", " << qhashBASPWM_size-1;

        int lastIndex = qhashBASPWM_size-1;

        beginRemoveRows(qmodelIndexForRemoveAll, 0, lastIndex);

        endRemoveRows();

        _sizeOf_qvectQStringList_EPSGData_tableFilled = 0;

        emit layoutChanged();


    }
}

//this method consider that the table was previously cleared if required
bool geoCRSSelect_inheritQATableModel::fillTablefromQvectQStringList() {

    qDebug() << __FUNCTION__ << "(geoCRSSelect_inheritQATableModel)";

    if (!_sizeOf_qvectQStringList_EPSGData) {
        return(false);
    }

    emit layoutAboutToBeChanged();

    _sizeOf_qvectQStringList_EPSGData_tableFilled = _sizeOf_qvectQStringList_EPSGData;

    QModelIndex qmodelIndexForInsert = index(0, 0);
    beginInsertRows(qmodelIndexForInsert, 0, _sizeOf_qvectQStringList_EPSGData_tableFilled-1);
    endInsertRows();

    emit layoutChanged();

    return(true);
}

void geoCRSSelect_inheritQATableModel::slot_selectionChangedInTableView(int index) {

    if (  (index < 0)
        ||(index >= _qvectQStringList_EPSGDataFoundFromKeywordSearch.count())) {
          return;
     }

    emit signal_EPSGDataAboutSelection(
        _qvectQStringList_EPSGDataFoundFromKeywordSearch[index][0],
        _qvectQStringList_EPSGDataFoundFromKeywordSearch[index][1]);
}

bool geoCRSSelect_inheritQATableModel::checkStrEPSGCodeisKnown(const QString& strEPSGCode) const {
    QString strAssociatedNameToEPSGCode;
    return(checkStrEPSGCodeisKnown(strEPSGCode, strAssociatedNameToEPSGCode));
}

bool geoCRSSelect_inheritQATableModel::checkStrEPSGCodeisKnown(const QString& strEPSGCode, QString& strAssociatedNameToEPSGCode) const {
    strAssociatedNameToEPSGCode.clear();
    //QHash<QString, S_dataForAKnownEPSGCode_fromProjDB>::const_iterator
    auto qhash_constIterator = _qhash_str_SdfakEPSGCode.cbegin();
    qhash_constIterator = _qhash_str_SdfakEPSGCode.constFind(strEPSGCode);

    if ( qhash_constIterator != _qhash_str_SdfakEPSGCode.cend()) {
        qDebug() << __FUNCTION__  << "(geoCRSSelect_inheritQATableModel) found:"
                 << qhash_constIterator.key() << ", " << qhash_constIterator.value().strName;
        strAssociatedNameToEPSGCode = qhash_constIterator.value().strName;
    }
    return( qhash_constIterator != _qhash_str_SdfakEPSGCode.cend());
}

/*
void geoCRSSelect_inheritQATableModel::getModelIndex_TL_BR(QModelIndex& qmiTopLeft, QModelIndex& qmiBottomRight) {

    qmiTopLeft = index(0, 0, QModelIndex());

    int qhashBASPWM_size  = _sizeOf_qvectQStringList_EPSGData_tableFilled;

    if (!qhashBASPWM_size) {
        qDebug() << __FUNCTION__ << "_sizeOf_qvectQStringList_EPSGData_tableFilled is zero";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);

        qmiBottomRight = index(0, 0, QModelIndex());
        return;
    }

    int lastIndex = qhashBASPWM_size-1;

    qDebug() << __FUNCTION__ << "size != zero";
    qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
    qDebug() << __FUNCTION__ << "hasIndex(lastIndex, e_GCRSTVC_lastColumn) = " << hasIndex(lastIndex, e_GCRSTVC_lastColumn);
    qDebug() << __FUNCTION__ << "lastIndex = " << lastIndex;

    qmiBottomRight = index(lastIndex, e_GCRSTVC_lastColumn);

}
*/


