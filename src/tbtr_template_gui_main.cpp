/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

 /** @file tbtr_template_gui_main.cpp Main window for template configuration and overview. */

#include "stdafx.h"

#include "core/geometry_func.hpp"
#include "group.h"
#include "rail_gui.h"
#include "tilehighlight_func.h"
#include "vehicle_gui_base.h"

#include "tbtr_template_gui_create.h"
#include "tbtr_template_gui_main.h"

typedef GUIList<const Group*> GUIGroupList;

enum TemplateReplaceWindowWidgets {
	TRW_CAPTION,

	TRW_WIDGET_INSET_GROUPS,
	TRW_WIDGET_TOP_MATRIX,
	TRW_WIDGET_TOP_SCROLLBAR,

	TRW_WIDGET_INSET_TEMPLATES,
	TRW_WIDGET_BOTTOM_MATRIX,
	TRW_WIDGET_BOTTOM_SCROLLBAR,

	TRW_WIDGET_TMPL_INFO_INSET,
	TRW_WIDGET_TMPL_INFO_PANEL,

	TRW_WIDGET_TMPL_PRE_BUTTON_FLUFF,

	TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REUSE,
	TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_KEEP,
	TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REFIT,
	TRW_WIDGET_TMPL_BUTTONS_CONFIG_RIGHTPANEL,

	TRW_WIDGET_TMPL_BUTTONS_DEFINE,
	TRW_WIDGET_TMPL_BUTTONS_EDIT,
	TRW_WIDGET_TMPL_BUTTONS_CLONE,
	TRW_WIDGET_TMPL_BUTTONS_DELETE,
	TRW_WIDGET_TMPL_BUTTONS_RPLALL,
	TRW_WIDGET_TMPL_BUTTON_FLUFF,
	TRW_WIDGET_TMPL_BUTTONS_EDIT_RIGHTPANEL,

	TRW_WIDGET_TITLE_INFO_GROUP,
	TRW_WIDGET_TITLE_INFO_TEMPLATE,

	TRW_WIDGET_INFO_GROUP,
 	TRW_WIDGET_INFO_TEMPLATE,

	TRW_WIDGET_TMPL_BUTTONS_SPACER,

	TRW_WIDGET_START,
	TRW_WIDGET_TRAIN_FLUFF_LEFT,
	TRW_WIDGET_TRAIN_RAILTYPE_DROPDOWN,
	TRW_WIDGET_TRAIN_FLUFF_RIGHT,
	TRW_WIDGET_STOP,

	TRW_WIDGET_SEL_TMPL_DISPLAY_CREATE,
};

static const NWidgetPart _widgets[] = {
	// Title bar
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_CLOSEBOX, COLOUR_GREY),
		NWidget(WWT_CAPTION, COLOUR_GREY, TRW_CAPTION), SetDataTip(STR_TMPL_RPL_TITLE, STR_TOOLTIP_WINDOW_TITLE_DRAG_THIS),
		NWidget(WWT_SHADEBOX, COLOUR_GREY),
		NWidget(WWT_STICKYBOX, COLOUR_GREY),
	EndContainer(),
	//Top Matrix
	NWidget(NWID_VERTICAL),
		NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_INSET_GROUPS), SetMinimalSize(216,12), SetDataTip(STR_TMPL_MAINGUI_DEFINEDGROUPS, STR_TMPL_MAINGUI_DEFINEDGROUPS), SetResize(1, 0), EndContainer(),
		NWidget(NWID_HORIZONTAL),
			NWidget(WWT_MATRIX, COLOUR_GREY, TRW_WIDGET_TOP_MATRIX), SetMinimalSize(216, 0), SetFill(1, 1), SetDataTip(0x1, STR_REPLACE_HELP_LEFT_ARRAY), SetResize(1, 0), SetScrollbar(TRW_WIDGET_TOP_SCROLLBAR),
			NWidget(NWID_VSCROLLBAR, COLOUR_GREY, TRW_WIDGET_TOP_SCROLLBAR),
		EndContainer(),
	EndContainer(),
	// Template Display
	NWidget(NWID_VERTICAL),
		NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_INSET_TEMPLATES), SetMinimalSize(216,12), SetDataTip(STR_TMPL_AVAILABLE_TEMPLATES, STR_TMPL_AVAILABLE_TEMPLATES), SetResize(1, 0), EndContainer(),
		NWidget(NWID_HORIZONTAL),
			NWidget(WWT_MATRIX, COLOUR_GREY, TRW_WIDGET_BOTTOM_MATRIX), SetMinimalSize(216, 0), SetFill(1, 1), SetDataTip(0x1, STR_REPLACE_HELP_RIGHT_ARRAY), SetResize(1, 1), SetScrollbar(TRW_WIDGET_BOTTOM_SCROLLBAR),
			NWidget(NWID_VSCROLLBAR, COLOUR_GREY, TRW_WIDGET_BOTTOM_SCROLLBAR),
		EndContainer(),
	EndContainer(),
	// Info Area
	NWidget(NWID_VERTICAL),
		NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_TMPL_INFO_INSET), SetMinimalSize(216,12), SetResize(1,0), SetDataTip(STR_TMPL_AVAILABLE_TEMPLATES, STR_TMPL_AVAILABLE_TEMPLATES), EndContainer(),
		NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TMPL_INFO_PANEL), SetMinimalSize(216,50), SetResize(1,0), EndContainer(),
	EndContainer(),
	// Control Area
	NWidget(NWID_VERTICAL),
		// Spacing
		NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_TMPL_PRE_BUTTON_FLUFF), SetMinimalSize(139, 12), SetResize(1,0), EndContainer(),
		// Config buttons
		NWidget(NWID_HORIZONTAL),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REUSE), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TMPL_SET_USEDEPOT, STR_TMPL_SET_USEDEPOT_TIP),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_KEEP), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TMPL_SET_KEEPREMAINDERS, STR_TMPL_SET_KEEPREMAINDERS_TIP),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REFIT), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TMPL_SET_REFIT, STR_TMPL_SET_REFIT_TIP),
			NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIG_RIGHTPANEL), SetMinimalSize(12,12), SetResize(1,0), EndContainer(),
		EndContainer(),
		// Edit buttons
		NWidget(NWID_HORIZONTAL),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_DEFINE), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TMPL_DEFINE_TEMPLATE, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_EDIT), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TMPL_EDIT_TEMPLATE, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
			NWidget(WWT_TEXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CLONE), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TMPL_CREATE_CLONE_VEH, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_DELETE), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TMPL_DELETE_TEMPLATE, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_RPLALL), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TMPL_RPL_ALL_TMPL, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
			NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_EDIT_RIGHTPANEL), SetMinimalSize(50,12), SetResize(1,0), EndContainer(),
		EndContainer(),
	EndContainer(),
	// Start/Stop buttons
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_START), SetMinimalSize(150, 12), SetDataTip(STR_TMPL_RPL_START, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
		NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TRAIN_FLUFF_LEFT), SetMinimalSize(15, 12), EndContainer(),
		NWidget(WWT_DROPDOWN, COLOUR_GREY, TRW_WIDGET_TRAIN_RAILTYPE_DROPDOWN), SetMinimalSize(150, 12), SetDataTip(0x0, STR_REPLACE_HELP_RAILTYPE), SetResize(1, 0),
		NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TRAIN_FLUFF_RIGHT), SetMinimalSize(16, 12), EndContainer(),
		NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_STOP), SetMinimalSize(150, 12), SetDataTip(STR_TMPL_RPL_STOP, STR_REPLACE_REMOVE_WAGON_HELP),
		NWidget(WWT_RESIZEBOX, COLOUR_GREY),
	EndContainer(),
};

static WindowDesc _replace_rail_vehicle_desc(
	WDP_AUTO,
	"template replace window",
	456, 156,
	WC_TEMPLATEGUI_MAIN,
	WC_NONE,					// parent window class
	WDF_CONSTRUCTION,
	_widgets, lengthof(_widgets)
);

class TemplateReplaceWindow : public Window {
private:

	GUIGroupList groups;  		 ///< List of groups
	byte unitnumber_digits;

	short line_height;
	short matrixContentLeftMargin;

	int details_height;           ///< Minimal needed height of the details panels (found so far).
	RailType sel_railtype;        ///< Type of rail tracks selected.
	Scrollbar *vscroll[2];
	// listing/sorting continued
	GUITemplateList templates;
	GUITemplateList::SortFunction **template_sorter_funcs;

	short selected_template_index;
	short selected_group_index;

	bool templateNotice;
	bool editInProgress;

public:
	TemplateReplaceWindow(WindowDesc *wdesc, byte dig, int step_h) : Window(wdesc)
	{
		// listing/sorting
		templates.SetSortFuncs(this->template_sorter_funcs);

		// From BaseVehicleListWindow
		this->unitnumber_digits = dig;

		this->sel_railtype = RAILTYPE_BEGIN;
		this->details_height   = 10 * FONT_HEIGHT_NORMAL + WD_FRAMERECT_TOP + WD_FRAMERECT_BOTTOM;

		this->line_height = step_h;

		this->CreateNestedTree(wdesc);
		this->vscroll[0] = this->GetScrollbar(TRW_WIDGET_TOP_SCROLLBAR);
		this->vscroll[1] = this->GetScrollbar(TRW_WIDGET_BOTTOM_SCROLLBAR);
		this->vscroll[0]->SetStepSize(step_h / 2);
		this->vscroll[1]->SetStepSize(step_h);
		this->FinishInitNested(VEH_TRAIN);

		this->owner = _local_company;

		this->groups.ForceRebuild();
		this->groups.NeedResort();
 		this->BuildGroupList(_local_company);
		this->groups.Sort(&GroupNameSorter);


		this->matrixContentLeftMargin = 40;
		this->selected_template_index = -1;
		this->selected_group_index = -1;

		this->templateNotice = false;
		this->editInProgress = false;

		this->templates.ForceRebuild();

		BuildTemplateGuiList(&this->templates, this->vscroll[1], this->owner, this->sel_railtype);
	}

	~TemplateReplaceWindow() {
		DeleteWindowById(WC_CREATE_TEMPLATE, this->window_number);
	}

	virtual void UpdateWidgetSize(int widget, Dimension *size, const Dimension &padding, Dimension *fill, Dimension *resize)
	{
		switch (widget) {
			case TRW_WIDGET_TOP_MATRIX:
				resize->height = GetVehicleListHeight(VEH_TRAIN, FONT_HEIGHT_NORMAL + WD_MATRIX_TOP) / 2;
				size->height = 8 * resize->height;
				break;
			case TRW_WIDGET_BOTTOM_MATRIX:
				resize->height = GetVehicleListHeight(VEH_TRAIN, FONT_HEIGHT_NORMAL + WD_MATRIX_TOP);
				size->height = 4 * resize->height;
				break;
			case TRW_WIDGET_TRAIN_RAILTYPE_DROPDOWN: {
				Dimension d = {0, 0};
				for (RailType rt = RAILTYPE_BEGIN; rt != RAILTYPE_END; rt++) {
					const RailtypeInfo *rti = GetRailTypeInfo(rt);
					// Skip rail type if it has no label
					if (rti->label == 0) continue;
					d = maxdim(d, GetStringBoundingBox(rti->strings.replace_text));
				}
				d.width += padding.width;
				d.height += padding.height;
				*size = maxdim(*size, d);
				break;
			}
		}
	}

	virtual void SetStringParameters(int widget) const
	{
		switch (widget) {
			case TRW_CAPTION:
				SetDParam(0, STR_TMPL_RPL_TITLE);
				break;
		}
	}

	virtual void DrawWidget(const Rect &r, int widget) const
	{
		switch (widget) {
			case TRW_WIDGET_TOP_MATRIX: {
				DrawAllGroupsFunction(this->line_height, r);
				break;
			}
			case TRW_WIDGET_BOTTOM_MATRIX: {
				DrawTemplateList(this->line_height, r);
				break;
			}
			case TRW_WIDGET_TMPL_INFO_PANEL: {
				DrawTemplateInfo(this->line_height, r);
				break;
			}
		}
	}

	virtual void OnPaint()
	{
		BuildTemplateGuiList(&this->templates, this->vscroll[1], this->owner, this->sel_railtype);

		this->BuildGroupList(_local_company);
		this->groups.Sort(&GroupNameSorter);

		if ( templateNotice ) {
			BuildTemplateGuiList(&this->templates, vscroll[1], _local_company, this->sel_railtype);
			templateNotice = false;
			this->SetDirty();
		}
		/* sets the colour of that art thing */
		this->GetWidget<NWidgetCore>(TRW_WIDGET_TRAIN_FLUFF_LEFT)->colour  = _company_colours[_local_company];
		this->GetWidget<NWidgetCore>(TRW_WIDGET_TRAIN_FLUFF_RIGHT)->colour = _company_colours[_local_company];

		/* Show the selected railtype in the pulldown menu */
		this->GetWidget<NWidgetCore>(TRW_WIDGET_TRAIN_RAILTYPE_DROPDOWN)->widget_data = GetRailTypeInfo(sel_railtype)->strings.replace_text;

		this->DrawWidgets();
	}

	virtual void OnClick(Point pt, int widget, int click_count)
	{
		if ( this->editInProgress ) return;

		switch (widget) {
			case TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REUSE: {
				if ( this->selected_template_index >= 0 ) {
					TemplateVehicle *sel = TemplateVehicle::Get(((this->templates)[selected_template_index])->index);
					sel->ToggleReuseDepotVehicles();
				}
				break;
			}
			case TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_KEEP: {
				if ( this->selected_template_index >= 0 ) {
					TemplateVehicle *sel = TemplateVehicle::Get(((this->templates)[selected_template_index])->index);
					sel->ToggleKeepRemainingVehicles();
				}
				break;
			}
			case TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REFIT: {
				if ( this->selected_template_index >= 0 ) {
					TemplateVehicle *sel = TemplateVehicle::Get(((this->templates)[selected_template_index])->index);
					sel->ToggleRefitAsTemplate();
				}
				break;
			}
			case TRW_WIDGET_TMPL_BUTTONS_DEFINE:
				ShowTemplateCreateWindow(0, &templateNotice, &editInProgress, this->line_height);
				break;
			case TRW_WIDGET_TMPL_BUTTONS_EDIT: {
				if ( this->selected_template_index >= 0 ) {
					editInProgress = true;
					TemplateVehicle *sel = TemplateVehicle::Get(((this->templates)[selected_template_index])->index);
					ShowTemplateCreateWindow(sel, &templateNotice, &editInProgress, this->line_height);
				}
				break;
			}
			case TRW_WIDGET_TMPL_BUTTONS_CLONE: {
				this->SetWidgetDirty(TRW_WIDGET_TMPL_BUTTONS_CLONE);
				this->ToggleWidgetLoweredState(TRW_WIDGET_TMPL_BUTTONS_CLONE);

				if (this->IsWidgetLowered(TRW_WIDGET_TMPL_BUTTONS_CLONE)) {
					static const CursorID clone_icon =	SPR_CURSOR_CLONE_TRAIN;
					SetObjectToPlaceWnd(clone_icon, PAL_NONE, HT_VEHICLE, this);
				} else {
					ResetObjectToPlace();
				}
				break;
			}
			case TRW_WIDGET_TMPL_BUTTONS_DELETE:
				if ( selected_template_index >= 0 && !editInProgress ) {
					// identify template to delete
					TemplateVehicle *del = TemplateVehicle::Get(((this->templates)[selected_template_index])->index);
					// remove a corresponding template replacement if existing
					TemplateReplacement *tr = GetTemplateReplacementByTemplateID(del->index);
					if ( tr ) {
						delete tr;
					}
					delete del;
					BuildTemplateGuiList(&this->templates, this->vscroll[1], this->owner, this->sel_railtype);
					selected_template_index = -1;
				}
				break;
			case TRW_WIDGET_TMPL_BUTTONS_RPLALL: {
				ShowTemplateReplaceAllGui();
				break;
			}
			case TRW_WIDGET_TRAIN_RAILTYPE_DROPDOWN: // Railtype selection dropdown menu
				ShowDropDownList(this, GetRailTypeDropDownList(true), sel_railtype, TRW_WIDGET_TRAIN_RAILTYPE_DROPDOWN);
				break;
			case TRW_WIDGET_TOP_MATRIX: {
 				uint16 newindex = (uint16)((pt.y - this->nested_array[TRW_WIDGET_TOP_MATRIX]->pos_y) / (this->line_height/2) ) + this->vscroll[0]->GetPosition();
				if ( newindex == this->selected_group_index || newindex >= this->groups.Length() ) {
					this->selected_group_index = -1;
				}
				else if ( newindex < this->groups.Length() ) {
					this->selected_group_index = newindex;
				}
				break;
			}
			case TRW_WIDGET_BOTTOM_MATRIX: {
 				uint16 newindex = (uint16)((pt.y - this->nested_array[TRW_WIDGET_BOTTOM_MATRIX]->pos_y) / this->line_height) + this->vscroll[1]->GetPosition();
				if ( newindex == this->selected_template_index || newindex >= templates.Length() ) {
					this->selected_template_index = -1;
				}
				else if ( newindex < templates.Length() ) {
					this->selected_template_index = newindex;
				}
				break;
			}
			case TRW_WIDGET_START: {
				if ( this->selected_template_index >= 0 && this->selected_group_index >= 0) {
					uint32 tv_index = ((this->templates)[selected_template_index])->index;
					int current_group_index = (this->groups)[this->selected_group_index]->index;
					IssueTemplateReplacement(current_group_index, tv_index);
				}
				break;
			}
			case TRW_WIDGET_STOP:
				if ( this->selected_group_index == -1 )
					return;
				int current_group_index = (this->groups)[this->selected_group_index]->index;
				TemplateReplacement *tr = GetTemplateReplacementByGroupID(current_group_index);
				if ( tr )
					delete tr;
				break;
		}
		this->SetDirty();
	}

	virtual bool OnVehicleSelect(const Vehicle *v)
	{
		// create a new template from the clicked vehicle
		TemplateVehicle *tv = CloneTemplateVehicleFromTrain((const Train*)v);
		if ( !tv )	return false;

		BuildTemplateGuiList(&this->templates, vscroll[1], _local_company, this->sel_railtype);
		this->ToggleWidgetLoweredState(TRW_WIDGET_TMPL_BUTTONS_CLONE);
		ResetObjectToPlace();
		this->SetDirty();

		return true;
	}

	virtual void OnDropdownSelect(int widget, int index)
	{
		RailType temp = (RailType)index;
		if (temp == this->sel_railtype) return; // we didn't select a new one. No need to change anything
		this->sel_railtype = temp;
		/* Reset scrollbar positions */
		this->vscroll[0]->SetPosition(0);
		this->vscroll[1]->SetPosition(0);
		BuildTemplateGuiList(&this->templates, this->vscroll[1], this->owner, this->sel_railtype);
		this->SetDirty();
	}

	virtual void OnResize()
	{
		/* Top Matrix */
		NWidgetCore *nwi = this->GetWidget<NWidgetCore>(TRW_WIDGET_TOP_MATRIX);
		this->vscroll[0]->SetCapacityFromWidget(this, TRW_WIDGET_TOP_MATRIX);
		nwi->widget_data = (this->vscroll[0]->GetCapacity() << MAT_ROW_START) + (1 << MAT_COL_START);
		/* Bottom Matrix */
		NWidgetCore *nwi2 = this->GetWidget<NWidgetCore>(TRW_WIDGET_BOTTOM_MATRIX);
		this->vscroll[1]->SetCapacityFromWidget(this, TRW_WIDGET_BOTTOM_MATRIX);
		nwi2->widget_data = (this->vscroll[1]->GetCapacity() << MAT_ROW_START) + (1 << MAT_COL_START);
	}

	virtual void OnTick()
	{
		if ( templateNotice ) {
			BuildTemplateGuiList(&this->templates, this->vscroll[1], this->owner, this->sel_railtype);
			this->SetDirty();
			templateNotice = false;
		}

	}

	virtual void OnInvalidateData(int data = 0, bool gui_scope = true)
	{
		this->groups.ForceRebuild();
		this->templates.ForceRebuild();
	}

	/** For a given group (id) find the template that is issued for template replacement for this group and return this template's index
	 *  from the gui list */
	short FindTemplateIndexForGroup(short gid) const
	{
		TemplateReplacement *tr = GetTemplateReplacementByGroupID(gid);
		if ( !tr )
			return -1;

		for ( uint32 i=0; i<this->templates.Length(); ++i )
			if ( templates[i]->index == tr->sel_template )
				return i;
		return -1;
	}

		/** Sort the groups by their name */
	static int CDECL GroupNameSorter(const Group * const *a, const Group * const *b)
	{
		static const Group *last_group[2] = { NULL, NULL };
		static char         last_name[2][64] = { "", "" };

		if (*a != last_group[0]) {
			last_group[0] = *a;
			SetDParam(0, (*a)->index);
			GetString(last_name[0], STR_GROUP_NAME, lastof(last_name[0]));
		}

		if (*b != last_group[1]) {
			last_group[1] = *b;
			SetDParam(0, (*b)->index);
			GetString(last_name[1], STR_GROUP_NAME, lastof(last_name[1]));
		}

		int r = strnatcmp(last_name[0], last_name[1]); // Sort by name (natural sorting).
		if (r == 0) return (*a)->index - (*b)->index;
		return r;
	}

	void BuildGroupList(Owner owner)
	{
		if (!this->groups.NeedRebuild()) {
			return;
		}
		this->groups.Clear();

		const Group *g;
		FOR_ALL_GROUPS(g) {
			if (g->owner == owner ) {
				*this->groups.Append() = g;
			}
		}

		this->groups.Compact();
		this->groups.RebuildDone();
		this->vscroll[0]->SetCount(groups.Length());
	}

	void DrawAllGroupsFunction(int line_height, const Rect &r) const
	{
		int left = r.left + WD_MATRIX_LEFT;
		int right = r.right - WD_MATRIX_RIGHT;
		int y = r.top;
		int max = min(this->vscroll[0]->GetPosition() + this->vscroll[0]->GetCapacity(), this->groups.Length());

		/* Then treat all groups defined by/for the current company */
		for ( int i=this->vscroll[0]->GetPosition(); i<max; ++i ) {
			const Group *g = (this->groups)[i];
			short g_id = g->index;

			/* Fill the background of the current cell in a darker tone for the currently selected template */
			if ( this->selected_group_index == i ) {
				GfxFillRect(left, y, right, y+(this->line_height)/2, _colour_gradient[COLOUR_GREY][3]);
			}

			SetDParam(0, g_id);
			StringID str = STR_GROUP_NAME;
			DrawString(left+30, right, y+2, str, TC_BLACK);

			/* Draw the template in use for this group, if there is one */
			short template_in_use = FindTemplateIndexForGroup(g_id);
			if ( template_in_use >= 0 ) {
				SetDParam(0, template_in_use);
				DrawString ( left, right, y+2, STR_TMPL_GROUP_USES_TEMPLATE, TC_BLACK, SA_HOR_CENTER);
			}
			/* If there isn't a template applied from the current group, check if there is one for another rail type */
			else if ( GetTemplateReplacementByGroupID(g_id) ) {
				DrawString ( left, right, y+2, STR_TMPL_TMPLRPL_EX_DIFF_RAILTYPE, TC_SILVER, SA_HOR_CENTER);
			}

			/* Draw the number of trains that still need to be treated by the currently selected template replacement */
			TemplateReplacement *tr = GetTemplateReplacementByGroupID(g_id);
			if ( tr ) {
				TemplateVehicle *tv = TemplateVehicle::Get(tr->sel_template);
				int num_trains = NumTrainsNeedTemplateReplacement(g_id, tv);
				// Draw text
				TextColour color = TC_GREY;
				if ( num_trains ) color = TC_BLACK;
				DrawString(left, right-16, y+2, STR_TMPL_NUM_TRAINS_NEED_RPL, color, SA_RIGHT);
				// Draw number
				if ( num_trains ) color = TC_ORANGE;
				else color = TC_GREY;
				SetDParam(0, num_trains);
				DrawString(left, right-4, y+2, STR_JUST_INT, color, SA_RIGHT);
			}

			y+=line_height / 2;
		}
	}

	void DrawTemplateList(int line_height, const Rect &r) const
	{
		int left = r.left;
		int right = r.right;
		int y = r.top;

		Scrollbar *draw_vscroll = vscroll[1];
		uint max = min(draw_vscroll->GetPosition() + draw_vscroll->GetCapacity(), this->templates.Length());

		const TemplateVehicle *v;
		for ( uint i = draw_vscroll->GetPosition(); i < max; ++i) {

			v = (this->templates)[i];

			/* Fill the background of the current cell in a darker tone for the currently selected template */
			if ( this->selected_template_index == (int32)i ) {
				GfxFillRect(left, y, right, y+this->line_height, _colour_gradient[COLOUR_GREY][3]);
			}

			/* Draw a notification string for chains that are not runnable */
			if ( v->IsFreeWagonChain() ) {
				DrawString(left, right-2, y+line_height-FONT_HEIGHT_SMALL-WD_FRAMERECT_BOTTOM - 2, STR_TMPL_WARNING_FREE_WAGON, TC_RED, SA_RIGHT);
			}

			/* Draw the template's length in tile-units */
			SetDParam(0, v->GetRealLength());
			SetDParam(1, 1);
			DrawString(left, right-4, y+2, STR_TINY_BLACK_DECIMAL, TC_BLACK, SA_RIGHT);

			/* Draw the template */
			DrawTemplate(v, left+50, right, y);

			/* Buying cost */
			SetDParam(0, CalculateOverallTemplateCost(v));
			DrawString(left+35, right, y + line_height - FONT_HEIGHT_SMALL - WD_FRAMERECT_BOTTOM - 2, STR_TMPL_TEMPLATE_OVR_VALUE_notinyfont, TC_BLUE, SA_LEFT);

			/* Index of current template vehicle in the list of all templates for its company */
			SetDParam(0, i);
			DrawString(left+5, left+25, y + line_height/2, STR_BLACK_INT, TC_BLACK, SA_RIGHT);

			/* Draw whether the current template is in use by any group */
			if ( v->NumGroupsUsingTemplate() > 0 ) {
				DrawString(left+200, right, y + line_height - FONT_HEIGHT_SMALL - WD_FRAMERECT_BOTTOM - 2, STR_TMP_TEMPLATE_IN_USE, TC_GREEN, SA_LEFT);
			}

			/* Draw information about template configuration settings */
			TextColour color;
			if ( v->IsSetReuseDepotVehicles() ) color = TC_LIGHT_BLUE;
			else color = TC_GREY;
			DrawString(left+200, right, y+2, STR_TMPL_CONFIG_USEDEPOT, color, SA_LEFT);
			if ( v->IsSetKeepRemainingVehicles() ) color = TC_LIGHT_BLUE;
			else color = TC_GREY;
			DrawString(left+275, right, y+2, STR_TMPL_CONFIG_KEEPREMAINDERS, color, SA_LEFT);
			if ( v->IsSetRefitAsTemplate() ) color = TC_LIGHT_BLUE;
			else color = TC_GREY;
			DrawString(left+350, right, y+2, STR_TMPL_CONFIG_REFIT, color, SA_LEFT);

			y += line_height;
		}
	}

	void DrawTemplateInfo(int line_height, const Rect &r) const
	{
		if ( this->selected_template_index == -1 || (short)this->templates.Length() <= this->selected_template_index )
			return;

		const TemplateVehicle *tmp = this->templates[this->selected_template_index];

		/* Draw vehicle performance info */
		SetDParam(2, tmp->max_speed);
		SetDParam(1, tmp->power);
		SetDParam(0, tmp->weight);
		SetDParam(3, tmp->max_te);
		DrawString(r.left+8, r.right, r.top+4, STR_VEHICLE_INFO_WEIGHT_POWER_MAX_SPEED_MAX_TE);

		/* Draw cargo summary */
		short top = r.top + 24;
		short left = r.left + 8;
		short count_rows = 0;
		short max_rows = 2;

		CargoArray cargo_caps;
		for ( ; tmp; tmp=tmp->Next() )
			cargo_caps[tmp->cargo_type] += tmp->cargo_cap;
		int y = top;
		for (CargoID i = 0; i < NUM_CARGO; ++i) {
			if ( cargo_caps[i] > 0 ) {
				count_rows++;
				SetDParam(0, i);
				SetDParam(1, cargo_caps[i]);
				SetDParam(2, _settings_game.vehicle.freight_trains);
				DrawString(left, r.right, y, FreightWagonMult(i) > 1 ? STR_TMPL_CARGO_SUMMARY_MULTI : STR_TMPL_CARGO_SUMMARY, TC_WHITE, SA_LEFT);
				y += this->line_height/2;
				if ( count_rows % max_rows == 0 ) {
					y = top;
					left += 150;
				}
			}
		}
	}
};

void ShowTemplateReplaceWindow(byte dig, int step_h)
{
	new TemplateReplaceWindow(&_replace_rail_vehicle_desc, dig, step_h);
}

