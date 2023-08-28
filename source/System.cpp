#include "System.h"

class SystemCallback : public RE::IMessageBoxCallback
{
public:
    SystemCallback() = default;
    ~SystemCallback() override = default;

    void Run(RE::IMessageBoxCallback::Message a_message) override
    {
        const std::int32_t response = static_cast<std::int32_t>(a_message) - 4;
        const auto system = System::GetSingleton();

        if (response == 0) {
            system->RemoveEnchantment(system->GetCurrentEntry());
        }
    }
};

void System::ConstructMessageBox()
{
    const auto factoryManager = RE::MessageDataFactoryManager::GetSingleton();
    const auto strings = RE::InterfaceStrings::GetSingleton();
    const auto gameSettings = RE::GameSettingCollection::GetSingleton();
    const auto sYesText = gameSettings->GetSetting("sYesText");
    const auto sNoText = gameSettings->GetSetting("sNoText");
    const auto sConfirmDisenchant = gameSettings->GetSetting("sConfirmDisenchant");
     
    if (factoryManager && strings) {
        if (const auto factory = factoryManager->GetCreator<RE::MessageBoxData>(strings->messageBoxData)) {
            if (const auto messageBox = factory->Create()) {
                if (sConfirmDisenchant) {
                    messageBox->bodyText = sConfirmDisenchant->GetString();
                }
                
                if (sYesText && sNoText) {
                    messageBox->buttonText.push_back(sYesText->GetString());
                    messageBox->buttonText.push_back(sNoText->GetString());
                }

                messageBox->unk38 = 10;
                messageBox->callback = RE::BSTSmartPointer<RE::IMessageBoxCallback>{ new SystemCallback() };
                messageBox->unk4C = 4;
                messageBox->QueueMessage();
            }
        }
    }
}

auto System::GetCurrentEntry() -> RE::InventoryEntryData*
{
    return currentEntry;
}

void System::SetCurrentEntry(RE::InventoryEntryData* a_entry)
{
    currentEntry = a_entry;
}

auto System::GetExtraHealthList(RE::BSSimpleList<RE::ExtraDataList*>* a_lists) -> RE::ExtraDataList*
{
    if (a_lists) {
        for (const auto& xList : *a_lists) {
            if (xList && xList->GetByType<RE::ExtraHealth>()) {
                return xList;
            }
        }
    }
    return nullptr;
}

void System::RemoveEnchantment(RE::InventoryEntryData* a_entry)
{
    if (a_entry) {
        auto item = a_entry->object;

        if (a_entry->extraLists) {
            for (const auto& xList : *a_entry->extraLists) {
                if (xList) {
                    auto xEnchantment = xList->GetByType<RE::ExtraEnchantment>();

                    if (xEnchantment) {
                        xList->Remove(RE::ExtraDataType::kEnchantment, xEnchantment);
                    }

                    auto xCharge = xList->GetByType<RE::ExtraCharge>();
                    
                    if (xCharge) {
                        xList->Remove(RE::ExtraDataType::kCharge, xCharge);
                    }
                }
            }
        }

        RE::TESBoundObject* templateItem = nullptr;

        if (item && item->IsArmor()) {
            templateItem = item->As<RE::TESObjectARMO>()->templateArmor;
        }

        if (item && item->IsWeapon()) {
            templateItem = item->As<RE::TESObjectWEAP>()->templateWeapon;
        }

        if (templateItem) {
            auto xListOld = GetExtraHealthList(a_entry->extraLists);
            const auto player = RE::PlayerCharacter::GetSingleton();           

            if (xListOld) {
                auto xListNew = ConstructExtraDataList(RE::MemoryManager::GetSingleton()->Allocate(0x20, 0, false));
                SetExtraHealth(xListNew, GetExtraHealth(xListOld));

                player->RemoveItem(item, 1, RE::ITEM_REMOVE_REASON::kRemove, xListOld, nullptr);
                player->AddObjectToContainer(templateItem, xListNew, 1, nullptr);                
            } else {
                player->RemoveItem(item, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
                player->AddObjectToContainer(templateItem, nullptr, 1, nullptr);
            }
        }

        UpdateUI();
    }
}

void System::UpdateUI()
{
    const auto queue = RE::UIMessageQueue::GetSingleton();
    const auto strings = RE::InterfaceStrings::GetSingleton();
    const auto tasks = SKSE::GetTaskInterface();
        
    tasks->AddUITask([queue, strings]() {
        queue->AddMessage(strings->craftingMenu, RE::UI_MESSAGE_TYPE::kHide, nullptr);
        queue->AddMessage(strings->craftingMenu, RE::UI_MESSAGE_TYPE::kShow, nullptr);
    });
}

auto System::ConstructExtraDataList(void* a_this) -> RE::ExtraDataList*
{
    using func_t = decltype(&ConstructExtraDataList);
    REL::Relocation<func_t> func{ RELOCATION_ID(11437, 11583) };
    return func(a_this);
}

auto System::GetExtraHealth(RE::ExtraDataList* a_extra) -> float
{
    using func_t = decltype(&GetExtraHealth);
    REL::Relocation<func_t> func{ RELOCATION_ID(11557, 11703) };
    return func(a_extra);
}

void System::SetExtraHealth(RE::ExtraDataList* a_extra, float a_health)
{
    using func_t = decltype(&SetExtraHealth);
    REL::Relocation<func_t> func{ RELOCATION_ID(11470, 11616) };
    return func(a_extra, a_health);
}