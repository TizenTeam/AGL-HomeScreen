#include "layouthandler.h"
#include <QTimerEvent>

LayoutHandler::LayoutHandler(QObject *parent) :
    QObject(parent),
    m_secondsTimerId(-1),
    mp_dBusWindowManagerProxy(0),
    mp_dBusPopupProxy(0),
    m_visibleSurfaces(),
    m_invisibleSurfaces(),
    m_requestsToBeVisibleSurfaces()
{
    qDBusRegisterMetaType<SimplePoint>();
    qDBusRegisterMetaType<QList<SimplePoint> >();
    qDBusRegisterMetaType<LayoutArea>();
    qDBusRegisterMetaType<QList<LayoutArea> >();
    qDBusRegisterMetaType<Layout>();
    qDBusRegisterMetaType<QList<Layout> >();

    qDebug("D-Bus: connect to org.agl.windowmanager /windowmanager");
    mp_dBusWindowManagerProxy = new org::agl::windowmanager("org.agl.windowmanager",
                                              "/windowmanager",
                                              QDBusConnection::sessionBus(),
                                              0);
    qDebug("D-Bus: connect to org.agl.homescreen /Popup");
    mp_dBusPopupProxy = new org::agl::popup("org.agl.homescreen",
                                              "/Popup",
                                              QDBusConnection::sessionBus(),
                                              0);
}

LayoutHandler::~LayoutHandler()
{
    delete mp_dBusPopupProxy;
    delete mp_dBusWindowManagerProxy;
}

void LayoutHandler::setUpLayouts()
{
    qDebug("setUpLayouts");
    QList<LayoutArea> surfaceAreas;
    LayoutArea surfaceArea;

    const int SCREEN_WIDTH = 1080;
    const int SCREEN_HEIGHT = 1920;

    const int STATUSBAR_HEIGHT = 60;
    const int STATUSBAR_WIDTH = SCREEN_WIDTH;
    const int STATUSBAR_X = 0;
    const int STATUSBAR_Y = 0;
    const int CONTROLBAR_HEIGHT = 60;
    const int CONTROLBAR_WIDTH = SCREEN_WIDTH;
    const int CONTROLBAR_X = 0;
    const int CONTROLBAR_Y = SCREEN_HEIGHT - CONTROLBAR_HEIGHT;


    // layout 1:
    // one app surface, statusbar, control bar
    surfaceArea.x = 0;
    surfaceArea.y = STATUSBAR_HEIGHT;
    surfaceArea.width = SCREEN_WIDTH;
    surfaceArea.height = SCREEN_HEIGHT - STATUSBAR_HEIGHT - CONTROLBAR_HEIGHT;

    surfaceAreas.append(surfaceArea);

    mp_dBusWindowManagerProxy->addLayout(1, "one app", surfaceAreas);


    surfaceAreas.clear();

    // layout 2:
    // two app surfaces (one on top of the other), statusbar, control bar

    // top surface
    surfaceArea.x = 0;
    surfaceArea.y = STATUSBAR_HEIGHT;
    surfaceArea.width = SCREEN_WIDTH;
    surfaceArea.height = (SCREEN_HEIGHT - STATUSBAR_HEIGHT - CONTROLBAR_HEIGHT) / 2;

    surfaceAreas.append(surfaceArea);

    // bottom surface
    surfaceArea.x = 0;
    surfaceArea.y = SCREEN_HEIGHT / 2;
    surfaceArea.width = SCREEN_WIDTH;
    surfaceArea.height = (SCREEN_HEIGHT - STATUSBAR_HEIGHT - CONTROLBAR_HEIGHT) / 2;

    surfaceAreas.append(surfaceArea);

    mp_dBusWindowManagerProxy->addLayout(2, "top on bottom", surfaceAreas);


    surfaceAreas.clear();

    // layout 3:
    // two app surfaces (one besides the other), statusbar, control bar

    // left surface
    surfaceArea.x = 0;
    surfaceArea.y = STATUSBAR_HEIGHT;
    surfaceArea.width = SCREEN_WIDTH / 2;
    surfaceArea.height = SCREEN_HEIGHT - STATUSBAR_HEIGHT - CONTROLBAR_HEIGHT;

    surfaceAreas.append(surfaceArea);

    // right surface
    surfaceArea.x = SCREEN_WIDTH / 2;
    surfaceArea.y = STATUSBAR_HEIGHT;
    surfaceArea.width = SCREEN_WIDTH / 2;
    surfaceArea.height = SCREEN_HEIGHT - STATUSBAR_HEIGHT - CONTROLBAR_HEIGHT;

    surfaceAreas.append(surfaceArea);

    mp_dBusWindowManagerProxy->addLayout(3, "side by side", surfaceAreas);
}

void LayoutHandler::showAppLayer()
{
    mp_dBusWindowManagerProxy->showLayer(1); //1==app layer
}

void LayoutHandler::hideAppLayer()
{
    mp_dBusWindowManagerProxy->hideLayer(1); //1==app layer
}

void LayoutHandler::makeMeVisible(int pid)
{
    qDebug("makeMeVisible %d", pid);

    m_requestsToBeVisiblePids.append(pid);

    // callback every second
    if (-1 != m_secondsTimerId)
    {
        killTimer(m_secondsTimerId);
        m_secondsTimerId = -1;
    }
    m_secondsTimerId = startTimer(1000);
}

void LayoutHandler::checkToDoQueue()
{
    if ((-1 != m_secondsTimerId) && (0 == m_requestsToBeVisiblePids.size()))
    {
        killTimer(m_secondsTimerId);
        m_secondsTimerId = -1;
    }

    if (0 != m_requestsToBeVisiblePids.size())
    {
        int pid = m_requestsToBeVisiblePids.at(0);
        qDebug("pid %d wants to be visible", pid);

        QList<int> allSurfaces;
        allSurfaces = mp_dBusWindowManagerProxy->getAllSurfacesOfProcess(pid);
        if (0 == allSurfaces.size())
        {
            qDebug("no surfaces for pid %d. retrying!", pid);
        }
        else
        {
            m_requestsToBeVisiblePids.removeAt(0);
            qSort(allSurfaces);

            if (0 != allSurfaces.size())
            {
                m_requestsToBeVisibleSurfaces.append(allSurfaces.at(0));

                qDebug("m_visibleSurfaces %d", m_visibleSurfaces.size());
                qDebug("m_invisibleSurfaces %d", m_invisibleSurfaces.size());
                qDebug("m_requestsToBeVisibleSurfaces %d", m_requestsToBeVisibleSurfaces.size());

                QList<int> availableLayouts = mp_dBusWindowManagerProxy->getAvailableLayouts(m_visibleSurfaces.size() + m_requestsToBeVisibleSurfaces.size());
                if (0 == availableLayouts.size())
                {
                    // no layout fits the need!
                    // replace the last app
                    qDebug("no layout fits the need!");
                    qDebug("replace the last surface");

                    m_invisibleSurfaces.append(m_visibleSurfaces.last());
                    m_visibleSurfaces.removeLast();

                    m_visibleSurfaces.append(m_requestsToBeVisibleSurfaces);
                    m_requestsToBeVisibleSurfaces.clear();

                    for (int i = 0; i < m_visibleSurfaces.size(); ++i)
                    {
                        mp_dBusWindowManagerProxy->setSurfaceToLayoutArea(m_visibleSurfaces.at(i), i);
                    }
                }
                if (1 == availableLayouts.size())
                {
                    // switch to new layout
                    qDebug("switch to new layout %d", availableLayouts.at(0));
                    m_visibleSurfaces.append(m_requestsToBeVisibleSurfaces);
                    m_requestsToBeVisibleSurfaces.clear();

                    mp_dBusWindowManagerProxy->setLayoutById(availableLayouts.at(0));
                    for (int i = 0; i < m_visibleSurfaces.size(); ++i)
                    {
                        mp_dBusWindowManagerProxy->setSurfaceToLayoutArea(m_visibleSurfaces.at(i), i);
                    }
                }
                if (1 < availableLayouts.size())
                {
                    // more than one layout possible! Ask user.
                    qDebug("more than one layout possible! Ask user.");

                    QStringList choices;
                    for (int i = 0; i < availableLayouts.size(); ++i)
                    {
                        choices.append(mp_dBusWindowManagerProxy->getLayoutName(availableLayouts.at(i)));
                    }

                    mp_dBusPopupProxy->showPopupComboBox("Select Layout", choices);
                }
            }
        }
    }
}

QList<int> LayoutHandler::requestGetAllSurfacesOfProcess(int pid)
{
    qDebug("requestGetAllSurfacesOfProcess %d", pid);

    return mp_dBusWindowManagerProxy->getAllSurfacesOfProcess(pid);
}

int LayoutHandler::requestGetSurfaceStatus(int surfaceId)
{
    int result = -1;

    if (-1 != m_visibleSurfaces.indexOf(surfaceId))
    {
        result = 0;
    }
    if (-1 != m_invisibleSurfaces.indexOf(surfaceId))
    {
        result = 1;
    }
    if (-1 != m_requestsToBeVisibleSurfaces.indexOf(surfaceId))
    {
        result = 1;
    }

    return result;
}

void LayoutHandler::requestRenderSurfaceToArea(int surfaceId, const QRect &renderArea)
{
    qDebug("requestRenderSurfaceToArea %d %d,%d,%d,%d", surfaceId, renderArea.x(), renderArea.y(), renderArea.width(), renderArea.height());
}

void LayoutHandler::requestSurfaceIdToFullScreen(int surfaceId)
{
    qDebug("requestSurfaceIdToFullScreen %d", surfaceId);
}

void LayoutHandler::setLayoutByName(QString layoutName)
{
    // switch to new layout
    qDebug("setLayout: switch to new layout %s", layoutName.toStdString().c_str());
    m_visibleSurfaces.append(m_requestsToBeVisibleSurfaces);
    m_requestsToBeVisibleSurfaces.clear();

    mp_dBusWindowManagerProxy->setLayoutByName(layoutName);
    for (int i = 0; i < m_visibleSurfaces.size(); ++i)
    {
        mp_dBusWindowManagerProxy->setSurfaceToLayoutArea(i, i);
    }
}

void LayoutHandler::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_secondsTimerId)
    {
        checkToDoQueue();
    }
}

