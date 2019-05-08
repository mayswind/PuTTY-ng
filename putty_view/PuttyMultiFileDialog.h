#ifndef PUTTYMULTIFILEDIALOG_H
#define PUTTYMULTIFILEDIALOG_H

#include "ui_base/shell_dialogs/select_file_dialog.h"
#include "base/file_path.h"
#include "Singleton.hpp"

// usage :
//	PuttyMultiFileDialogSingleton::instance()->showOpenDialog(nativeParent, this);

class PuttyMultiFileDialog : public ui::SelectFileDialog::Listener {
public:
	PuttyMultiFileDialog();
	virtual ~PuttyMultiFileDialog();

	void showOpenDialog(gfx::NativeWindow owning_window, void* frontend);

	// SelectFileDialog::Listener implemenation.
	virtual void FileSelected(const FilePath& path,
		                    int index, void* params) OVERRIDE;
	virtual void MultiFilesSelected(const std::vector<FilePath>& files,
		                               void* params) OVERRIDE;
	virtual void FileSelectionCanceled(void* params) OVERRIDE;


private:
	scoped_refptr<ui::SelectFileDialog> select_file_dialog_;

};
typedef DesignPattern::Singleton<PuttyMultiFileDialog> PuttyMultiFileDialogSingleton;

#endif /* PUTTYMULTIFILEDIALOG_H */
