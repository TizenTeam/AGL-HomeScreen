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

#ifndef APPFRAMEWORK_HPP
#define APPFRAMEWORK_HPP

#include <QtDBus>

class AppInfo
{
public:
    AppInfo();
    virtual ~AppInfo();

    QString id;
    QString version;
    int width;
    int height;
    QString name;
    QString description;
    QString shortname;
    QString author;
    QString iconPath;

    void read(const QJsonObject &json);

    friend QDBusArgument &operator <<(QDBusArgument &argument, const AppInfo &mAppInfo);
    friend const QDBusArgument &operator >>(const QDBusArgument &argument, AppInfo &mAppInfo);
};


Q_DECLARE_METATYPE(AppInfo)
Q_DECLARE_METATYPE(QList<AppInfo>)

#endif // APPFRAMEWORK_HPP
