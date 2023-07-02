#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/NoteData.hpp"

#include "config.hpp"
#include "main.hpp"

using namespace QuestUI;
using namespace GlobalNamespace;
using namespace UnityEngine;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Returns a logger, useful for printing debug messages
Logger &getLogger() {
	static Logger *logger = new Logger(modInfo);
	return *logger;
}

// stolen from SmallQubes
auto createHoriz(UnityEngine::UI::VerticalLayoutGroup *vertical) {
	auto horiz = BeatSaberUI::CreateHorizontalLayoutGroup(vertical);
	horiz->GetComponent<UnityEngine::UI::LayoutElement *>()->set_minHeight(8);
	horiz->set_childForceExpandHeight(true);
	horiz->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
	return horiz->get_transform();
}

void DidActivate(UnityEngine::GameObject *self, bool firstActivation) {
	if(firstActivation) {
		auto vertical = BeatSaberUI::CreateVerticalLayoutGroup(self->get_transform());
		BeatSaberUI::CreateToggle(createHoriz(vertical), "Enabled", getModConfig().enabled.GetValue(), [](bool enabled) {
			getModConfig().enabled.SetValue(enabled);
		});
		BeatSaberUI::CreateColorPicker(createHoriz(vertical), "Bomb Color", getModConfig().bombColor.GetValue(), [](UnityEngine::Color col) {
			getModConfig().bombColor.SetValue(col);
		});
	}
}
// General format: MAKE_HOOK_MATCH(hook name, hooked method, method return type, method class pointer, arguments...) {
//     HookName(self, arguments...);
//     your code here
// }

MAKE_HOOK_MATCH(BombNoteControllerInit, &GlobalNamespace::BombNoteController::Init, void, GlobalNamespace::BombNoteController *self, GlobalNamespace::NoteData *noteData, float worldRotation, UnityEngine::Vector3 moveStartPos, UnityEngine::Vector3 moveEndPos, UnityEngine::Vector3 jumpEndPos, float moveDuration, float jumpDuration, float jumpGravity) {
	BombNoteControllerInit(self, noteData, worldRotation, moveStartPos, moveEndPos, jumpEndPos, moveDuration, jumpDuration, jumpGravity);
	if(!getModConfig().enabled.GetValue())
		return;
	for(auto m : self->GetComponentInChildren<Renderer *>()->get_materials()) {
		m->SetColor("_SimpleColor", getModConfig().bombColor.GetValue());
		m->SetColor("_Color", getModConfig().bombColor.GetValue());
	}
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo &info) {
	info.id = MOD_ID;
	info.version = VERSION;
	modInfo = info;
	
	getModConfig().Init(modInfo);
	getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
	Init();
	QuestUI::Register::RegisterGameplaySetupMenu(modInfo, "Bomb Qolorer", Register::MenuType::All, DidActivate);

	il2cpp_functions::Init();

	getLogger().info("Installing hooks...");
	
	INSTALL_HOOK(getLogger(), BombNoteControllerInit);

	getLogger().info("Installed all hooks!");
}
