#include "LingmoShellCore/ShellConfig.h"
#include "private/ShellConfig_p.h"

#include <LingmoConfig/Config.h>

#include <QDebug>
#include <QStandardPaths>

namespace Lingmo {

// Config key constants
static constexpr auto kGroupShell       = "Shell";
static constexpr auto kKeyTheme         = "Shell/theme";
static constexpr auto kKeyAnimations    = "Shell/animationsEnabled";
static constexpr auto kKeyPanelHeight   = "Shell/panelHeight";

// ── Instance ────────────────────────────────────────────────────────────────

ShellConfig *ShellConfig::instance()
{
    static ShellConfig s_instance;
    return &s_instance;
}

ShellConfig::ShellConfig(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<ShellConfigPrivate>())
{
    d->config = std::make_unique<Lingmo::Config>(QStringLiteral("lingmo-shell"));
}

ShellConfig::~ShellConfig() = default;

// ── Load / reload ────────────────────────────────────────────────────────────

bool ShellConfig::load()
{
    qInfo("ShellConfig: loading configuration via lingmo-config...");

    // Standard layered load: system dir first, user dir second.
    const QString systemDir = QStringLiteral("/etc/lingmo");
    const QString userDir   = QStandardPaths::writableLocation(
                                  QStandardPaths::AppConfigLocation);

    const bool ok = d->config->load(systemDir, userDir);
    if (!ok) {
        qWarning("ShellConfig: lingmo-config load failed, using defaults");
    }

    // Read values from config, keeping existing defaults if keys are absent.
    d->shellTheme = d->config->value(
        QLatin1String(kKeyTheme),
        d->shellTheme).toString();

    d->animationsEnabled = d->config->value(
        QLatin1String(kKeyAnimations),
        d->animationsEnabled).toBool();

    d->panelHeight = d->config->value(
        QLatin1String(kKeyPanelHeight),
        d->panelHeight).toInt();

    // Forward config-file changes to our own signals.
    connect(d->config.get(), &Lingmo::Config::valueChanged,
            this, [this](const QString &key, const QVariant &value) {
        if (key == QLatin1String(kKeyTheme)) {
            const QString theme = value.toString();
            if (d->shellTheme != theme) {
                d->shellTheme = theme;
                Q_EMIT configChanged(QStringLiteral("shellTheme"));
                Q_EMIT themeChanged(theme);
            }
        } else if (key == QLatin1String(kKeyAnimations)) {
            const bool enabled = value.toBool();
            if (d->animationsEnabled != enabled) {
                d->animationsEnabled = enabled;
                Q_EMIT configChanged(QStringLiteral("animationsEnabled"));
            }
        } else if (key == QLatin1String(kKeyPanelHeight)) {
            const int height = value.toInt();
            if (d->panelHeight != height) {
                d->panelHeight = height;
                Q_EMIT configChanged(QStringLiteral("panelHeight"));
            }
        }
    });

    qInfo("ShellConfig: theme=%s animations=%d panelHeight=%d",
          qUtf8Printable(d->shellTheme), d->animationsEnabled, d->panelHeight);
    return true;
}

void ShellConfig::reload()
{
    qInfo("ShellConfig: reloading configuration...");
    load();
}

// ── Accessors ────────────────────────────────────────────────────────────────

QString ShellConfig::shellTheme() const { return d->shellTheme; }

void ShellConfig::setShellTheme(const QString &theme)
{
    if (d->shellTheme == theme)
        return;
    d->shellTheme = theme;
    d->config->setValue(QLatin1String(kKeyTheme), theme);
    d->config->save();
    Q_EMIT configChanged(QStringLiteral("shellTheme"));
    Q_EMIT themeChanged(theme);
}

bool ShellConfig::animationsEnabled() const { return d->animationsEnabled; }

void ShellConfig::setAnimationsEnabled(bool enabled)
{
    if (d->animationsEnabled == enabled)
        return;
    d->animationsEnabled = enabled;
    d->config->setValue(QLatin1String(kKeyAnimations), enabled);
    d->config->save();
    Q_EMIT configChanged(QStringLiteral("animationsEnabled"));
}

int ShellConfig::panelHeight() const { return d->panelHeight; }

void ShellConfig::setPanelHeight(int height)
{
    if (d->panelHeight == height)
        return;
    d->panelHeight = height;
    d->config->setValue(QLatin1String(kKeyPanelHeight), height);
    d->config->save();
    Q_EMIT configChanged(QStringLiteral("panelHeight"));
}

} // namespace Lingmo
