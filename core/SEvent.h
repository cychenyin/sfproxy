/*
 * Callback.h
 *
 *  Created on: Apr 2, 2014
 *      Author: asdf
 */
//http://blog.csdn.net/xqq771084591/article/details/7742412
#ifndef SEVENT_H_
#define SEVENT_H_

namespace FinagleRegistryProxy {

// single handle event
// pass parameter a to Class's eventhandler method
template<class Class, typename EventSourceType, typename StateType>
class SEvent {
public:
	typedef void (Class::*EventHandler)(EventSourceType*, StateType);

	SEvent(Class* _class_instance, EventHandler _method) {
		target = _class_instance;
		handler = _method;
	};

	void operator()(EventSourceType *from, StateType state) {
		invoke(from, state);
	};

	void invoke(EventSourceType *from, StateType state) {
		(target->*handler)(from, state);
	};

private:
	Class* target;
	EventHandler handler;
};

} /* namespace FinagleRegistryProxy */

#endif /* SEVENT_H_ */
