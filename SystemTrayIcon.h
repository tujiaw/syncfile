#pragma once
#include <QSystemTrayIcon>

class SystemTrayIcon : public QSystemTrayIcon
{
public:
	SystemTrayIcon(QObject *parent = 0);
	~SystemTrayIcon(void);
};

