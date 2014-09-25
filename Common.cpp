#include "Common.h"
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QSharedPointer>
#include <QFileDialog>

Common::Common(QObject *parent) : QObject(parent)
{

}


void Common::makeDir(const QString &dir)
{
	QFileInfo info(dir);
	if (dir.isEmpty() || info.exists() || info.isFile()) {
		return;
	}

	QStringList failedDirs;
	while (!info.exists()) {
		failedDirs.insert(0, info.absoluteFilePath());
		info.setFile(info.absolutePath());
	}
	foreach (const QString &failedDir, failedDirs) {
		QDir directory(failedDir);
		directory.mkdir(failedDir);
	}
}

void Common::deleteDir(const QString &dir)
{
	QDir curDir(dir);
	if (!curDir.exists()) {
		return;
	}

	QFileInfoList infoList = curDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
	foreach (const QFileInfo &info, infoList) {
		if (info.isFile()) {
			QDir tempDir = info.absoluteDir();
			tempDir.remove(info.fileName());
		} else if (info.isDir()) {
			deleteDir(info.absoluteFilePath());
			QDir tempDir = info.absolutePath();
			tempDir.rmdir(info.fileName());
		}
	}

	QFileInfo info(dir);
	if (curDir.cd("..")) {
		curDir.rmdir(info.fileName());
	}
}

void Common::deleteFile(const QString &path)
{
	QFileInfo info(path);
	if (info.isFile()) {
		QDir tempDir = info.absoluteDir();
		tempDir.remove(info.fileName());
	}
}

void Common::asyncCopy(QString from, QString to)
{
	QFileInfo fileFrom(from);
	if (!fileFrom.exists()) {
		return;
	}

	makeDir(to);

	QString cmd("xcopy");
	QStringList args;
	args.append(from.replace('/', '\\'));
	args.append(to.replace('/', '\\'));

	if (fileFrom.isDir()) {
		qDebug() << "is dir";
		args.append("/e");
	}

	args.append("/y");

	QSharedPointer<QProcess> processPtr(new QProcess());
	processPtr->start(cmd, args);
}

QString Common::currentDirFromDlg()
{
	QFileDialog dlg;
	dlg.setFileMode(QFileDialog::Directory);
	if (dlg.exec()) {
		return dlg.directory().path();
	}
	return "";
}

void Common::foreachDir(const QString &dir, QStringList &files, QStringList &subdirs, bool isRecursion)
{
	QDir curDir(dir);
	if (!curDir.exists()) {
		return;
	}

	QFileInfoList infoList = curDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
	foreach (const QFileInfo &info, infoList) {
		if (info.isFile()) {
			files.append(info.absoluteFilePath());
		} else if (info.isDir()) {
			subdirs.append(info.absoluteFilePath());
			if (isRecursion) {
				foreachDir(info.absoluteFilePath(), files, subdirs, isRecursion);
			}
		}
	}
}