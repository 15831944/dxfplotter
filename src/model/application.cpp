#include <application.h>
#include <geometry/assembler.h>
#include <geometry/cleaner.h>

#include <importer/dxf/importer.h>
#include <exporter/gcode/exporter.h>

#include <common/exception.h>

#include <QMimeDatabase>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

namespace Model
{

static const QString configFileName = "config.ini";

/** Retrieves application config file path
 * @return config file path
 */
static std::string configFilePath()
{
	const QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));

	// Ensure the path exists
	dir.mkpath(".");

	const QString path = dir.filePath(configFileName);

	return path.toStdString();
}

PathSettings Application::defaultPathSettings() const
{
	const Config::Config::DefaultPath &defaultPath = m_config.defaultPath();
	return PathSettings(defaultPath.feedRate(), defaultPath.intensity(), defaultPath.passes());
}

void Application::cutterCompensation(float scale)
{
	const Config::Config::Tool &tool = m_config.tool();
	const Config::Config::Dxf &dxf = m_config.dxf();

	const float radius = tool.radius() * scale;
	m_task->forEachSelectedPath([radius, minimumPolylineLength=dxf.minimumPolylineLength(),
		minimumArcLength=dxf.minimumArcLength()](Model::Path *path){
			path->offset(radius, minimumPolylineLength, minimumArcLength);
	});
}

Application::Application()
	:m_config(configFilePath())
{
}

Config::Config &Application::config()
{
	return m_config;
}

void Application::loadFileFromCmd(const QString &fileName)
{
	if (!fileName.isEmpty()) {
		if (!loadFile(fileName)) {
			qCritical() << "Invalid file type " + fileName;
		}
	}
}

bool Application::loadFile(const QString &fileName)
{
	const QMimeDatabase db;
	const QMimeType mime = db.mimeTypeForFile(fileName);

	if (mime.name() == "image/vnd.dxf") {
		loadDxf(fileName);
	}
	else if (mime.name() == "text/plain") {
		loadPlot(fileName);
	}
	else {
		return false;
	}

	// Update window title based on file name.
	const QString title = QFileInfo(fileName).fileName();
	emit titleChanged(title);

	return true;
}

bool Application::loadDxf(const QString &fileName)
{
	const Config::Config::Dxf &dxf = m_config.dxf();

	Geometry::Polyline::List polylines;
	try {
		// Import data
		Importer::Dxf::Importer imp(fileName.toStdString(), dxf.splineToArcPrecision(), dxf.minimumSplineLength());
		polylines = imp.polylines();
	}
	catch (const Common::FileException &e) {
		return false;
	}

	// Merge polylines to create longest contours
	Geometry::Assembler assembler(std::move(polylines), dxf.assembleTolerance());
	// Remove small bulges
	Geometry::Cleaner cleaner(assembler.polylines(), dxf.minimumPolylineLength(), dxf.minimumArcLength());

	m_paths = Path::FromPolylines(cleaner.polylines(), defaultPathSettings());
	m_task = new Task(this, m_paths);

	emit taskChanged(m_task);

	return true;
}

void Application::loadPlot(const QString &fileName)
{
	
}

bool Application::exportToGcode(const QString &fileName)
{
	// Copy gcode format from config file
	Exporter::GCode::Format format(m_config.gcode());

	try {
		Exporter::GCode::Exporter exporter(m_task, format, fileName.toStdString());
	}
	catch (const Common::FileException &e) {
		return false;
	}

	return true;
}

void Application::leftCutterCompensation()
{
	cutterCompensation(-1.0f);
}

void Application::rightCutterCompensation()
{
	cutterCompensation(1.0f);
}

void Application::resetCutterCompensation()
{
	m_task->forEachSelectedPath([](Model::Path *path){ path->resetOffset(); });
}

}
