#ifndef CUSTOMGRAPHICSVIEW_BASIC_H
#define CUSTOMGRAPHICSVIEW_BASIC_H

#include <QGraphicsView>

class CustomGraphicsScene_profil;

class CustomGraphicsView_basic : public QGraphicsView {

  Q_OBJECT

public:

    CustomGraphicsView_basic(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent* event) override;

    void setCustomGraphicsScenePtr(CustomGraphicsScene_profil *ptrCustomGraphicsScene_profil);
    void updateOnDemand();

    CustomGraphicsScene_profil *customGraphicsScenePtr();

    void clear();

signals:

public slots:

    void slot_fitInView();

private:

    CustomGraphicsScene_profil *_ptrCustomGraphicsScene_profil;
};


#endif // CUSTOMGRAPHICSVIEW_BASIC_H
