#pragma once

namespace MusicTagger {
	class TaggerDialog;

	class QueryThread : public threaded_process_callback
	{
	public:
		QueryThread(pfc::list_t<metadb_handle_ptr> tracks, TaggerDialog* targetDialog);

		virtual void run(threaded_process_status & p_status, abort_callback & p_abort) override;

	private:
        //! Get useful data from tracks and set query json string.
        /*!
             @return false if can't get useful data from tracks or selected tracks are not valid for querying.
        */
        bool PrepareQueryJsonString(std::string* queryJsonString);

		//! parent window
		TaggerDialog* taggerDialog_;
        pfc::list_t<metadb_handle_ptr> tracks_;
    };

}