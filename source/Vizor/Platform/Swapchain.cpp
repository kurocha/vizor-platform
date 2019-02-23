//
//  Swapchain.cpp
//  This file is part of the "Vizor" project and released under the MIT License.
//
//  Created by Samuel Williams on 20/11/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Swapchain.hpp"

#include <Logger/Console.hpp>

namespace Vizor
{
	namespace Platform
	{
		using namespace Logger;
		
		Swapchain::~Swapchain()
		{
		}
		
		vk::SwapchainKHR Swapchain::swapchain()
		{
			if (!_swapchain) {
				setup_swapchain();
			}
			
			return _swapchain.get();
		}
		
		vk::SurfaceFormatKHR Swapchain::select_surface_format(const std::vector<vk::SurfaceFormatKHR> & surface_formats)
		{
			if (surface_formats.size() == 1 && surface_formats[0].format == vk::Format::eUndefined) {
				return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
			}
			
			for (const auto & surface_format : surface_formats) {
				if (surface_format.format == vk::Format::eB8G8R8A8Unorm && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
					return surface_format;
				}
			}
			
			return surface_formats[0];
		}
		
		vk::PresentModeKHR Swapchain::select_present_mode(const std::vector<vk::PresentModeKHR> & present_modes)
		{
			vk::PresentModeKHR best_mode = vk::PresentModeKHR::eFifo;
			
			for (const auto & present_mode : present_modes) {
				if (present_mode == vk::PresentModeKHR::eMailbox) {
					return present_mode;
				} else if (present_mode == vk::PresentModeKHR::eImmediate) {
					best_mode = present_mode;
				}
			}
			
			return best_mode;
		}
		
		vk::Extent2D Swapchain::select_extent(const vk::SurfaceCapabilitiesKHR & surface_capabilities)
		{
			if (surface_capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
				return surface_capabilities.currentExtent;
			} else {
				return _extent;
			}
		}
		
		void Swapchain::setup_surface_format()
		{
			_surface_format = select_surface_format(
				_physical_device.getSurfaceFormatsKHR(_surface)
			);
		}
		
		void Swapchain::setup_present_mode()
		{
			_present_mode = select_present_mode(
				_physical_device.getSurfacePresentModesKHR(_surface)
			);
		}
		
		void Swapchain::setup_swapchain()
		{
			setup_surface_format();
			setup_present_mode();
			
			auto capabilities = _physical_device.getSurfaceCapabilitiesKHR(_surface);
			
			auto extent = select_extent(capabilities);
			
			std::size_t image_count = capabilities.minImageCount + 1;
			
			if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
				image_count = capabilities.maxImageCount;
			}
			
			auto swapchain_create_info = vk::SwapchainCreateInfoKHR()
				.setSurface(_surface)
				.setMinImageCount(image_count)
				.setImageFormat(_surface_format.format)
				.setImageColorSpace(_surface_format.colorSpace)
				.setImageExtent(extent)
				.setImageArrayLayers(1)
				.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
			
			uint32_t queue_family_indices[] = {
				_queue_family_indices.graphics_queue_family_index,
				_queue_family_indices.present_queue_family_index,
			};
			
			if (queue_family_indices[0] != queue_family_indices[1]) {
				swapchain_create_info
					.setImageSharingMode(vk::SharingMode::eConcurrent)
					.setQueueFamilyIndexCount(2)
					.setPQueueFamilyIndices(queue_family_indices);
			} else {
				swapchain_create_info
					.setImageSharingMode(vk::SharingMode::eExclusive);
			}
			
			swapchain_create_info
				.setPreTransform(capabilities.currentTransform)
				.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
				.setPresentMode(_present_mode)
				.setClipped(true)
				.setOldSwapchain(nullptr);
			
			setup_swapchain(swapchain_create_info);
			
			setup_image_buffers(
				_device.getSwapchainImagesKHR(_swapchain.get())
			);
			
			_extent = extent;
		}
		
		void Swapchain::setup_swapchain(vk::SwapchainCreateInfoKHR & swapchain_create_info)
		{
			_swapchain = _device.createSwapchainKHRUnique(swapchain_create_info, _allocation_callbacks);
		}
		
		void Swapchain::setup_image_buffers(const std::vector<vk::Image> & images)
		{
			using S = vk::ComponentSwizzle;
			
			auto subresource_range = vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0).setLevelCount(1)
				.setBaseArrayLayer(0).setLayerCount(1);
			
			_buffers.resize(0);
			_buffers.reserve(images.size());
			
			for (auto & image : images) {
				auto image_view_create_info = vk::ImageViewCreateInfo()
					.setImage(image)
					.setViewType(vk::ImageViewType::e2D)
					.setFormat(_surface_format.format)
					.setComponents({S::eIdentity, S::eIdentity, S::eIdentity, S::eIdentity})
					.setSubresourceRange(subresource_range);
				
				_buffers.push_back({
					image,
					_device.createImageViewUnique(image_view_create_info, _allocation_callbacks)
				});
			}
		}
	}
}