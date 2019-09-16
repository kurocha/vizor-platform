# Teapot v3.0.0 configuration generated at 2019-02-16 19:36:29 +1300

required_version "3.0"

define_project "vizor-platform" do |project|
	project.title = "Vizor Platform"
end

# Build Targets

define_target 'vizor-platform-library' do |target|
	target.depends 'Language/C++14'
	
	target.depends 'Library/Vizor', public: true
	target.depends :display_native, public: true
	
	target.depends 'Library/Streams', public: true
	target.depends 'Library/Input', public: true
	
	target.provides 'Library/Vizor/Platform' do
		source_root = target.package.path + 'source'
		
		library_path = build static_library: 'VizorPlatform', source_files: source_root.glob('Vizor/Platform/**/*.{cpp}')
		
		append linkflags library_path
		append header_search_paths source_root
	end
end

define_target 'vizor-platform-test-shaders' do |target|
	target.depends :platform
	target.depends 'Convert/GLSLang'
	
	target.provides 'Assets/Vizor/Platform/Shaders' do
		test_root = target.package.path + 'test'
		cache_prefix = environment[:build_prefix] / environment.checksum + "shaders"
		
		convert source: test_root.glob('**/*.{frag,vert}'), root: cache_prefix
		
		append asset_search_paths cache_prefix
	end
end

define_target 'vizor-platform-test' do |target|
	target.depends 'Library/Vizor/Platform'
	target.depends 'Library/UnitTest'
	
	target.depends 'Language/C++14'
	
	target.provides 'Test/Vizor/Platform' do |arguments|
		test_root = target.package.path + 'test'
		
		run tests: 'VizorPlatform', source_files: test_root.glob('Vizor/Platform/**/*.cpp'), arguments: arguments
	end
end

# Configurations

define_configuration 'development' do |configuration|
	configuration[:source] = "https://github.com/kurocha"
	configuration.import "vizor-platform"
	
	configuration.require 'glslang'
	
	# Provides all the build related infrastructure:
	configuration.require 'platforms'
	
	# Provides unit testing infrastructure and generators:
	configuration.require 'unit-test'
	
	# Provides some useful C++ generators:
	configuration.require 'generate-cpp-class'
	
	configuration.require "generate-project"
end

define_configuration "vizor-platform" do |configuration|
	configuration.public!
	
	configuration.require "vizor"
	
	configuration.require "input"
	
	host /darwin/ do
		configuration.require "display-cocoa"
	end
	
	host /linux/ do
		configuration.require "display-xcb"
	end
end
