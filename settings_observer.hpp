#pragma once

#include<string>

namespace fons
{
	class settings_observer
	{
	public:
		virtual ~settings_observer() = default;

		virtual void on_repo_select(std::string selected_repo) {};
		virtual void on_repo_found(std::string found_repo) {};
	};
}