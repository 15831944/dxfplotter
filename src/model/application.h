#pragma once

#include <model/task.h>
#include <config/config.h>

#include <QObject>

namespace Model
{

class Application : public QObject
{
	Q_OBJECT;

private:
	Config::Config m_config;

	Path::ListPtr m_paths; // TODO parent destruct
	Task *m_task;

	PathSettings defaultPathSettings() const;

	void cutterCompensation(float scale);

public:
	explicit Application();

	Config::Config &config();

	void loadFileFromCmd(const QString &fileName);
	bool loadFile(const QString &fileName);
	bool loadDxf(const QString &fileName);
	void loadPlot(const QString &fileName);

	bool exportToGcode(const QString &fileName);

	void leftCutterCompensation();
	void rightCutterCompensation();
	void resetCutterCompensation();

Q_SIGNALS:
	void taskChanged(Task *newTask);
	void titleChanged(QString title);
};

}
