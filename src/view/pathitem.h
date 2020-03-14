#pragma once

#include <model/path.h>

#include <QGraphicsPathItem>
#include <QPen>

namespace View
{

class PathItem : public QObject, public QGraphicsPathItem
{
	Q_OBJECT;

private:
	Model::Path *m_path;
	QPainterPath m_paintPath;
	QPainterPath m_shapePath;

	QPainterPath paintPath() const;
	QPainterPath shapePath() const;

public:
	explicit PathItem(Model::Path *path);
	QPainterPath shape() const override;

protected Q_SLOTS:
	void selected();
	void deselected();
};

}
