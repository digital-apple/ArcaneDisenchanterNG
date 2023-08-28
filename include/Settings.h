#pragma once

class Settings
{
public:
    using bSetting = AutoTOML::bSetting;
    using iSetting = AutoTOML::iSetting;
    using sSetting = AutoTOML::sSetting;

    static inline bSetting DisableItemDestruction{ "Patches", "DisableItemDestruction", true };

    static inline bSetting RequiresPerk{ "Settings", "RequiresPerk", false };
    static inline iSetting RequiredPerkID{ "Settings", "RequiredPerkID", 0xBEE97 };
    static inline sSetting RequiredPerkModName{ "Settings", "RequiredPerkModName", "Skyrim.esm" };

    static bool load_config(const std::string& a_path)
    {
        try {
            const auto table = toml::parse_file(a_path);
            const auto& settings = ISetting::get_settings();

            for (const auto& setting : settings) {
                try {
                    setting->load(table);
                } catch (const std::exception& e) {
                    logs::warn(fmt::runtime(e.what()));
                }
            }
        } catch (const toml::parse_error& e) {
            std::ostringstream ss;
            ss
                << "Error parsing file \'" << *e.source().path
                << "\':\n"
                << e.description()
                << "\n  (" << e.source().begin << ")\n";
            logs::error(fmt::runtime(ss.str()));
            return false;
        }
        return true;
    }
private:
    using ISetting = AutoTOML::ISetting;

    Settings();   
};