//
//  SurfaceDevice.hpp
//  This file is part of the "Vizor" project and released under the MIT License.
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 16/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include "SurfaceContext.hpp"
#include <Vizor/GraphicsDevice.hpp>

namespace Vizor
{
	namespace Platform
	{
		class SurfaceDevice : public GraphicsDevice
		{
		public:
			SurfaceDevice(const PhysicalContext & physical_context, Window & window, bool enable_swapchain = true) : GraphicsDevice(physical_context), _window(window), _enable_swapchain(enable_swapchain) {}
			virtual ~SurfaceDevice();
			
			SurfaceDevice(const SurfaceDevice &) = delete;
			
			vk::SurfaceKHR surface();
			
			std::uint32_t present_queue_family_index() const noexcept {return _present_queue_family_index;}
			vk::Queue present_queue() const noexcept {return _present_queue;}
			
			SurfaceContext context() {return SurfaceContext(GraphicsDevice::context(), present_queue(), surface());}
		
		protected:
			virtual void prepare(Layers & layers, Extensions & extensions) const noexcept override;
			
			virtual void setup_queues() override;
			
			virtual void setup_device(Layers & layers, Extensions & extensions) override;
			
			Window & _window;
			bool _enable_swapchain;
			
			std::uint32_t _present_queue_family_index = -1;
			vk::Queue _present_queue = nullptr;
			
			vk::SurfaceKHR _surface;
		};
	}
}