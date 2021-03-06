#pragma once

#include <model/task.h>
#include <model/path.h>
#include <model/pathsettings.h>

#include <cassert>

#include <common/function.h>

namespace Model
{

class PathGroupSettings : public QObject
{
	Q_OBJECT;

private:
	const Model::Task *m_task;

	/** Return value of a path settings property if all path have
	 * the same value for the given property.
	 * 
	 * @tparam T The returned value type
	 * @tparam Getter The member function used to acces the poperty.
	 */
	template <typename Getter, typename Return = typename Common::MemberFunctionTraits<Getter>::Return>
	std::optional<Return> valueIfAllEqual(Getter &&getter) const
	{
		Path::ListPtr::const_iterator it = m_task->selectedPaths().begin();

		// Reference value to compare with.
		const Return &reference = ((*it)->settings().*getter)();

		if (std::all_of(++it, m_task->selectedPaths().end(), [reference, &getter](Path *path)
			{
				const Return& value = (path->settings().*getter)();
				return value == reference;
			}))
		{
			return std::make_optional(reference);
		}
		return std::nullopt;
	}

	template <typename Setter, typename T>
	void setValue(Setter &&setter, T value)
	{
		m_task->forEachSelectedPath([value, &setter](Model::Path *path){
			(path->settings().*setter)(value);
		});
	}

public:
	explicit PathGroupSettings(const Task *task);

	std::optional<float> feedRate() const;
	void setFeedRate(float feedRate);

	std::optional<float> intensity() const;
	void setIntensity(float intensity);

	std::optional<int> passes() const;
	void setPasses(int passes);
};

}
