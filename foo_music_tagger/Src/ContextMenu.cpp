#include "stdafx.h"
#include "QueryThread.h"
#include "TaggerDialog.h"

namespace {
    // Identifier of context menu group.
    static const GUID kGuidGroup = { 0x215f91f9, 0x79e0, 0x43a4,{ 0x9f, 0x8f, 025, 0x20, 0x8d, 0x72, 0xdd, 0xfd } };
}

using namespace MusicTagger;

// Switch to contextmenu_group_factory to embed your commands in the root menu but separated from other commands.

static contextmenu_group_popup_factory gContextMenuGroup(kGuidGroup, contextmenu_groups::tagging, "Music Tagger", 0);

static void RunQueryCommand(metadb_handle_list_cref data);

// Simple context menu item class.
class MusicTaggerItem : public contextmenu_item_simple {
public:
    enum {
        cmd_query = 0,
        cmd_total
    };

    GUID get_parent() { return kGuidGroup; }
    unsigned get_num_items() { return cmd_total; }
    void get_item_name(unsigned p_index, pfc::string_base & p_out) {
        switch (p_index) {
        case cmd_query: p_out = "Get tags"; break;
        default: uBugCheck(); // should never happen unless somebody called us with invalid parameters - bail
        }
    }
    void context_command(unsigned p_index, metadb_handle_list_cref p_data, const GUID& p_caller) {
        switch (p_index) {
        case cmd_query:
            RunQueryCommand(p_data);
            break;
        default:
            uBugCheck();
        }
    }
    // Overriding this is not mandatory. We're overriding it just to demonstrate stuff that you can do such as context-sensitive menu item labels.
    bool context_get_display(unsigned p_index, metadb_handle_list_cref p_data, pfc::string_base & p_out, unsigned & p_displayflags, const GUID & p_caller) {
        switch (p_index) {
        case cmd_query:
            if (!__super::context_get_display(p_index, p_data, p_out, p_displayflags, p_caller)) return false;
            // Example context sensitive label: append the count of selected items to the label.
            p_out << " : " << p_data.get_count() << " item";
            if (p_data.get_count() != 1) p_out << "s";
            p_out << " selected";
            return true;
        default: uBugCheck();
        }
    }
    GUID get_item_guid(unsigned p_index) {
        // These GUIDs identify our context menu items. Substitute with your own GUIDs when reusing code.
        //{006b72e2-5dcb-477a-b64c-7c398d3de292}
        static const GUID guid_query = { 0x006b72e2, 0x5dcb, 0x477a,{ 0xb6, 0x4c, 0x7c, 0x39, 0x8d, 0x3d, 0xe2, 0x92 } };
        switch (p_index) {
        case cmd_query: return guid_query;
        default: uBugCheck(); // should never happen unless somebody called us with invalid parameters - bail
        }

    }
    bool get_item_description(unsigned p_index, pfc::string_base & p_out) {
        switch (p_index) {
        case cmd_query:
            p_out = "Get Tag data from multiple sources.";
            return true;
        default:
            uBugCheck(); // should never happen unless somebody called us with invalid parameters - bail
        }
    }
};

static contextmenu_item_factory_t<MusicTaggerItem> g_myitem_factory;

static void RunQueryCommand(metadb_handle_list_cref data) {

	pfc::list_t<metadb_handle_ptr> tracks;
	tracks.add_items(data);

	//Create tag data window but keep hiding
	TaggerDialog* taggerDialog = new TaggerDialog(tracks);

    //Query tag data
    threaded_process::g_run_modeless(new service_impl_t<QueryThread>(tracks, taggerDialog),
        threaded_process::flag_show_progress | threaded_process::flag_show_abort, core_api::get_main_window(), "Fetching tag data...");
}