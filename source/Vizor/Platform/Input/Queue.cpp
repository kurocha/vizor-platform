//
//  Queue.cpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 23/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#include "Queue.hpp"

namespace Vizor
{
	namespace Platform
	{
		namespace Input
		{
			Queue::Queue()
			{
			}
			
			void Queue::dequeue(Handler & handler)
			{
				// auto items = _queue.dequeue();
				// 
				// for (auto & item : items) {
				// 	handler.process(reinterpret_cast<const Event &>(item));
				// }
			}
			
			bool Queue::process(const ResizeEvent & event)
			{
				// _queue.enqueue(event);
				
				return true;
			}
			
			bool Queue::process(const ButtonEvent & event)
			{
				// _queue.enqueue(event);
				
				return true;
			}
			
			bool Queue::process(const MotionEvent & event)
			{
				// _queue.enqueue(event);
				
				return true;
			}
			
			bool Queue::process(const SystemEvent & event)
			{
				// _queue.enqueue(event);
				
				return true;
			}
		}
	}
}
