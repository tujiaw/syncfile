#pragma once

#include <QObject>

class Common : public QObject
{
	Q_OBJECT
public:
	explicit Common(QObject *parent = 0);
	static void makeDir(const QString &dir);
	static void deleteDir(const QString &dir);
	static void deleteFile(const QString &path);
	static void asyncCopy(QString from, QString to);
	static QString currentDirFromDlg();
	static void foreachDir(const QString &dir, QStringList &files, QStringList &subdirs, bool isRecursion = false);
};

