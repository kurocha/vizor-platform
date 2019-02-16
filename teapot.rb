# Teapot v3.0.0 configuration generated at 2019-02-16 19:36:29 +1300

required_version "3.0"

define_project "vizor-platform" do |project|
	project.title = "Vizor Platform"
end

# Build Targets

define_target 'vizor-platform-library' do |target|
	target.depends 'Language/C++14'
	
	target.provides 'Library/VizorPlatform' do
		source_root = target.package.path + 'source'
		
		library_path = build static_library: 'VizorPlatform', source_files: source_root.glob('VizorPlatform/**/*.cpp')
		
		append linkflags library_path
		append header_search_paths source_root
	end
end

define_target 'vizor-platform-test' do |target|
	target.depends 'Library/VizorPlatform'
	target.depends 'Library/UnitTest'
	
	target.depends 'Language/C++14'
	
	target.provides 'Test/VizorPlatform' do |arguments|
		test_root = target.package.path + 'test'
		
		run tests: 'VizorPlatform', source_files: test_root.glob('VizorPlatform/**/*.cpp'), arguments: arguments
	end
end

define_target 'vizor-platform-executable' do |target|
	target.depends 'Library/VizorPlatform'
	
	target.depends 'Language/C++14'
	
	target.provides 'Executable/VizorPlatform' do
		source_root = target.package.path + 'source'
		
		build executable: 'VizorPlatform', source_files: source_root.glob('VizorPlatform.cpp')
	end
end

define_target 'vizor-platform-run' do |target|
	target.depends 'Executable/VizorPlatform'
	
	target.depends :executor
	
	target.provides 'Run/VizorPlatform' do |*arguments|
		run executable: 'VizorPlatform', arguments: arguments
	end
end

# Configurations

define_configuration 'development' do |configuration|
	configuration[:source] = "https://github.com/kurocha"
	configuration.import "vizor-platform"
	
	# Provides all the build related infrastructure:
	configuration.require 'platforms'
	
	# Provides unit testing infrastructure and generators:
	configuration.require 'unit-test'
	
	# Provides some useful C++ generators:
	configuration.require 'generate-cpp-class'
	
	configuration.require "generate-project"
	configuration.require "vizor"
end

define_configuration "vizor-platform" do |configuration|
	configuration.public!
end
