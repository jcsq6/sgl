#include "utils/error.h"
#include <deque>

SGL_BEG
DETAIL_BEG
std::deque<error> queue;

void log_error(error err)
{
	queue.push_back(std::move(err));
}
DETAIL_END

bool get_error(error &err)
{
	if (detail::queue.size())
	{
		err = detail::queue.front();
		detail::queue.pop_front();

		return true;
	}
	else
	{
		err = { "No error.", error_code::no_error };

		return false;
	}
}


SGL_END