static void de_gui_text_box_deinit(de_gui_node_t* n)
{
	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);

	DE_UNUSED(n);
}

/*=======================================================================================*/
de_gui_node_t* de_gui_text_box_create(de_gui_t* gui)
{
	de_gui_node_t* n;
	de_gui_text_box_t* tb;

	static de_gui_dispatch_table_t dispatch_table;
	{
		static de_bool_t init = DE_FALSE;
		if (!init)
		{
			dispatch_table.deinit = de_gui_text_box_deinit;
			init = DE_TRUE;
		}
	}

	n = de_gui_node_alloc(gui, DE_GUI_NODE_TEXT_BOX, &dispatch_table);

	tb = &n->s.text_box;

	tb->border = de_gui_border_create(gui);
	de_gui_node_attach(tb->border, n);

	tb->text_block = de_gui_text_create(gui);
	de_gui_node_attach(tb->text_block, tb->border);

	DE_ARRAY_INIT(tb->text);

	return n;
}
