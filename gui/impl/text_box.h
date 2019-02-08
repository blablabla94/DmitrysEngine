/* Copyright (c) 2017-2019 Dmitry Stepanov a.k.a mr.DIMAS
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

static void de_gui_text_box_deinit(de_gui_node_t* n) {
	de_gui_text_box_t* tb;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	tb = &n->s.text_box;
	DE_ARRAY_FREE(tb->str);
	DE_ARRAY_FREE(tb->lines);
}

static void de_gui_text_box_reset_caret_blink(de_gui_text_box_t* tb) {
	tb->caret_visible = true;
	tb->blink_timer = 0;
}

static void de_gui_text_box_lost_focus(de_gui_node_t* n, de_gui_routed_event_args_t* args) {
	de_gui_text_box_t* tb;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	DE_UNUSED(args);
	tb = &n->s.text_box;
	tb->show_caret = false;
}

static void de_gui_text_box_got_focus(de_gui_node_t* n, de_gui_routed_event_args_t* args) {
	de_gui_text_box_t* tb;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	DE_UNUSED(args);
	tb = &n->s.text_box;
	tb->show_caret = true;
	de_gui_text_box_reset_caret_blink(tb);
}

static void de_gui_text_box_move_caret_y(de_gui_text_box_t* tb, int dir) {
	if (dir > 0) {
		if (tb->caret_line < (int)tb->lines.size - 1) {
			++tb->caret_line;
			de_gui_text_box_reset_caret_blink(tb);
		}
	} else if (dir < 0) {
		if (tb->caret_line > 0) {
			--tb->caret_line;
			de_gui_text_box_reset_caret_blink(tb);
		}
	}
}

static void de_gui_text_box_move_caret_x(de_gui_text_box_t* tb, int amount) {
	de_gui_text_line_t* line, *prev_line;
	line = tb->lines.size ? tb->lines.data + tb->caret_line : NULL;
	if (line) {
		if (amount < 0) {
			tb->caret_offset += amount;
			if (tb->caret_offset < 0) {
				int prev_index = tb->caret_line - 1;
				prev_line = prev_index >= 0 ? tb->lines.data + prev_index : NULL;
				if (prev_line) {
					tb->caret_offset = prev_line->end - prev_line->begin - (tb->caret_offset - amount);
					--tb->caret_line;
				} else {
					tb->caret_offset = 0;
				}
			}
			de_gui_text_box_reset_caret_blink(tb);
		} else if (amount > 0) {
			int len;
			tb->caret_offset += amount;
			len = (int)line->end - (int)line->begin;
			if (tb->caret_offset > len) {
				if (tb->caret_line + 1 < (int)tb->lines.size) {
					tb->caret_offset = 1;
					++tb->caret_line;
				} else {
					tb->caret_offset = len;
				}
			}
			de_gui_text_box_reset_caret_blink(tb);
		}
	}
}

static int de_gui_text_box_caret_to_index(de_gui_text_box_t* tb) {
	/* any out-of-bounds result are valid here since we are inserting characters via oob-protected func */
	if (tb->lines.size && tb->caret_line < (int)tb->lines.size) {
		de_gui_text_line_t* line = tb->lines.data + tb->caret_line;
		return line->begin + tb->caret_offset;
	}
	return 0;
}

static void de_gui_text_box_key_down(de_gui_node_t* n, de_gui_routed_event_args_t* args) {
	de_gui_text_box_t* tb;
	int index;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	tb = &n->s.text_box;
	switch (args->s.key_down.key) {
		case DE_KEY_LEFT:
			de_gui_text_box_move_caret_x(tb, -1);
			break;
		case DE_KEY_RIGHT:
			de_gui_text_box_move_caret_x(tb, +1);
			break;
		case DE_KEY_UP:
			de_gui_text_box_move_caret_y(tb, -1);
			break;
		case DE_KEY_DOWN:
			de_gui_text_box_move_caret_y(tb, +1);
			break;
		case DE_KEY_BACKSPACE:
			index = de_gui_text_box_caret_to_index(tb) - 1;
			if (index >= 0) {
				de_str32_remove(&tb->str, index, 1);
				de_gui_text_box_move_caret_x(tb, -1);
			}
			break;
		case DE_KEY_DELETE:
			index = de_gui_text_box_caret_to_index(tb);
			if (index >= 0) {
				de_str32_remove(&tb->str, index, 1);
			}
			break;
		default:
			break;
	}
}

static void de_gui_text_box_update(de_gui_node_t* node) {
	de_gui_text_box_t* tb;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEXT_BOX);
	tb = &node->s.text_box;
	++tb->blink_timer;
	if (tb->blink_timer >= tb->blink_interval) {
		tb->blink_timer = 0;
		/* blink */
		tb->caret_visible = !tb->caret_visible;
	}
}

static void de_gui_text_box_break_on_lines(de_gui_node_t* node) {
	size_t i;
	float max_width = 0;
	de_gui_text_box_t* tb;
	de_gui_text_line_t line = { 0 };
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEXT_BOX);
	tb = &node->s.text_box;
	if (!tb->font) {
		return;
	}
	tb->total_lines_height = 0;
	DE_ARRAY_CLEAR(tb->lines);
	for (i = 0; i < tb->str.size; ++i) {
		float new_width;
		bool control_char;
		uint32_t code = tb->str.data[i];
		control_char = code == '\n' || code == '\r';
		de_glyph_t* glyph = de_font_get_glyph(tb->font, code);
		if (!glyph) {
			continue;
		}
		new_width = line.width + glyph->advance;
		if (new_width > node->actual_size.x || control_char) {
			/* commit line */
			DE_ARRAY_APPEND(tb->lines, line);
			if (line.width > max_width) {
				max_width = line.width;
			}
			/* start new line */
			line.begin = control_char ? i + 1 : i;
			line.end = line.begin + 1;
			line.width = glyph->advance;
			tb->total_lines_height += tb->font->ascender;
		} else {
			line.width = new_width;
			++line.end;
		}
	}
	/* commit rest of the text */
	if (line.begin != line.end) {
		line.end = tb->str.size;
		DE_ARRAY_APPEND(tb->lines, line);
		tb->total_lines_height += tb->font->ascender;
	}
}

static bool de_gui_text_box_text_entered(de_gui_node_t* n, const de_gui_routed_event_args_t* args) {
	de_gui_text_box_t* tb;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	tb = &n->s.text_box;
	switch (args->type) {
		case DE_GUI_ROUTED_EVENT_TEXT:
			if (iscntrl(args->s.text.unicode)) {
				break;
			}
			de_str32_insert(&tb->str, de_gui_text_box_caret_to_index(tb), args->s.text.unicode);
			de_gui_text_box_break_on_lines(n);
			de_gui_text_box_move_caret_x(tb, 1);
			return true;
		default:
			break;
	}
	return false;
}

static void de_gui_text_box_render(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting) {
	size_t i;
	const de_vec2_t* scr_pos = &n->screen_position;
	const de_gui_text_box_t* tb = &n->s.text_box;
	de_color_t c = { 120, 120, 120, 255 };
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	DE_UNUSED(nesting);
	de_gui_draw_list_push_rect_filled(dl, scr_pos, &n->actual_size, &c, NULL);
	de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, 0, n);
	if (tb->font) {
		de_vec2_t pos = n->screen_position;
		de_gui_text_box_break_on_lines(n);
		switch (tb->ver_alignment) {
			case DE_GUI_VERTICAL_ALIGNMENT_TOP:
				/* already at top */
				break;
			case DE_GUI_VERTICAL_ALIGNMENT_BOTTOM:
				pos.y = scr_pos->y + n->actual_size.y - tb->total_lines_height;
				break;
			case DE_GUI_VERTICAL_ALIGNMENT_CENTER:
				pos.y = scr_pos->y + n->actual_size.y * 0.5f - tb->total_lines_height * 0.5f;
				break;
			case DE_GUI_VERTICAL_ALIGNMENT_STRETCH:
				/* todo */
				break;
		}
		for (i = 0; i < tb->lines.size; ++i) {
			de_gui_text_line_t* line = tb->lines.data + i;
			uint32_t* str = tb->str.data + line->begin;
			size_t len = line->end - line->begin;
			switch (tb->hor_alignment) {
				case DE_GUI_HORIZONTAL_ALIGNMENT_LEFT:
					pos.x = scr_pos->x;
					break;
				case DE_GUI_HORIZONTAL_ALIGNMENT_CENTER:
					pos.x = scr_pos->x + n->actual_size.x * 0.5f - line->width * 0.5f;
					break;
				case DE_GUI_HORIZONTAL_ALIGNMENT_RIGHT:
					pos.x = scr_pos->x + n->actual_size.x - line->width;
					break;
				case DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH:
					/* todo */
					break;
			}
			de_gui_draw_list_push_text(dl, tb->font, str, len, &pos, &n->color);
			pos.y += tb->font->ascender;
		}
		de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, tb->font->texture->id, n);
		/* caret */
		if (tb->show_caret && tb->caret_visible) {
			de_vec2_t cpos, csize;
			int k;
			de_gui_text_line_t* line;
			if (tb->lines.size) {
				line = tb->lines.data + tb->caret_line;
				cpos.x = 0;
				for (i = line->begin, k = 0; i < line->end && k < tb->caret_offset; ++i, ++k) {
					de_glyph_t* glyph = de_font_get_glyph(tb->font, tb->str.data[i]);
					if (glyph) {
						cpos.x += glyph->advance;
					}
				}
				cpos.x += scr_pos->x;
			} else {
				cpos.x = scr_pos->x;
			}
			cpos.y = scr_pos->y + tb->caret_line * tb->font->ascender;
			csize.x = 2;
			csize.y = tb->font->ascender;
			de_gui_draw_list_push_rect_filled(dl, &cpos, &csize, &n->color, NULL);
		}
	}
}

de_gui_dispatch_table_t* de_gui_text_box_get_dispatch_table(void) {
	static de_gui_dispatch_table_t dispatch_table;
	static bool init = false;
	if (!init) {
		dispatch_table.deinit = de_gui_text_box_deinit;
		dispatch_table.render = de_gui_text_box_render;
		dispatch_table.update = de_gui_text_box_update;
		init = true;
	}
	return &dispatch_table;
}

de_gui_node_t* de_gui_text_box_create(de_gui_t* gui) {
	de_gui_node_t* n;
	de_gui_text_box_t* tb;
	n = de_gui_node_alloc(gui, DE_GUI_NODE_TEXT_BOX, de_gui_text_box_get_dispatch_table());
	n->text_entered = de_gui_text_box_text_entered;
	n->got_focus = de_gui_text_box_got_focus;
	n->lost_focus = de_gui_text_box_lost_focus;
	n->key_down = de_gui_text_box_key_down;
	de_color_set(&n->color, 255, 255, 255, 255);
	tb = &n->s.text_box;
	tb->font = gui->default_font;
	tb->blink_interval = 35;
	tb->blink_timer = 0;
	DE_ARRAY_INIT(tb->str);
	return n;
}