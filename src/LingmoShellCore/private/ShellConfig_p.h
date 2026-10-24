#pragma once

#include "LingmoShellCore/ShellConfig.h"

#include <QtCore/QString>
#include <memory>

namespace Lingmo {

class Config;

class ShellConfigPrivate
{
public:
    QString shellTheme = QStringLiteral("default");
    bool animationsEnabled = true;
    int panelHeight = 32;

    // lingmo-config backend providing layered (system/user/session) storage.
    std::unique_ptr<Lingmo::Config> config;
};

} // namespace Lingmo
