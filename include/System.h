#pragma once

class System
{
public:
    static System* GetSingleton()
	{
		static System singleton;
		return &singleton;
	}

    void ConstructMessageBox();
    auto GetCurrentEntry() -> RE::InventoryEntryData*;
    void SetCurrentEntry(RE::InventoryEntryData* a_entry);
    auto GetExtraHealthList(RE::BSSimpleList<RE::ExtraDataList*>* a_lists) -> RE::ExtraDataList*;
    void RemoveEnchantment(RE::InventoryEntryData* a_entry);
    void UpdateUI();

    static auto ConstructExtraDataList(void* a_this) -> RE::ExtraDataList*;
    static auto GetExtraHealth(RE::ExtraDataList* a_extra) -> float;
    static void SetExtraHealth(RE::ExtraDataList* a_extra, float a_health);
private:
    System() = default;
	System(const System&) = delete;
	System(System&&) = delete;

	~System() = default;

	System& operator=(const System&) = delete;
	System& operator=(System&&) = delete;

    RE::InventoryEntryData* currentEntry;
};