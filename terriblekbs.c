#include <linux/keyboard.h>
#include <linux/input.h>
#include "terriblekbs.h"

/*
 * Keymap references:
 * https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
 * http://www.quadibloc.com/comp/scan.htm
 */
static const char *us_keymap[][2] = {
	{"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"},       // 0-3
	{"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"},                 // 4-7
	{"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},                 // 8-11
	{"-", "_"}, {"=", "+"}, {"_BACKSPACE_", "_BACKSPACE_"},         // 12-14
	{"_TAB_", "_TAB_"}, {"q", "Q"}, {"w", "W"}, {"e", "E"}, {"r", "R"},
	{"t", "T"}, {"y", "Y"}, {"u", "U"}, {"i", "I"},                 // 20-23
	{"o", "O"}, {"p", "P"}, {"[", "{"}, {"]", "}"},                 // 24-27
	{"\n", "\n"}, {"_LCTRL_", "_LCTRL_"}, {"a", "A"}, {"s", "S"},   // 28-31
	{"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"},                 // 32-35
	{"j", "J"}, {"k", "K"}, {"l", "L"}, {";", ":"},                 // 36-39
	{"'", "\""}, {"`", "~"}, {"_LSHIFT_", "_LSHIFT_"}, {"\\", "|"}, // 40-43
	{"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"},                 // 44-47
	{"b", "B"}, {"n", "N"}, {"m", "M"}, {",", "<"},                 // 48-51
	{".", ">"}, {"/", "?"}, {"_RSHIFT_", "_RSHIFT_"}, {"_PRTSCR_", "_KPD*_"},
	{"_LALT_", "_LALT_"}, {" ", " "}, {"_CAPS_", "_CAPS_"}, {"F1", "F1"},
	{"F2", "F2"}, {"F3", "F3"}, {"F4", "F4"}, {"F5", "F5"},         // 60-63
	{"F6", "F6"}, {"F7", "F7"}, {"F8", "F8"}, {"F9", "F9"},         // 64-67
	{"F10", "F10"}, {"_NUM_", "_NUM_"}, {"_SCROLL_", "_SCROLL_"},   // 68-70
	{"_KPD7_", "_HOME_"}, {"_KPD8_", "_UP_"}, {"_KPD9_", "_PGUP_"}, // 71-73
	{"-", "-"}, {"_KPD4_", "_LEFT_"}, {"_KPD5_", "_KPD5_"},         // 74-76
	{"_KPD6_", "_RIGHT_"}, {"+", "+"}, {"_KPD1_", "_END_"},         // 77-79
	{"_KPD2_", "_DOWN_"}, {"_KPD3_", "_PGDN"}, {"_KPD0_", "_INS_"}, // 80-82
	{"_KPD._", "_DEL_"}, {"_SYSRQ_", "_SYSRQ_"}, {"\0", "\0"},      // 83-85
	{"\0", "\0"}, {"F11", "F11"}, {"F12", "F12"}, {"\0", "\0"},     // 86-89
	{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},
	{"\0", "\0"}, {"_KPENTER_", "_KPENTER_"}, {"_RCTRL_", "_RCTRL_"}, {"/", "/"},
	{"_PRTSCR_", "_PRTSCR_"}, {"_RALT_", "_RALT_"}, {"\0", "\0"},   // 99-101
	{"_HOME_", "_HOME_"}, {"_UP_", "_UP_"}, {"_PGUP_", "_PGUP_"},   // 102-104
	{"_LEFT_", "_LEFT_"}, {"_RIGHT_", "_RIGHT_"}, {"_END_", "_END_"},
	{"_DOWN_", "_DOWN_"}, {"_PGDN", "_PGDN"}, {"_INS_", "_INS_"},   // 108-110
	{"_DEL_", "_DEL_"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},   // 111-114
	{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},         // 115-118
	{"_PAUSE_", "_PAUSE_"},                                         // 119
};

#define BUF_SIZE 10
static char terrible_keys_buf[BUF_SIZE+1] = {0};
static char terrible_keys_buf_reversed[BUF_SIZE+1] = {0};

static int terrible_kb_cb(struct notifier_block *nblock,
		unsigned long code,
		void *_param);

static struct notifier_block terrible_kbdb = {
	.notifier_call = terrible_kb_cb,
};

static void (*on_buffer_update)(char*) = 0;

int terrible_init_keyboard(void (*update)(char*)) {
  on_buffer_update = update;
  return register_keyboard_notifier(&terrible_kbdb);
}

int terrible_uninit_keyboard() {
  on_buffer_update = 0;
  return unregister_keyboard_notifier(&terrible_kbdb);
}

static char keycode_to_char(int keycode, int shift_mask)
{
	if (keycode > KEY_RESERVED && keycode <= KEY_PAUSE) {
		const char *us_key = (shift_mask == 1)
		? us_keymap[keycode][1]
		: us_keymap[keycode][0];

		if (strlen(us_key) != 1) return 0;
		else return us_key[0];
	}
	else return 0;
}

static int terrible_kb_cb(struct notifier_block *nblock,
		  unsigned long code,
		  void *_param)
{
	struct keyboard_notifier_param *param = _param;

	pr_debug("code: 0x%lx, down: 0x%x, shift: 0x%x, value: 0x%x\n",
		 code, param->down, param->shift, param->value);

	/* Trace only when a key is pressed down */
	if (!(param->down))
		return NOTIFY_OK;

	/* Convert keycode to readable string in keybuf */
	char c = keycode_to_char(param->value, param->shift);
	if (c < ' ' || c > '~')
		return NOTIFY_OK;

	pr_debug("Terrible Key Press: %c\n", c);
	
	for (size_t i=1; i<BUF_SIZE; ++i) 
	  terrible_keys_buf[BUF_SIZE-i] = terrible_keys_buf[BUF_SIZE-i-1];
	terrible_keys_buf[0] = c;
	terrible_keys_buf[BUF_SIZE] = 0;
	
	pr_debug("Terrible Buffer: %s\n", terrible_keys_buf);
	
	for (size_t i = 0; i<BUF_SIZE; ++i)
	  terrible_keys_buf_reversed[BUF_SIZE-i-1] = terrible_keys_buf[i];

	size_t beginning = 0;
	while (beginning < BUF_SIZE && *(terrible_keys_buf_reversed + beginning) == 0)
	  ++beginning;
	
	if (on_buffer_update) on_buffer_update(terrible_keys_buf_reversed + beginning);

	return NOTIFY_OK;
}
