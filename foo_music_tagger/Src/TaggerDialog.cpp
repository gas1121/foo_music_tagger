#include "stdafx.h"
#include "TaggerDialog.h"

namespace MusicTagger {

    TaggerDialog::TaggerDialog(pfc::list_t<metadb_handle_ptr> tracks) 
		:CDialogImpl<TaggerDialog>(), isLoaded_(false), selectIndex_(0), tracks_(tracks)
    {
        Create(core_api::get_main_window());
	}

	void TaggerDialog::DisplayDialog(const std::vector<Release>& releaseList)
	{
		for (const auto& release:releaseList)
		{
			assert(release.trackTitle.size() == tracks_.get_size());
		}

		releaseList_ = releaseList;
		ShowWindow(SW_SHOW);
	}

	bool TaggerDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
    {
        static_api_ptr_t<modeless_dialog_manager>()->add(m_hWnd);

		// List view styles
		auto styles = LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP;
		CListViewCtrl releaseList = GetDlgItem(IDC_RELEASE_LIST);
		releaseList.SetExtendedListViewStyle(styles, styles);

		// Adding release list columns
		listview_helper::insert_column(releaseList, kArtistColumn, "Artist", 100);
		listview_helper::insert_column(releaseList, kReleaseColumn, "Release", 110);
		listview_helper::insert_column(releaseList, kDateColumn, "Date", 45);

		CListViewCtrl trackList = GetDlgItem(IDC_TRACK_LIST);
		trackList.SetExtendedListViewStyle(styles, styles);
		// Adding track list columns
		listview_helper::insert_column(trackList, 0, "", 0); // Fake column
		trackList.InsertColumn(1, L"#", LVCFMT_RIGHT, 30);
		trackList.DeleteColumn(0);
        listview_helper::insert_column(trackList, 1, "Title", 125);
        listview_helper::insert_column(trackList, 2, "Artist", 100);

        return true;
    }

    void TaggerDialog::OnShowWindow(BOOL bShow, UINT nStatus)
    {
		assert(!releaseList_.empty());

		SetMsgHandled(FALSE);
		if (isLoaded_)
		{
			return;
		}
		isLoaded_ = true;

		CListViewCtrl releaseList = GetDlgItem(IDC_RELEASE_LIST);
		for (size_t i = 0;i<releaseList_.size();i++)
		{
			// Artist
			listview_helper::insert_item(releaseList, i, releaseList_[i].albumArtist.c_str(), NULL);
			// Title
			listview_helper::set_item_text(releaseList, i, kReleaseColumn, releaseList_[i].title.c_str());
			// Date
			listview_helper::set_item_text(releaseList, i, kDateColumn, releaseList_[i].date.c_str());
		}

		UpdateRelease();
    }

	LRESULT TaggerDialog::OnReleaseListChange(LPNMHDR pnmh)
	{
		if (((LPNMLISTVIEW)pnmh)->iItem != -1 && ((LPNMLISTVIEW)pnmh)->uChanged & LVIS_DROPHILITED && ((LPNMLISTVIEW)pnmh)->uNewState & LVIS_SELECTED) {
			if (selectIndex_ != ((LPNMITEMACTIVATE)pnmh)->iItem) {
				selectIndex_ = ((LPNMITEMACTIVATE)pnmh)->iItem;
				UpdateRelease();
			}
		}
		return 0;
	}

	void TaggerDialog::OnClose()
    {
        DestroyWindow();
    }

    void TaggerDialog::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
    {
        DestroyWindow();
    }

    void TaggerDialog::OnOk(UINT uNotifyCode, int nID, CWindow wndCtl)
    {
		static_api_ptr_t<metadb_io_v2> dbio;
		const Release& usedRelease = releaseList_[selectIndex_];
		for (size_t i=0;i<usedRelease.trackTitle.size();i++)
		{
			file_info_impl info;
			tracks_[i]->get_info(info);

            if (!usedRelease.title.empty())
            {
                info.meta_set("album", usedRelease.title.c_str());
            }
            if (!usedRelease.albumArtist.empty())
            {
                info.meta_set("album artist", usedRelease.albumArtist.c_str());
            }
            if (!usedRelease.date.empty())
            {
                info.meta_set("date", usedRelease.date.c_str());
            }
            if (!usedRelease.genre.empty())
            {
                info.meta_set("date", usedRelease.date.c_str());
            }
            if (!usedRelease.trackTitle[i].empty())
            {
                info.meta_set("title", usedRelease.trackTitle[i].c_str());
            }
            if (!usedRelease.trackArtist[i].empty())
            {
                info.meta_set("artist", usedRelease.trackArtist[i].c_str());
            }
            else
            {
                info.meta_set("artist", usedRelease.albumArtist.c_str());
            }
            info.meta_set("tracknumber", std::to_string(i + 1).c_str());
            info.meta_set("totaltracks", std::to_string(usedRelease.trackTitle.size()).c_str());

            dbio->update_info_async_simple(pfc::list_single_ref_t<metadb_handle_ptr>(tracks_[i]),
                pfc::list_single_ref_t<const file_info*>(&info), core_api::get_main_window(),
                metadb_io_v2::op_flag_delay_ui, nullptr);
		}

        DestroyWindow();
    }

	void TaggerDialog::UpdateRelease()
	{
		assert(!releaseList_.empty());
		assert(selectIndex_ >= 0);
		assert(selectIndex_ < releaseList_.size());

		const Release& release = releaseList_[selectIndex_];

		auto artist = GetDlgItem(IDC_ARTIST);
		uSetWindowText(artist, release.albumArtist.c_str());

		auto album = GetDlgItem(IDC_ALBUM);
		uSetWindowText(album, release.title.c_str());

		auto date = GetDlgItem(IDC_DATE);
		uSetWindowText(date, release.date.c_str());

        CListViewCtrl trackListView = GetDlgItem(IDC_TRACK_LIST);
        trackListView.DeleteAllItems();
        for (size_t i = 0; i < release.trackTitle.size(); i++)
        {
            // number
            listview_helper::insert_item(trackListView, i, std::to_string(i+1).c_str(), NULL);
            // Title
            listview_helper::set_item_text(trackListView, i, 1, release.trackTitle[i].c_str());
            //track artist if exist
            if (!release.trackArtist[i].empty())
            {
                listview_helper::set_item_text(trackListView, i, 2, release.trackArtist[i].c_str());
            }
        }
	}

}
