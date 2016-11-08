#ifndef LAYOUTHANDLER_H
#define LAYOUTHANDLER_H

#include <QObject>
#include "windowmanager_proxy.h"
#include "popup_proxy.h"

class LayoutHandler : public QObject
{
    Q_OBJECT
public:
    explicit LayoutHandler(QObject *parent = 0);
    ~LayoutHandler();

    void setUpLayouts();

signals:

public slots:
    void showAppLayer();
    void hideAppLayer();
    void makeMeVisible(int pid);
private:
    void checkToDoQueue();
public slots:
    QList<int> requestGetAllSurfacesOfProcess(int pid);
    int requestGetSurfaceStatus(int surfaceId);
    void requestRenderSurfaceToArea(int surfaceId, const QRect &renderArea);
    void requestSurfaceIdToFullScreen(int surfaceId);
    void setLayoutByName(QString layoutName);

protected:
    void timerEvent(QTimerEvent *e);
private:
    int m_secondsTimerId;
    org::agl::windowmanager *mp_dBusWindowManagerProxy;
    org::agl::popup *mp_dBusPopupProxy;

    QList<int> m_requestsToBeVisiblePids;
    QList<int> m_visibleSurfaces;
    QList<int> m_invisibleSurfaces;
    QList<int> m_requestsToBeVisibleSurfaces;
};

#endif // LAYOUTHANDLER_H
