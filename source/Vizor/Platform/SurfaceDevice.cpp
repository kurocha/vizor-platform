//
//  SurfaceDevice.cpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 17/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#include "SurfaceDevice.hpp"

namespace Vizor
{
	namespace Platform
	{
		SurfaceDevice::~SurfaceDevice()
		{
		}
		
		vk::SurfaceKHR SurfaceDevice::surface()
		{
			if (!_surface) {
				GraphicsDevice::setup_device();
			}
			
			return _surface;
		}
		
		void SurfaceDevice::prepare(Layers & layers, Extensions & extensions) const noexcept
		{
			GraphicsDevice::prepare(layers, extensions);
			
			if (_enable_swapchain) {
				extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			}
			
			_window.prepare(layers, extensions);
		}
		
		void SurfaceDevice::setup_queues()
		{
			auto surface = _window.surface();
			
			auto queue_family_properties = _physical_device.getQueueFamilyProperties();
			
			for (std::size_t index = 0; index < queue_family_properties.size(); index += 1) {
				auto & properties = queue_family_properties[index];
				
				if (properties.queueCount == 0) continue;
				
				if (properties.queueFlags & vk::QueueFlagBits::eGraphics) {
					_graphics_queue_family_index = index;
				}
				
				auto can_present = _physical_device.getSurfaceSupportKHR(index, surface);
				
				if (can_present) {
					_present_queue_family_index = index;
					
					if (_graphics_queue_family_index == _present_queue_family_index) {
						break;
					}
				}
			}
			
			if (_graphics_queue_family_index == -1) {
				throw std::runtime_error("Could not find graphics queue!");
			}
			
			if (_present_queue_family_index == -1) {
				throw std::runtime_error("Could not find present queue!");
			}
		}
		
		void SurfaceDevice::setup_device(Layers & layers, Extensions & extensions)
		{
			float queue_priority = 1.0f;
			
			setup_queues();
			
			std::size_t queue_count = (_graphics_queue_family_index == _present_queue_family_index) ? 1 : 2;
			vk::DeviceQueueCreateInfo queue_create_infos[queue_count];
			
			std::size_t index = 0;
			while (index < queue_count) {
				auto queue_family_index = index == 0 ? _graphics_queue_family_index : _present_queue_family_index;
				
				queue_create_infos[index]
					.setQueueCount(1)
					.setQueueFamilyIndex(queue_family_index)
					.setPQueuePriorities(&queue_priority);
			}
			
			auto device_create_info = vk::DeviceCreateInfo()
				.setEnabledLayerCount(layers.size())
				.setPpEnabledLayerNames(layers.data())
				.setEnabledExtensionCount(extensions.size())
				.setPpEnabledExtensionNames(extensions.data())
				.setPQueueCreateInfos(queue_create_infos)
				.setQueueCreateInfoCount(queue_count);
			
			// Enable all features by default:
			auto features = _physical_device.getFeatures();
			device_create_info.setPEnabledFeatures(&features);
			
			GraphicsDevice::setup_device(device_create_info);
			
			_graphics_queue = _device->getQueue(_graphics_queue_family_index, 0);
			_present_queue = _device->getQueue(_present_queue_family_index, 0);
		}
	}
}
