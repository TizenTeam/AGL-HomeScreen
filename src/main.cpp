/*
 * Copyright (C) 2016 Mentor Graphics Development (Deutschland) GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mainwindow.h"
#include "include/daynightmode.h"
#include <QApplication>
#include <QSysInfo>
#include <QSharedMemory>
#include "inputeventdistributor.h"

#ifdef __i386__
#include "inputdevicesimulator.h"
#include "systemsettingssimulator.h"
#endif


int main(int argc, char *argv[])
{
    // allow only one instance of this application
    QSharedMemory appInstance;
    appInstance.setKey("AGLHomeScreenApp");
    if (!appInstance.create(1))
    {
        qDebug("Only one instance of the Home Screen App allowed!");
        exit(-1);
    }

    QApplication a(argc, argv);

    // used for application settings (QSettings)
    QCoreApplication::setOrganizationDomain("LinuxFoundation");
    QCoreApplication::setOrganizationName("AutomotiveGradeLinux");
    QCoreApplication::setApplicationName("HomeScreen");
    QCoreApplication::setApplicationVersion("0.0.1");

    MainWindow w;
    w.move(0, 0);
    w.show();

    InputEventDistributor *mp_inputEventDistributor;
    mp_inputEventDistributor = new InputEventDistributor();


    // start input and system settings simulator on developer PCs
#ifdef __arm__
    qDebug("Running on ARM architecture");
#endif
#ifdef __i386__
    InputDeviceSimulator inputdevicesimulator(&w);
    inputdevicesimulator.show();
    SystemSettingsSimulator systemsettingssimulator;
    systemsettingssimulator.show();
    qDebug("Running on x86 architecture");
#endif

    return a.exec();
}
