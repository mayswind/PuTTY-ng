#include <assert.h>
#include <stdlib.h>

#include "putty.h"
#include "dialog.h"
#include "storage.h"

static const char* all_key_str[] = {"TAB", "`~",     "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-_",         "=+",        "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[{",     "]}",     "\\|",    "a", "s", "d", "f", "g", "h", "j", "k", "l", ";",      "'\"",    "z", "x", "c", "v", "b", "n", "m", ",<",         ".>",          "/?" };
static const int all_key_val[] = { VK_TAB, VK_OEM_3, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', VK_OEM_MINUS, VK_OEM_PLUS, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', VK_OEM_4, VK_OEM_6, VK_OEM_5, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', VK_OEM_1, VK_OEM_7, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', VK_OEM_COMMA, VK_OEM_PERIOD, VK_OEM_2 };
static const int all_key_val_count = sizeof(all_key_str) / sizeof(all_key_str[0]);

static void global_key_radiobutton_handler(union control *ctrl, void *dlg,
	void *data, int event)
{
	char* key = (char*)ctrl->checkbox.context.p;
	int button;
	Conf *conf = (Conf *)data;

	/*
	* For a standard radio button set, the context parameter gives
	* the primary key (CONF_foo), and the extra data per button
	* gives the value the target field should take if that button
	* is the one selected.
	*/
	if (event == EVENT_REFRESH) {
		int val = global_conf_get_int(key);
		for (button = 0; button < ctrl->radio.nbuttons; button++)
			if (val == ctrl->radio.buttondata[button].i)
				break;
		/* We expected that `break' to happen, in all circumstances. */
		assert(button < ctrl->radio.nbuttons);
		dlg_radiobutton_set(ctrl, dlg, button);
	}
	else if (event == EVENT_VALCHANGE) {
		button = dlg_radiobutton_get(ctrl, dlg);
		assert(button >= 0 && button < ctrl->radio.nbuttons);
		save_global_isetting(key,
			ctrl->radio.buttondata[button].i);
	}
}

static void global_key_checkbox_handler(union control *ctrl, void *dlg,
	void *data, int event)
{
	char* key = (char*)ctrl->checkbox.context.p;
	if (event == EVENT_REFRESH) {
		int val = load_global_isetting(key, 1);
		dlg_checkbox_set(ctrl, dlg, val);
	}
	else if (event == EVENT_VALCHANGE) {
		int val = dlg_checkbox_get(ctrl, dlg);
		save_global_isetting(key, val);
	}
}

static void global_key_checkbox_handler2(union control *ctrl, void *dlg,
	void *data, int event)
{
	char* key = (char*)ctrl->checkbox.context.p;
	if (event == EVENT_REFRESH) {
		int val = load_global_isetting(key, 0);
		dlg_checkbox_set(ctrl, dlg, val);
	}
	else if (event == EVENT_VALCHANGE) {
		int val = dlg_checkbox_get(ctrl, dlg);
		save_global_isetting(key, val);
	}
}

static void global_key_editbox_handler(union control *ctrl, void *dlg,
	void *data, int event)
{
	char* key = (char*)ctrl->checkbox.context.p;
	int length = ctrl->editbox.context2.i;
	Conf *conf = (Conf *)data;

	if (length > 0) {
		if (event == EVENT_REFRESH) {
			char *field = load_global_ssetting(key, "");
			dlg_editbox_set(ctrl, dlg, field);
		}
		else if (event == EVENT_VALCHANGE) {
			char *field = dlg_editbox_get(ctrl, dlg);
			save_global_ssetting(key, field);
			sfree(field);
		}
	}
	else if (length < 0) {
		if (event == EVENT_REFRESH) {
			char str[80];
			int value = global_conf_get_int(key);
			if (length == -1)
				sprintf(str, "%d", value);
			else
				sprintf(str, "%g", (double)value / (double)(-length));
			dlg_editbox_set(ctrl, dlg, str);
		}
		else if (event == EVENT_VALCHANGE) {
			char *str = dlg_editbox_get(ctrl, dlg);
			if (length == -1)
				save_global_isetting(key, atoi(str));
			else
				save_global_isetting(key, (int)((-length) * atof(str)));
			sfree(str);
		}
	}
}

static void shortcut_keys_handler(union control *ctrl, void *dlg,
	void *data, int event);
static void shortcut_type_handler(union control *ctrl, void *dlg,
	void *data, int event)
{
	char* key = (char*)ctrl->generic.context.p;
	int def_val = ctrl->generic.subkey.i;
	union control* key_ctrl = (union control*)ctrl->listbox.context2.p;
	int mask = memcmp(key, SHORTCUT_KEY_SELECT_TAB, strlen(SHORTCUT_KEY_SELECT_TAB)) == 0 ? MASK_NO_FN : MASK_ALL;
	if (event == EVENT_REFRESH) {
		int val = load_global_isetting(key, def_val);
		dlg_update_start(ctrl, dlg);
		dlg_listbox_clear(ctrl, dlg);
		bool cfg_valid = false;
		for (int i = 0; i < all_key_type_count; i++){
			if ((1 << i) & mask){
				dlg_listbox_addwithid(ctrl, dlg, all_shortcut_type_str[i], i);
				if (i == val){ cfg_valid = true; }
			}
		}
		if (!cfg_valid){ val = def_val; }
		dlg_listbox_select(ctrl, dlg, val);
		dlg_update_done(ctrl, dlg);
		dlg_refresh(key_ctrl, dlg);
	}
	else if (event == EVENT_SELCHANGE) {
		int i = dlg_listbox_index(ctrl, dlg);
		if (i >= 0){
			i = dlg_listbox_getid(ctrl, dlg, i);
			if ((1 << i) & mask){
				save_global_isetting(key, i);
			}
		}
		dlg_refresh(key_ctrl, dlg);
	}
}

static void shortcut_keys_handler(union control *ctrl, void *dlg,
	void *data, int event)
{
	char* key = (char*)ctrl->generic.context.p;
	int def_val = ctrl->generic.subkey.i;
	union control* type_ctrl = (union control*)ctrl->listbox.context2.p;
	if (event == EVENT_REFRESH) {
		int type = dlg_listbox_index(type_ctrl, dlg);
		if (type < 0){
			dlg_show_ctrl(ctrl, dlg, FALSE);
			return;
		}
		type = dlg_listbox_getid(type_ctrl, dlg, type);
		if (((1 << type) & MASK_NO_FN) == 0){
			dlg_show_ctrl(ctrl, dlg, FALSE);
			return;
		}
		dlg_show_ctrl(ctrl, dlg, TRUE);

		int val = load_global_isetting(key, def_val);
		dlg_update_start(ctrl, dlg);
		dlg_listbox_clear(ctrl, dlg);
		int index = -1;
		int def_index = -1;
		for (int i = 0; i < all_key_val_count; i++){
			dlg_listbox_addwithid(ctrl, dlg, all_key_str[i], all_key_val[i]);
			if (all_key_val[i] == val){ index = i; }
			if (all_key_val[i] == def_val){ def_index = i; }
		}
		if (index < 0){ index = def_index; }
		dlg_listbox_select(ctrl, dlg, index);
		dlg_update_done(ctrl, dlg);
	}
	else if (event == EVENT_SELCHANGE) {
		int i = dlg_listbox_index(ctrl, dlg);
		if (i >= 0){
			i = dlg_listbox_getid(ctrl, dlg, i);
			save_global_isetting(key, i);
		}
	}
}

#define ADD_SHORTCUT_KEY_TYPE(name, save_key) \
		c = ctrl_checkbox(s, name, '\0', HELPCTX(no_help), global_key_checkbox_handler, P(save_key "Enable")); \
		c->generic.column = 0; \
			\
			c = ctrl_droplist(s, NULL, '\0', 100, HELPCTX(no_help), shortcut_type_handler, P(save_key "Type")); \
		c->generic.column = 1;\
		c->generic.subkey = I(get_default_shortcut_keytype(save_key)); \
union control * type_ctrl = c;

#define ADD_SHORTCUT_KEY(name, save_key) \
	{\
		ADD_SHORTCUT_KEY_TYPE(name, save_key) \
		\
		c = ctrl_droplist(s, NULL, '\0', 100, HELPCTX(no_help), shortcut_keys_handler, P(save_key "Key")); \
		c->generic.column = 2;\
		c->generic.subkey = I(get_default_shortcut_keyval(save_key)); \
		c->listbox.context2 = P(type_ctrl);\
		type_ctrl->listbox.context2 = P(c);\
	}

#define ADD_SHORTCUT_KEY_LABEL(name, save_key, label_value) \
{\
	ADD_SHORTCUT_KEY_TYPE(name, save_key) \
	\
	c = ctrl_text(s, label_value, HELPCTX(no_help)); \
	c->generic.column = 2; \
	type_ctrl->listbox.context2 = P(c); \
}

void global_setup_config_box(struct controlbox *b)
{
    struct controlset *s;
    union control *c;
	
	char* str = dupprintf("Options controlling %s's window", appname);
	ctrl_settitle(b, WINDOW_SETTING_NAME, str);
	sfree(str);

	s = ctrl_getset(b, WINDOW_SETTING_NAME, "size", "Set the size of the window");
	ctrl_columns(s, 2, 50, 50);
	c = ctrl_editbox(s, "Columns", 'm', 100,
		HELPCTX(window_size),
		global_key_editbox_handler, P(WINDOW_WIDTH_KEY), I(-1));
	c->generic.column = 0;
	c = ctrl_editbox(s, "Rows", 'r', 100,
		HELPCTX(window_size),
		global_key_editbox_handler, P(WINDOW_HEIGHT_KEY), I(-1));
	c->generic.column = 1;
	ctrl_columns(s, 1, 100);

	/*
	* Resize-by-changing-font is a Windows insanity.
	*/
	ctrl_radiobuttons(s, "When window is resized:", 'z', 1,
		HELPCTX(window_resize),
		global_key_radiobutton_handler,
		P(WINDOW_RESIZE_ACTION_KEY),
		"Change the number of rows and columns", I(RESIZE_TERM),
		"Change the size of the font", I(RESIZE_FONT),
		"Change font size only when maximised", I(RESIZE_EITHER),
		"Forbid resizing completely", I(RESIZE_DISABLED), NULL);

	ctrl_checkbox(s, "Maximize window on startup", '\0', HELPCTX(no_help), global_key_checkbox_handler2, P(WINDOW_MAXIMIZED));

	ctrl_settitle(b, SHORTCUT_SETTING_NAME, "Global Shortcut Settings");
	s = ctrl_getset(b, SHORTCUT_SETTING_NAME, "~general", "Function to Keys(No check for duplicated keys)");

	ctrl_columns(s, 3, 55, 25, 20);
	ADD_SHORTCUT_KEY_LABEL("Select Tab", SHORTCUT_KEY_SELECT_TAB, "       Num");
	ADD_SHORTCUT_KEY("Select Next Tab", SHORTCUT_KEY_SELECT_NEXT_TAB);
	ADD_SHORTCUT_KEY("Select Previous Tab", SHORTCUT_KEY_SELECT_PRE_TAB);
	ADD_SHORTCUT_KEY("Edit Tab Title", SHORTCUT_KEY_EDIT_TAB_TITLE);
	ADD_SHORTCUT_KEY("Create NEW Session", SHORTCUT_KEY_NEW_TAB);
	ADD_SHORTCUT_KEY("Duplicate Session", SHORTCUT_KEY_DUP_TAB);
	ADD_SHORTCUT_KEY("Rename Session In Config Dlg ", SHORTCUT_KEY_RENAME_SESSION);
	ADD_SHORTCUT_KEY("Reload Session", SHORTCUT_KEY_RELOAD_TAB);
	ADD_SHORTCUT_KEY("Close Session", SHORTCUT_KEY_CLOSE_TAB);
	ADD_SHORTCUT_KEY("Show/Hide Toolbar", SHORTCUT_KEY_HIDE_SHOW_TOOLBAR);
	c = ctrl_text(s, "Note", HELPCTX(no_help));
	c = ctrl_text(s, "1. If duplicated, only the first one in code logic will be valided.", HELPCTX(no_help));
	c = ctrl_text(s, "2. The keys are implemented with US keyboard layout. With others, please try, God will bless you.", HELPCTX(no_help));
	c = ctrl_text(s, "3. Some keys can't be caught, like Alt+Tab or Alt in Config Dialog. Please try another one, as God doesn't stand on your side.", HELPCTX(no_help));


	ctrl_settitle(b, BUGS_SETTING_NAME, "Walkarounds for global issue");
	s = ctrl_getset(b, BUGS_SETTING_NAME, "~general", "Global Issues");
	c = ctrl_checkbox(s, "Disable Windows Native Theme.", '\0', HELPCTX(no_help), global_key_checkbox_handler, P(DISABLE_WINDOWS_NATIVE_THEME_KEY));
	c = ctrl_text(s, "-- A walkaround for the tab layerout issue on some Win10 platform. It is not reproduced on my Win10. So there is no fix solution. Restart needed if changed.)", HELPCTX(no_help));
}
