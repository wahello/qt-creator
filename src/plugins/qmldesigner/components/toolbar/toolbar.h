// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0+ OR GPL-3.0 WITH Qt-GPL-exception-1.0

#pragma once

#include <QQuickWidget>

namespace QmlDesigner {

class ToolBar : public QQuickWidget
{
    Q_OBJECT

public:
    ToolBar();
    static void create();
    static void createStatusBar();
    static bool isVisible();
};

} // namespace QmlDesigner