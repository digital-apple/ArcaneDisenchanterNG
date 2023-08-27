#include "Hooks.h"
#include "Settings.h"

void InitLogging()
{
	auto path = logs::log_directory();
	if (!path)
		return;

	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	*path /= fmt::format("{}.log", plugin->GetName());

	std::vector<spdlog::sink_ptr> sinks{ 
		std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true), 
		std::make_shared<spdlog::sinks::msvc_sink_mt>() 
	};

	auto logger = std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());
	logger->set_level(spdlog::level::info);
	logger->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(logger));
	spdlog::set_pattern("[%^%L%$] %v");
}

void InitMessaging()
{
	logs::trace("Initializing messaging listener.");
	const auto interface = SKSE::GetMessagingInterface();
	if (!interface->RegisterListener([](SKSE::MessagingInterface::Message* a_msg) {
		if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			Addresses::Hook();
		}
		})) {
		stl::report_and_fail("Failed to initialize message listener.");
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	InitLogging();

	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info("{} v{} is loading...", plugin->GetName(), plugin->GetVersion());

	SKSE::Init(a_skse);

	if (Settings::load_config("Data/SKSE/Plugins/Arcane-Disenchanter-NG.toml"s)) {
		logs::info("Loading configuration file.");
	} else {
		logs::info("Couldn't load configuration file!");
	}

	InitMessaging();

	logs::info("{} loaded.", plugin->GetName());

	return true;
}
