//
//  Window.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 24/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include <Display/Native.hpp>
#include <Vizor/Context.hpp>

namespace Vizor
{
	namespace Platform
	{
		using namespace Display;
		
		class Window : public Context, public Native::Window
		{
		public:
			template<typename... Args>
			Window(const Context & context, Args&&... args) : Context(context), Native::Window(std::forward<Args>(args)...) {}
			virtual ~Window();
			
			vk::SurfaceKHR surface();
			
			virtual void prepare(Layers & layers, Extensions & extensions) const noexcept;
			
		protected:
			void setup_surface();
			
			vk::UniqueSurfaceKHR _surface;
		};
	}
}
