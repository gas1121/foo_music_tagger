#pragma once

#include <MusicTagger/Release.h>

namespace  MusicTagger {

    //! Dialog to display tag data if query is succeeded
    class TaggerDialog : public CDialogImpl<TaggerDialog> 
    {
    public:
        //! Used dialog
        enum 
		{
            IDD = IDD_TAGGER_DIALOG
        };

        enum columns 
		{
            kArtistColumn,
            kReleaseColumn,
            kDateColumn
        };

        TaggerDialog(pfc::list_t<metadb_handle_ptr> tracks);

		void DisplayDialog(const std::vector<Release>& releaseList);

        BEGIN_MSG_MAP(TaggerDialog)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_SHOWWINDOW(OnShowWindow)
            MSG_WM_CLOSE(OnClose)
            COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
            COMMAND_ID_HANDLER_EX(IDOK, OnOk)
            NOTIFY_HANDLER_EX(IDC_RELEASE_LIST, LVN_ITEMCHANGED, OnReleaseListChange)
            ////CHAIN_MSG_MAP(CDialogImpl<TaggerDialog>)
        END_MSG_MAP()

        bool OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

        void OnShowWindow(BOOL bShow, UINT nStatus);

		LRESULT OnReleaseListChange(LPNMHDR pnmh);

        void OnClose();

        void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);

        void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl);

    private:
		void UpdateRelease();

		bool isLoaded_;
		
		std::vector<Release> releaseList_;
		size_t selectIndex_;
		pfc::list_t<metadb_handle_ptr> tracks_;
    };
}