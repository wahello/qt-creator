/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "qmlprofilertimelinemodel.h"

namespace QmlProfiler {
namespace Internal {

class InputEventsModel : public QmlProfilerTimelineModel
{
    Q_OBJECT

protected:
    bool accepted(const QmlEventType &event) const;
    void loadEvent(const QmlEvent &event, const QmlEventType &type) override;
    void finalize() override;
    void clear() override;

public:
    struct InputEvent {
        InputEvent(InputEventType type = MaximumInputEventType, int a = 0, int b = 0);
        InputEventType type;
        int a;
        int b;
    };

    InputEventsModel(QmlProfilerModelManager *manager, QObject *parent = 0);

    int typeId(int index) const;
    QColor color(int index) const;
    QVariantList labels() const;
    QVariantMap details(int index) const;
    int expandedRow(int index) const;
    int collapsedRow(int index) const;

private:
    static QMetaEnum metaEnum(const char *name);
    int m_keyTypeId;
    int m_mouseTypeId;

    QVector<InputEvent> m_data;
};

} // namespace Internal
} // namespace QmlProfiler
