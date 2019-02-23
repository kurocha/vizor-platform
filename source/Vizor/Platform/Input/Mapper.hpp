//
//  Mapper.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 23/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include <unordered_map>

namespace Vizor
{
	namespace Platform
	{
		namespace Input
		{
			template <typename ActionT>
			class Mapper {
			protected:
				using Actions = std::unordered_map<Key, ActionT>;
				Actions _actions;
				
			public:
				void bind(const Key & key, ActionT action) {
					_actions[key] = action;
				}
				
				void bind(DeviceT device, ButtonT button, ActionT action) {
					bind(Key(device, button), action);
				}
				
				// mapper.invoke(key, ->(action){});
				template <typename BlockT>
				bool invoke(const Key & key, BlockT block) const {
					auto action = _actions.find(key);
					
					if (action != _actions.end()) {
						block(action);
						
						return true;
					} else {
						return false;
					}
				}
				
				template <typename BlockT>
				bool invoke(const Key & key) const {
					auto action = _actions.find(key);
					
					if (action != _actions.end()) {
						action();
						
						return true;
					} else {
						return false;
					}
				}
			};
		}
	}
}
