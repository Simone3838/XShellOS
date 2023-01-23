/*****************************************
		NanoShell Operating System
	      (C) 2022 iProgramInCpp

            Icon listing module
******************************************/
#include <icon.h>
#include <misc.h>

#define C_TRANSPAR TRANSPARENT

typedef uint32_t IconColor;

#define COLR_WHITE 0x00FFFFFF
#define COLR_LGRAY 0x00C0C0C0

#define COLR_DGRAY 0x00808080
#define COLR_BLACK 0x00000000

#define COLOR__RED 0x00FF0000
#define COLOR_DRED 0x00800000

#define COLR_GREEN 0x0000FF00
#define CLR_DGREEN 0x00008000

#define COLOR_BLUE 0x000000FF
#define COLR_DBLUE 0x00000080

#define COLOR_CYAN 0x0000FFFF
#define COLOR_TEAL 0x00008080

#define CLR_YELLOW 0x00FFFF00
#define CL_DYELLOW 0x00808000

#define CL_MAGENTA 0x00FF00FF
#define C_DMAGENTA 0x00800080

//WORK: Add in your icons here:
#include <icons/cabinet.h>
#include <icons/chip.h>
#include <icons/chip_sq.h>
#include <icons/computer.h>
#include <icons/computer_shutdown.h>
#include <icons/desktop.h>
#include <icons/draw.h>
#include <icons/earth.h>
#include <icons/error.h>
#include <icons/execute_file.h>
#include <icons/file.h>
#include <icons/files.h>
#include <icons/folder.h>
#include <icons/folder_blank.h>
#include <icons/folder_move.h>
#include <icons/folder_parent.h>
#include <icons/folder16_closed.h>
#include <icons/folder16_open.h>
#include <icons/globe.h>
#include <icons/go.h>
#include <icons/hand.h>
#include <icons/help.h>
#include <icons/info.h>
#include <icons/keyboard.h>
#include <icons/keyboard2.h>
#include <icons/laptop.h>
#include <icons/notes.h>
#include <icons/paint.h>
#include <icons/serial.h>
#include <icons/stop.h>
#include <icons/text_file.h>
#include <icons/warning.h>
#include <icons/nanoshell.h>
#include <icons/nanoshell16.h>
#include <icons/nanoshell_shell.h>
#include <icons/bomb.h>
#include <icons/bomb2.h>
#include <icons/nanoshell_shell16.h>
#include <icons/file16.h>
#include <icons/text_file16.h>
#include <icons/execute_file16.h>
#include <icons/folder_parent16.h>
// Icons V1.1
#include <icons/folder_settings.h>
#include <icons/cabinet16.h>
#include <icons/computer16.h>
#include <icons/command.h>
#include <icons/command16.h>
#include <icons/error16.h>
// Icons V1.2
#include <icons/lock.h>
#include <icons/directions.h>
#include <icons/certificate.h>
#include <icons/file_write.h>
#include <icons/scrap_file.h>
#include <icons/scrap_file16.h>
#include <icons/resmon.h>
#include <icons/billboard.h>
#include <icons/cscript_file.h>
#include <icons/cscript_file16.h>
#include <icons/file_click.h>
#include <icons/keys.h>
#include <icons/restricted.h>
#include <icons/home.h>
#include <icons/home16.h>
#include <icons/adapter.h>
#include <icons/clock.h>
#include <icons/clock16.h>
// Icons V1.3
#include <icons/application.h>
#include <icons/application16.h>
#include <icons/taskbar.h>
#include <icons/app_demo.h>
#include <icons/computer_flat.h>
#include <icons/calculator.h>
#include <icons/calculator16.h>
#include <icons/desktop2.h>
#include <icons/mouse.h>
// Icons V1.31
#include <icons/ambulance.h>
// Icons V1.32
#include <icons/fonts.h>
#include <icons/fonts16.h>
// Icons V1.33
#include <icons/resmon16.h>
#include <icons/notes16.h>
#include <icons/file_nano.h>
// Icons V1.34
#include <icons/clock_empty.h>
// Icons V1.35
#include <icons/run.h>
#include <icons/run16.h>
// Icons V1.4
#include <icons/cabinet_new.h>
#include <icons/devtool.h>
#include <icons/devtool_file.h>
#include <icons/hex_edit.h>
#include <icons/chain.h>
#include <icons/chain16.h>
#include <icons/devtool16.h>
#include <icons/todo.h>
#include <icons/folder_docu.h>
#include <icons/dlgedit.h>
#include <icons/desk_sett.h>
#include <icons/shutdn.h>
#include <icons/notepad.h>
#include <icons/file_mkdown.h>
#include <icons/file_mkdown16.h>
#include <icons/computer_panic.h>//actually 96x32
#include <icons/experiment.h>
#include <icons/graph.h>
#include <icons/cabinet_combine.h>//for the mount GUI
#include <icons/remote.h>
// Icons V1.5
#include <icons/device_char.h>
#include <icons/device_block.h>
#include <icons/hard_drive.h>
#include <icons/hard_drive_mount.h>
#include <icons/window.h>
#include <icons/window_snap.h>
#include <icons/windows_overlapped.h>
#include <icons/sweep_smiley.h>
#include <icons/sweep_smiley_click.h>
#include <icons/sweep_smiley_dead.h>
#include <icons/sweep_smiley_caret.h>
#include <icons/dlgedit16.h>
#include <icons/bomb16.h>
#include <icons/magnify.h>
#include <icons/magnify16.h>
#include <icons/tar_archive.h>
#include <icons/sysmon.h>
#include <icons/sysmon16.h>
#include <icons/computer_shutdown16.h>
#include <icons/exit.h>
#include <icons/key.h>
#include <icons/keybrepspeed.h>
#include <icons/keybrepdelay.h>
#include <icons/monitor.h>
//icons V1.6
#include <icons/ns_ini.h>
#include <icons/wmenu.h>
#include <icons/wmenu16.h>
#include <icons/file_image.h>
#include <icons/file_image16.h>
#include <icons/file_log.h>
#include <icons/sticky_notes.h>
#include <icons/sticky_notes16.h>
#include <icons/note_yellow.h>
#include <icons/note_blue.h>
#include <icons/note_green.h>
#include <icons/note_white.h>
#include <icons/folder_open.h>
//icons V1.61
#include <icons/experiment2.h>
#include <icons/floppy.h>
#include <icons/action_save16.h>
#include <icons/action_open.h>
#include <icons/action_open16.h>
#include <icons/plus.h>
//icons V1.7
#include <icons/paste.h>
#include <icons/paste16.h>
#include <icons/delete.h>
#include <icons/delete16.h>
#include <icons/copy.h>
#include <icons/copy16.h>
#include <icons/back.h>
#include <icons/back16.h>
#include <icons/forward.h>
#include <icons/forward16.h>
#include <icons/undo.h>
#include <icons/undo16.h>
#include <icons/redo.h>
#include <icons/redo16.h>
#include <icons/file_search.h>
#include <icons/file_search16.h>
#include <icons/file_properties.h>
#include <icons/file_properties16.h>
#include <icons/properties.h>
#include <icons/properties16.h>
#include <icons/whats_this.h>
#include <icons/whats_this16.h>
#include <icons/view_icon.h>
#include <icons/view_icon16.h>
#include <icons/view_list.h>
#include <icons/view_list16.h>
#include <icons/view_table.h>
#include <icons/view_table16.h>
#include <icons/sort_alpha.h>
#include <icons/sort_alpha16.h>
#include <icons/form.h>
#include <icons/form16.h>
#include <icons/journal.h>
#include <icons/journal16.h>
#include <icons/packager.h>
#include <icons/packager16.h>
//icons V1.71
#include <icons/box_check.h>
#include <icons/box_uncheck.h>
#include <icons/folder_settings16.h>
//icons V1.8
#include <icons/minimize.h>
#include <icons/maximize.h>
#include <icons/restore.h>
#include <icons/close.h>
#include <icons/arrow_up.h>
#include <icons/arrow_down.h>
#include <icons/arrow_left.h>
#include <icons/arrow_right.h>
#include <icons/pipe.h>
#include <icons/pipe16.h>
#include <icons/vb_cursor.h>
#include <icons/vb_select.h>
#include <icons/vb_text.h>
#include <icons/vb_text_cen.h>
#include <icons/vb_input_1line.h>
#include <icons/vb_input_mline.h>
#include <icons/vb_checkbox.h>
#include <icons/vb_surr_rect.h>
#include <icons/vb_button.h>
#include <icons/clipboard.h>
#include <icons/paint2.h>
#include <icons/file_broken.h>

Image * g_iconTable[] = {
	NULL,
	&g_cabinet_new_icon,
	&g_chip_icon,
	&g_chip_sq_icon,
	&g_computer_icon,
	&g_computer_shutdown_icon,
	&g_desktop_icon,
	&g_draw_icon,
	&g_earth_icon,
	&g_error_icon,
	&g_execute_file_icon,
	&g_file_icon,
	&g_files_icon,
	&g_folder_icon,
	&g_folder_blank_icon,
	&g_folder_move_icon,
	&g_folder_parent_icon,
	&g_folder16_closed_icon,
	&g_folder16_open_icon,
	&g_globe_icon,
	&g_go_icon,
	&g_hand_icon,
	&g_help_icon,
	&g_info_icon,
	&g_keyboard_icon,
	&g_keyboard2_icon,
	&g_laptop_icon,
	&g_notes_icon,
	&g_paint_icon,
	&g_serial_icon,
	&g_stop_icon,
	&g_text_file_icon,
	&g_warning_icon,
	&g_nanoshell_icon,
	&g_nanoshell16_icon,
	&g_nanoshell_shell_icon,
	&g_nanoshell_shell16_icon,
	&g_bomb_icon,
	&g_bomb2_icon,
	&g_file16_icon,
	&g_text_file16_icon,
	&g_execute_file16_icon,
	&g_folder_parent16_icon,
	&g_folder_settings_icon,
	&g_cabinet16_icon,
	&g_computer16_icon,
	&g_command_icon,
	&g_command16_icon,
	&g_error16_icon,
	&g_lock_icon,
	&g_directions_icon,
	&g_certificate_icon,
	&g_file_write_icon,
	&g_scrap_file_icon,
	&g_scrap_file16_icon,
	&g_resmon_icon,
	&g_billboard_icon,
	&g_cscript_file_icon,
	&g_cscript_file16_icon,
	&g_file_click_icon,
	&g_keys_icon,
	&g_restricted_icon,
	&g_home_icon,
	&g_home16_icon,
	&g_adapter_icon,
	&g_clock_icon,
	&g_clock16_icon,
	&g_application_icon,
	&g_application16_icon,
	&g_taskbar_icon,
	&g_app_demo_icon,
	&g_computer_flat_icon,
	&g_calculator_icon,
	&g_calculator16_icon,
	&g_desktop2_icon,
	&g_mouse_icon,
	&g_ambulance_icon,
	&g_fonts_icon,
	&g_fonts16_icon,
	&g_resmon16_icon,
	&g_notes16_icon,
	&g_file_nano_icon,
	&g_clock_empty_icon,
	&g_run_icon,
	&g_run16_icon,
	&g_devtool_icon,
	&g_devtool_file_icon,
	&g_hex_edit_icon,
	&g_chain_icon,
	&g_chain16_icon,
	&g_devtool16_icon,
	&g_todo_icon,
	&g_folder_docu_icon,
	&g_dlgedit_icon,
	&g_desk_sett_icon,
	&g_shutdn_icon,
	&g_notepad_icon,
	&g_file_mkdown_icon,
	&g_file_mkdown16_icon,
	&g_computer_panic_icon,//actually 96x32
	&g_experiment_icon,
	&g_graph_icon,
	&g_cabinet_combine_icon,//for the mount GUI
	&g_remote_icon,
	&g_cabinet_icon,
	&g_device_char_icon,
	&g_device_block_icon,
	&g_hard_drive_icon,
	&g_hard_drive_mount_icon,
	&g_window_icon,
	&g_window_snap_icon,
	&g_windows_overlapped_icon,
	&g_sweep_smiley_icon,        // all the sweep-smiley icons are 17x17
	&g_sweep_smiley_click_icon,
	&g_sweep_smiley_dead_icon,
	&g_sweep_smiley_caret_icon,
	&g_dlgedit16_icon,
	&g_bomb16_icon,
	&g_magnify_icon,
	&g_magnify16_icon,
	&g_tar_archive_icon,
	&g_sysmon_icon,
	&g_sysmon16_icon,
	&g_computer_shutdown16_icon,
	&g_exit_icon,
	&g_key_icon,
	&g_keybrepspeed_icon,
	&g_keybrepdelay_icon,
	&g_monitor_icon,
	&g_ns_ini_icon,
	&g_wmenu_icon,
	&g_wmenu16_icon,
	&g_file_image_icon,
	&g_file_image16_icon,
	&g_file_log_icon,
	&g_sticky_notes_icon,
	&g_sticky_notes16_icon,
	&g_note_yellow_icon,
	&g_note_blue_icon,
	&g_note_green_icon,
	&g_note_white_icon,
	&g_folder_open_icon,
	&g_experiment2_icon,
	&g_floppy_icon,
	&g_action_save16_icon,
	&g_action_open_icon,
	&g_action_open16_icon,
	&g_plus_icon,
	&g_paste_icon,
	&g_paste16_icon,
	&g_delete_icon,
	&g_delete16_icon,
	&g_copy_icon,
	&g_copy16_icon,
	&g_back_icon,
	&g_back16_icon,
	&g_forward_icon,
	&g_forward16_icon,
	&g_undo_icon,
	&g_undo16_icon,
	&g_redo_icon,
	&g_redo16_icon,
	&g_file_search_icon,
	&g_file_search16_icon,
	&g_file_properties_icon,
	&g_file_properties16_icon,
	&g_properties_icon,
	&g_properties16_icon,
	&g_whats_this_icon,
	&g_whats_this16_icon,
	&g_view_icon_icon,
	&g_view_icon16_icon,
	&g_view_list_icon,
	&g_view_list16_icon,
	&g_view_table_icon,
	&g_view_table16_icon,
	&g_sort_alpha_icon,
	&g_sort_alpha16_icon,
	&g_form_icon,
	&g_form16_icon,
	&g_journal_icon,
	&g_journal16_icon,
	&g_packager_icon,
	&g_packager16_icon,
	&g_box_check_icon,
	&g_box_uncheck_icon,
	&g_folder_settings16_icon,
	&g_minimize_icon,
	&g_maximize_icon,
	&g_restore_icon,
	&g_close_icon,
	&g_arrow_up_icon,
	&g_arrow_down_icon,
	&g_arrow_left_icon,
	&g_arrow_right_icon,
	&g_pipe_icon,
	&g_pipe16_icon,
	&g_vb_cursor_icon,
	&g_vb_select_icon,
	&g_vb_text_icon,
	&g_vb_text_cen_icon,
	&g_vb_input_1line_icon,
	&g_vb_input_mline_icon,
	&g_vb_checkbox_icon,
	&g_vb_surr_rect_icon,
	&g_vb_button_icon,
	&g_clipboard_icon,
	&g_paint2_icon,
	&g_file_broken_icon,
};

STATIC_ASSERT(ARRAY_COUNT(g_iconTable) == ICON_COUNT, "Change this array if adding icons.");

Image* GetIconImage(IconType type, int sz)
{
	if (type >= ICON_COUNT || type <= ICON_NULL) return NULL;
	
	if (sz == 16)
	{
		// Convert certain icons to their 16x counterparts:
		switch (type)
		{
			#define CASE(typo) case ICON_ ## typo: type = ICON_ ## typo ## 16; break;
			CASE(CABINET)
			CASE(COMPUTER)
			CASE(ERROR)
			case ICON_FOLDER:      type = ICON_FOLDER16_CLOSED; break;
			case ICON_CLOCK_EMPTY: type = ICON_CLOCK16;         break;
			CASE(FOLDER_PARENT)
			CASE(EXECUTE_FILE)
			CASE(FILE)
			CASE(TEXT_FILE)
			CASE(NANOSHELL)
			CASE(NANOSHELL_LETTERS)
			CASE(COMMAND)
			CASE(SCRAP_FILE)
			CASE(FILE_CSCRIPT)
			CASE(HOME)
			CASE(CLOCK)
			CASE(APPLICATION)
			CASE(CALCULATOR)
			CASE(FONTS)
			CASE(RESMON)
			CASE(NOTES)
			CASE(RUN)
			CASE(CHAIN)
			CASE(SYSMON)
			CASE(FILE_MKDOWN)
			CASE(DEVTOOL)
			CASE(DLGEDIT)
			CASE(MAGNIFY)
			CASE(BOMB_SPIKEY)
			CASE(COMPUTER_SHUTDOWN)
			CASE(ACTION_SAVE)
			CASE(ACTION_OPEN)
			CASE(PASTE)
			CASE(DELETE)
			CASE(COPY)
			CASE(BACK)
			CASE(FORWARD)
			CASE(UNDO)
			CASE(REDO)
			CASE(FILE_SEARCH)
			CASE(FILE_PROPERTIES)
			CASE(PROPERTIES)
			CASE(WHATS_THIS)
			CASE(VIEW_ICON)
			CASE(VIEW_LIST)
			CASE(VIEW_TABLE)
			CASE(SORT_ALPHA)
			CASE(FORM)
			CASE(JOURNAL)
			CASE(PACKAGER)
			CASE(FOLDER_SETTINGS)
			CASE(PIPE)
			#undef CASE
		}
	}
	
	return g_iconTable[type];
}
void RenderIcon(IconType type, int x, int y)
{
	if (type >= ICON_COUNT || type <= ICON_NULL) return;
	
	Image* p = g_iconTable[type];
	VidBlitImage(p, x, y);
}
void RenderIconOutline(IconType type, int x, int y, uint32_t color)
{
	if (type >= ICON_COUNT || type <= ICON_NULL) return;
	
	Image* p = g_iconTable[type];
	VidBlitImageOutline(p, x, y, color);
}
void RenderThumbClock(int x, int y, int size);
void RenderIconForceSize(IconType type, int x, int y, int size)
{
	Image *p = GetIconImage(type, size);
	if (p)
	{
		VidBlitImageResize(p, x, y, size, size);
		if (type == ICON_CLOCK_EMPTY)
		{
			RenderThumbClock(x, y, size);
		}
	}
}
void RenderIconForceSizeOutline(IconType type, int x, int y, int size, uint32_t color)
{
	Image *p = GetIconImage(type, size);
	if (p)
	{
		VidBlitImageResizeOutline(p, x, y, size, size, color);
	}
}
