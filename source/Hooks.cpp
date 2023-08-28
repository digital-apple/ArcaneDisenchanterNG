#include "Hooks.h"
#include "Settings.h"
#include "System.h"

namespace Addresses
{
    namespace DisableItemDestruction
    {
        void ProcessEntry(RE::InventoryEntryData* a_entry)
        {
            if (a_entry) {
                const auto system = System::GetSingleton();
                system->RemoveEnchantment(a_entry);
            }
        }

        void Hook()
        {
            REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(50459, 51363), 0xE5 };

            struct Patch : Xbyak::CodeGenerator
            {
                Patch(std::uintptr_t a_function, std::uintptr_t a_address)
                {
                    Xbyak::Label functionLabel;
                    Xbyak::Label returnLabel;

                    mov(rcx, r14);
                    call(ptr[rip + functionLabel]);
                    jmp(ptr[rip + returnLabel]);

                    L(functionLabel);
                    dq(a_function);

                    L(returnLabel);
                    dq(a_address + 0x35);
                }
            };

            Patch patch(reinterpret_cast<uintptr_t>(DisableItemDestruction::ProcessEntry), target.address());
            patch.ready();

            auto& trampoline = SKSE::GetTrampoline();
            SKSE::AllocTrampoline(45);
            trampoline.write_branch<5>(target.address(), trampoline.allocate(patch));

            REL::safe_fill(target.address() + 0x5, REL::NOP, 0x30);
        }
    }

    namespace RemoveEnchantment
    {
        void ProcessEntry(RE::InventoryEntryData* a_entry)
        {
            if (a_entry) {
                if (*Settings::RequiresPerk) {
                    const auto perk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(static_cast<RE::FormID>(*Settings::RequiredPerkID), *Settings::RequiredPerkModName);
                    if (perk) {
                        if (!RE::PlayerCharacter::GetSingleton()->HasPerk(perk)) {
                            return;
                        }
                    } else {
                        logs::warn("Addresses::RemoveEnchantment::ProcessEntry :: Invalid perk '{:x}' + '{}'", *Settings::RequiredPerkID, *Settings::RequiredPerkModName);
                    }
                } 
                const auto system = System::GetSingleton();
                system->SetCurrentEntry(a_entry);
                system->ConstructMessageBox();
            }
        }

        void Hook()
        {
            REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(50440, 51344), REL::Relocate(0x77, 0x7E) };

            struct Patch : Xbyak::CodeGenerator
            {
                Patch(std::uintptr_t a_function, std::uintptr_t a_address)
                {
                    Xbyak::Label functionLabel;
                    Xbyak::Label returnLabel;

                    if SKYRIM_REL_CONSTEXPR (REL::Module::IsAE()) {
                        mov(rcx, ptr[rcx + 0x18]);
                    } else {
                        mov(rcx, ptr[rax + 0x18]);
                    }

                    call(ptr[rip + functionLabel]);
                    jmp(ptr[rip + returnLabel]);

                    L(functionLabel);
                    dq(a_function);

                    L(returnLabel);
                    dq(a_address + 0x1E);
                }
            };

            Patch patch(reinterpret_cast<uintptr_t>(RemoveEnchantment::ProcessEntry), target.address());
            patch.ready();

            auto& trampoline = SKSE::GetTrampoline();
            SKSE::AllocTrampoline(46);
            trampoline.write_branch<5>(target.address(), trampoline.allocate(patch));

            REL::safe_fill(target.address() + 0x5, REL::NOP, 0x19);
        }
    }

    void Hook()
    {
        if (*Settings::DisableItemDestruction) {
            DisableItemDestruction::Hook();

            logs::info("Addresses :: Patched 'DisableItemDestruction'.");
        }

        RemoveEnchantment::Hook();
        logs::info("Addresses :: Patched 'RemoveEnchantment'.");
    }
}