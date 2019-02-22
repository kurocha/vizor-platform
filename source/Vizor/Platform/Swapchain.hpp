//
//  Swapchain.hpp
//  This file is part of the "Vizor" project and released under the MIT License.
//
//  Created by Samuel Williams on 20/11/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include "SurfaceContext.hpp"
#include "Window.hpp"

namespace Vizor
{
	namespace Platform
	{
		class Swapchain : public SurfaceContext
		{
		public:
			struct QueueFamilyIndices
			{
				std::uint32_t graphics_queue_family_index;
				std::uint32_t present_queue_family_index;
			};
			
			Swapchain(const SurfaceContext & surface_context, QueueFamilyIndices queue_family_indices, vk::Extent2D extent) : SurfaceContext(surface_context), _queue_family_indices(queue_family_indices), _extent(extent) {}
			virtual ~Swapchain();
			
			vk::SwapchainKHR swapchain();
			
			struct Buffer {
				vk::Image image;
				vk::UniqueImageView image_view;
			};
			
			vk::SurfaceFormatKHR surface_format() const noexcept {return _surface_format;}
			
			virtual void resize(vk::Extent2D extent);
			
		protected:
			virtual vk::Extent2D select_extent(const vk::SurfaceCapabilitiesKHR & surface_capabilities);
			virtual vk::SurfaceFormatKHR select_surface_format(const std::vector<vk::SurfaceFormatKHR> & surface_formats);
			virtual vk::PresentModeKHR select_present_mode(const std::vector<vk::PresentModeKHR> & present_modes);
			
			virtual void setup_surface_format();
			virtual void setup_present_mode();
			
			virtual void setup_swapchain();
			virtual void setup_swapchain(vk::SwapchainCreateInfoKHR & swapchain_create_info);
			
			virtual void setup_image_buffers(const std::vector<vk::Image> & images);
			
		private:
			QueueFamilyIndices _queue_family_indices;
			
			vk::Extent2D _extent;
			vk::SurfaceFormatKHR _surface_format;
			vk::PresentModeKHR _present_mode = vk::PresentModeKHR::eFifo;
			
			vk::UniqueSwapchainKHR _swapchain;
			
			std::vector<Buffer> _buffers;
		};
	}
}
