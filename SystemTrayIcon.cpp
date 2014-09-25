#include "SystemTrayIcon.h"
#include <QtWidgets>

SystemTrayIcon::SystemTrayIcon(QObject *parent)
	: QSystemTrayIcon(parent)
{
	this->setIcon(QIcon(":/Resources/tray.png"));
	this->setToolTip(tr("syncfile"));

	QAction *acSetting = new QAction(tr("Setting"), this);
	QAction *acExit = new QAction(tr("Exit"), this);
	QMenu *menu = new QMenu;
	menu->addAction(acSetting);
	menu->addAction(acExit);
	this->setContextMenu(menu);
}


SystemTrayIcon::~SystemTrayIcon(void)
{
}
