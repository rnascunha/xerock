#ifndef MONITOR_APP_TYPES_HPP__
#define MONITOR_APP_TYPES_HPP__

namespace Apps{
namespace Monitor{

enum class Monitor_Type{
	set = 0,
	cancel
};

struct Message_Monitor_Data{
	unsigned int sample_time;
	Monitor_Type type;
	bool cont;
};

}
}

#endif /* MONITOR_APP_TYPES_HPP__ */
