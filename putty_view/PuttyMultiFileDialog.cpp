#include <vector>

#include "PuttyMultiFileDialog.h"
#include "ui_base/shell_dialogs/select_file_policy.h"
#include "zmodem_session.h"


class PuttySelectMultiFilePolicy : public ui::SelectFilePolicy {
public:
	explicit PuttySelectMultiFilePolicy(){}
	virtual ~PuttySelectMultiFilePolicy(){}

	// Overridden from ui::SelectFilePolicy:
	virtual bool CanOpenSelectFileDialog() { return true; }
	virtual void SelectFileDenied() {}

private:

	DISALLOW_COPY_AND_ASSIGN(PuttySelectMultiFilePolicy);
};

PuttyMultiFileDialog::PuttyMultiFileDialog()
	: select_file_dialog_(ui::SelectFileDialog::Create(
	this, new PuttySelectMultiFilePolicy())) {

}


PuttyMultiFileDialog::~PuttyMultiFileDialog()
{

}

void PuttyMultiFileDialog::showOpenDialog(gfx::NativeWindow owning_window, void* frontend)
{
	select_file_dialog_->SelectFile(
		ui::SelectFileDialog::SELECT_OPEN_MULTI_FILE, string16(),
		FilePath(), NULL,
		0,  // 1-based index for |file_type_info.extensions| to specify default.
		L"",
		owning_window, frontend);
}

// SelectFileDialog::Listener implemenation.
void PuttyMultiFileDialog::FileSelected(const FilePath& path,
	int index, void* params)
{
	ZmodemSession* zSession = (ZmodemSession*)params;
	std::vector<FilePath> files;
	files.push_back(path);
	zSession->onMultiFileSelected(files);
}

// SelectFileDialog::Listener implemenation.
void PuttyMultiFileDialog::MultiFilesSelected(const std::vector<FilePath>& files,
	void* params)
{
	ZmodemSession* zSession = (ZmodemSession*)params;
	zSession->onMultiFileSelected(files);
}

void PuttyMultiFileDialog::FileSelectionCanceled(void* params)
{
	ZmodemSession* zSession = (ZmodemSession*)params;
	zSession->reset();
}