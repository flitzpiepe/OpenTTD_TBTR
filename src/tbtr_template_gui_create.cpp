/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file tbtr_template_gui_create.cpp Window for the creation of template trains. */

#include "stdafx.h"

#include "strings_func.h"
#include "tilehighlight_func.h"

#include "tbtr_template_gui_create.h"
#include "tbtr_template_vehicle_func.h"

class TemplateReplaceWindow;

// some space in front of the virtual train in the matrix
uint16 TRAIN_FRONT_SPACE = 16;

enum TemplateReplaceWindowWidgets {
	TCW_CAPTION,
	TCW_MATRIX_NEW_TMPL,
	TCW_INFO_PANEL,
	TCW_SCROLLBAR_NEW_TMPL,
	TCW_SELL_TMPL,
	TCW_NEW,
	TCW_OK,
	TCW_CANCEL,
	TCW_REFIT,
	TCW_CLONE,
};

static const NWidgetPart _widgets[] = {
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_CLOSEBOX, COLOUR_GREY),
		NWidget(WWT_CAPTION, COLOUR_GREY, TCW_CAPTION), SetDataTip(STR_TMPL_CREATEGUI_TITLE, STR_TOOLTIP_WINDOW_TITLE_DRAG_THIS),
		NWidget(WWT_SHADEBOX, COLOUR_GREY),
		NWidget(WWT_STICKYBOX, COLOUR_GREY),
	EndContainer(),
	NWidget(NWID_HORIZONTAL),
		NWidget(NWID_VERTICAL),
			NWidget(WWT_MATRIX, COLOUR_GREY, TCW_MATRIX_NEW_TMPL), SetMinimalSize(216, 60), SetFill(1, 0), SetDataTip(0x1, STR_REPLACE_HELP_LEFT_ARRAY), SetResize(1, 0), SetScrollbar(TCW_SCROLLBAR_NEW_TMPL),
			NWidget(WWT_PANEL, COLOUR_GREY, TCW_INFO_PANEL), SetMinimalSize(216,80), SetResize(1,1), EndContainer(),
			NWidget(NWID_HSCROLLBAR, COLOUR_GREY, TCW_SCROLLBAR_NEW_TMPL), SetResize(1,0),
		EndContainer(),
		NWidget(WWT_IMGBTN, COLOUR_GREY, TCW_SELL_TMPL), SetDataTip(0x0, STR_NULL), SetMinimalSize(23,23), SetResize(0, 1), SetFill(0, 1),
	EndContainer(),
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TCW_OK),		SetMinimalSize(52, 12), SetResize(1,0), SetDataTip(STR_TMPL_CONFIRM, STR_TMPL_CONFIRM),
		NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TCW_NEW),		SetMinimalSize(52, 12), SetResize(1,0), SetDataTip(STR_TMPL_NEW,	 STR_TMPL_NEW),
		NWidget(WWT_TEXTBTN, COLOUR_GREY, TCW_CLONE),	SetMinimalSize(52, 12), SetResize(1,0), SetDataTip(STR_TMPL_CREATE_CLONE_VEH,	 STR_TMPL_CREATE_CLONE_VEH),
		NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TCW_REFIT),	SetMinimalSize(52, 12), SetResize(1,0), SetDataTip(STR_TMPL_REFIT,	 STR_TMPL_REFIT),
		NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TCW_CANCEL),	SetMinimalSize(52, 12), SetResize(1,0), SetDataTip(STR_TMPL_CANCEL,  STR_TMPL_CANCEL),
		NWidget(WWT_RESIZEBOX, COLOUR_GREY),
	EndContainer(),
};

static WindowDesc _template_create_window_desc(
	WDP_AUTO,						// window position
	"template create window",		// const char* ini_key
	456, 100,						// window size
	WC_CREATE_TEMPLATE,				// window class
	WC_TEMPLATEGUI_MAIN,			// parent window class
	WDF_CONSTRUCTION,				// window flags
	_widgets, lengthof(_widgets)	// widgets + num widgets
);

static void TrainDepotMoveVehicle(const Vehicle *wagon, VehicleID sel, const Vehicle *head)
{
	const Vehicle *v = Vehicle::Get(sel);

	if (v == wagon) return;

	if (wagon == NULL) {
		if (head != NULL) wagon = head->Last();
	} else {
		wagon = wagon->Previous();
		if (wagon == NULL) return;
	}

	if (wagon == v) return;

	CmdMoveRailVehicle(INVALID_TILE, DC_EXEC, (_ctrl_pressed ? 1:0)<<20 | (1<<21) | v->index, wagon == NULL ? INVALID_VEHICLE : wagon->index, 0);
}

class TemplateCreateWindow : public Window {
private:
	Scrollbar *hscroll;
	int line_height;
	Train* virtual_train;
	bool editMode;
	bool *noticeParent;
	bool *createWindowOpen;			/// used to notify main window of progress (dummy way of disabling 'delete' while editing a template)
	bool virtualTrainChangedNotice;
	VehicleID selected_train;		/// the selected train in the GUI
	VehicleID vehicle_over;
	TemplateVehicle *editTemplate;

public:
	TemplateCreateWindow(WindowDesc* _wdesc, TemplateVehicle *to_edit, bool *notice, bool *windowOpen, int step_h) : Window(_wdesc)
	{
		this->line_height = step_h;
		this->CreateNestedTree(_wdesc);
		this->hscroll = this->GetScrollbar(TCW_SCROLLBAR_NEW_TMPL);
		this->FinishInitNested(VEH_TRAIN);
		/* a sprite */
		this->GetWidget<NWidgetCore>(TCW_SELL_TMPL)->widget_data = SPR_SELL_TRAIN;

		this->owner = _local_company;

		noticeParent = notice;
		createWindowOpen = windowOpen;
		virtualTrainChangedNotice = false;
		this->editTemplate = to_edit;

		if ( to_edit ) editMode = true;
		else editMode = false;

		this->selected_train = INVALID_VEHICLE;
		this->vehicle_over = INVALID_VEHICLE;

		this->virtual_train = VirtualTrainFromTemplateVehicle(to_edit);

		this->resize.step_height = 1;
	}
	~TemplateCreateWindow()
	{
		if ( virtual_train )
			delete virtual_train;

		SetWindowClassesDirty(WC_TRAINS_LIST);

		/* more cleanup */
		*createWindowOpen = false;
		DeleteWindowById(WC_BUILD_VIRTUAL_TRAIN, this->window_number);

	}

	virtual void UpdateWidgetSize(int widget, Dimension *size, const Dimension &padding, Dimension *fill, Dimension *resize)
	{
		switch (widget) {
			case TCW_MATRIX_NEW_TMPL:
				size->height = 20;
				break;
		}
	}
	virtual void OnResize()
	{
		NWidgetCore *nwi = this->GetWidget<NWidgetCore>(TCW_MATRIX_NEW_TMPL);
		this->hscroll->SetCapacity(nwi->current_x);
		nwi->widget_data = (this->hscroll->GetCapacity() << MAT_ROW_START) + (1 << MAT_COL_START);
	}


	virtual void OnInvalidateData(int data = 0, bool gui_scope = true)
	{
		virtualTrainChangedNotice = true;
	}
	virtual void OnClick(Point pt, int widget, int click_count)
	{
		switch(widget) {
			case TCW_MATRIX_NEW_TMPL: {
				NWidgetBase *nwi = this->GetWidget<NWidgetBase>(TCW_MATRIX_NEW_TMPL);
				ClickedOnVehiclePanel(pt.x - nwi->pos_x-TRAIN_FRONT_SPACE, pt.y - nwi->pos_y);
				break;
			}
			case TCW_NEW: {
				ShowBuildVirtualTrainWindow(&virtual_train, &virtualTrainChangedNotice);
				break;
			}
			case TCW_CLONE: {
				this->SetWidgetDirty(TCW_CLONE);
				this->ToggleWidgetLoweredState(TCW_CLONE);
				if (this->IsWidgetLowered(TCW_CLONE)) {
					static const CursorID clone_icon =	SPR_CURSOR_CLONE_TRAIN;
					SetObjectToPlaceWnd(clone_icon, PAL_NONE, HT_VEHICLE, this);
				} else {
					ResetObjectToPlace();
				}
				break;
			}
			case TCW_OK: {
				TemplateVehicle *tv = NULL;
				if ( editMode )	tv = DeleteTemplateVehicle(editTemplate);
				editTemplate = TemplateVehicleFromVirtualTrain(virtual_train);
				if ( tv ) *noticeParent = true;
				delete this;
				break;
			}
			case TCW_CANCEL: {
				delete this;
				break;
			}
			case TCW_REFIT: {
				if(this->virtual_train) {
					ShowVehicleRefitWindow(virtual_train, INVALID_VEH_ORDER_ID, this, false, true);
				}
				break;
			}
		}
	}
	virtual bool OnVehicleSelect(const Vehicle *v)
	{
		// throw away the current virtual train
		if ( virtual_train )
			delete this->virtual_train;
		// create a new one
		this->virtual_train = CloneVirtualTrainFromTrain((const Train*)v);
		this->ToggleWidgetLoweredState(TCW_CLONE);
		ResetObjectToPlace();
		this->SetDirty();

		return true;
	}
	virtual void DrawWidget(const Rect &r, int widget) const
	{
		switch(widget) {
			case TCW_MATRIX_NEW_TMPL: {
				if ( this->virtual_train ) {
					DrawTrainImage(virtual_train, r.left+TRAIN_FRONT_SPACE, r.right, r.top+2, this->selected_train, EIT_PURCHASE, this->hscroll->GetPosition(), this->vehicle_over);
					SetDParam(0, CeilDiv(virtual_train->gcache.cached_total_length * 10, TILE_SIZE));
					SetDParam(1, 1);
					DrawString(r.left, r.right, r.top, STR_TINY_BLACK_DECIMAL, TC_BLACK, SA_RIGHT);
				}
				break;
			}
			case TCW_INFO_PANEL: {
				if ( this->virtual_train ) {
					/* Draw vehicle performance info */
					const GroundVehicleCache *gcache = this->virtual_train->GetGroundVehicleCache();
					SetDParam(2, this->virtual_train->GetDisplayMaxSpeed());
					SetDParam(1, gcache->cached_power);
					SetDParam(0, gcache->cached_weight);
					SetDParam(3, gcache->cached_max_te / 1000);
					DrawString(r.left+8, r.right, r.top+4, STR_VEHICLE_INFO_WEIGHT_POWER_MAX_SPEED_MAX_TE);
					/* Draw cargo summary */
					CargoArray cargo_caps;
					for ( const Train *tmp=this->virtual_train; tmp; tmp=tmp->Next() )
						cargo_caps[tmp->cargo_type] += tmp->cargo_cap;
					int y = r.top+24;
					for (CargoID i = 0; i < NUM_CARGO; ++i) {
						if ( cargo_caps[i] > 0 ) {
							SetDParam(0, i);
							SetDParam(1, cargo_caps[i]);
							SetDParam(2, _settings_game.vehicle.freight_trains);
							DrawString(r.left+8, r.right, y, STR_TMPL_CARGO_SUMMARY, TC_WHITE, SA_LEFT);
							y += this->line_height/2;
						}
					}
				}
				break;
			}
			default:
				break;
		}
	}
	virtual void OnTick()
	{
		if ( virtualTrainChangedNotice ) {
			this->SetDirty();
			virtualTrainChangedNotice = false;
		}
	}
	virtual void OnDragDrop(Point pt, int widget)
	{
		switch (widget) {
			case TCW_MATRIX_NEW_TMPL: {
				const Vehicle *v = NULL;
				VehicleID sel;
				if ( virtual_train ) sel = virtual_train->index;
				else sel = INVALID_VEHICLE;

				this->SetDirty();

				NWidgetBase *nwi = this->GetWidget<NWidgetBase>(TCW_MATRIX_NEW_TMPL);
				GetDepotVehiclePtData gdvp = { NULL, NULL };

				if (this->GetVehicleFromDepotWndPt(pt.x - nwi->pos_x, pt.y - nwi->pos_y, &v, &gdvp) == MODE_DRAG_VEHICLE && sel != INVALID_VEHICLE) {
					if (gdvp.wagon == NULL || gdvp.wagon->index != sel) {
						this->vehicle_over = INVALID_VEHICLE;
						TrainDepotMoveVehicle(gdvp.wagon, sel, gdvp.head);
						virtual_train = virtual_train->First();
					}
				}
				break;
			}
			case TCW_SELL_TMPL: {
				if (this->IsWidgetDisabled(widget)) return;
				if (this->selected_train == INVALID_VEHICLE) return;

				this->virtual_train = DeleteVirtualTrain(this->virtual_train, Train::Get(this->selected_train));

				this->selected_train = INVALID_VEHICLE;

				this->SetDirty();
				break;
			}
			default:
				this->selected_train = INVALID_VEHICLE;
				this->SetDirty();
		}
		_cursor.vehchain = false;
		this->selected_train = INVALID_VEHICLE;
		this->SetDirty();
	}
	virtual void OnMouseDrag(Point pt, int widget)
	{
		if (this->selected_train == INVALID_VEHICLE) return;
		/* A rail vehicle is dragged.. */
		if (widget != TCW_MATRIX_NEW_TMPL) { // ..outside of the depot matrix.
			if (this->vehicle_over != INVALID_VEHICLE) {
				this->vehicle_over = INVALID_VEHICLE;
				this->SetWidgetDirty(TCW_MATRIX_NEW_TMPL);
			}
			return;
		}

		NWidgetBase *matrix = this->GetWidget<NWidgetBase>(widget);
		const Vehicle *v = NULL;
		GetDepotVehiclePtData gdvp = {NULL, NULL};

		if (this->GetVehicleFromDepotWndPt(pt.x - matrix->pos_x, pt.y - matrix->pos_y, &v, &gdvp) != MODE_DRAG_VEHICLE) return;
		VehicleID new_vehicle_over = INVALID_VEHICLE;
		if (gdvp.head != NULL) {
			if (gdvp.wagon == NULL && gdvp.head->Last()->index != this->selected_train) { // ..at the end of the train.
				/* NOTE: As a wagon can't be moved at the begin of a train, head index isn't used to mark a drag-and-drop
				 * destination inside a train. This head index is then used to indicate that a wagon is inserted at
				 * the end of the train.
				 */
				new_vehicle_over = gdvp.head->index;
			} else if (gdvp.wagon != NULL && gdvp.head != gdvp.wagon &&
					gdvp.wagon->index != this->selected_train &&
					gdvp.wagon->Previous()->index != this->selected_train) { // ..over an existing wagon.
				new_vehicle_over = gdvp.wagon->index;
			}
		}
		if (this->vehicle_over == new_vehicle_over) return;

		this->vehicle_over = new_vehicle_over;
		this->SetWidgetDirty(widget);
	}
	virtual void OnPaint()
	{
		uint max_width = 32;
		uint width = 0;
		if ( virtual_train )
			for (Train *v = virtual_train; v != NULL; v = v->Next())
				width += v->GetDisplayImageWidth();

		max_width = max(max_width, width);
		this->hscroll->SetCount(max_width+25);

		this->DrawWidgets();
	}
	struct GetDepotVehiclePtData {
		const Vehicle *head;
		const Vehicle *wagon;
	};

	enum DepotGUIAction {
		MODE_ERROR,
		MODE_DRAG_VEHICLE,
		MODE_SHOW_VEHICLE,
		MODE_START_STOP,
	};

	uint count_width;
	uint header_width;
 	DepotGUIAction GetVehicleFromDepotWndPt(int x, int y, const Vehicle **veh, GetDepotVehiclePtData *d) const
	{
		const NWidgetCore *matrix_widget = this->GetWidget<NWidgetCore>(TCW_MATRIX_NEW_TMPL);
		/* In case of RTL the widgets are swapped as a whole */
		if (_current_text_dir == TD_RTL) x = matrix_widget->current_x - x;

		uint xm = x;

		bool wagon = false;

		x += this->hscroll->GetPosition();
		const Train *v = virtual_train;
		d->head = d->wagon = v;

		if (xm <= this->header_width) {

			if (wagon) return MODE_ERROR;

			return MODE_SHOW_VEHICLE;
		}

		/* Account for the header */
		x -= this->header_width;

		/* find the vehicle in this row that was clicked */
		for (; v != NULL; v = v->Next()) {
			x -= v->GetDisplayImageWidth();
			if (x < 0) break;
		}

		d->wagon = (v != NULL ? v->GetFirstEnginePart() : NULL);

		return MODE_DRAG_VEHICLE;
	}

	void ClickedOnVehiclePanel(int x, int y)
	{
		GetDepotVehiclePtData gdvp = { NULL, NULL };
		const Vehicle *v = NULL;
		this->GetVehicleFromDepotWndPt(x, y, &v, &gdvp);

		v = gdvp.wagon;

		if (v != NULL && VehicleClicked(v)) return;
		VehicleID sel = this->selected_train;

		if (sel != INVALID_VEHICLE) {
			this->selected_train = INVALID_VEHICLE;
		} else if (v != NULL) {
			SetObjectToPlaceWnd(SPR_CURSOR_MOUSE, PAL_NONE, HT_DRAG, this);
			SetMouseCursorVehicle(v, EIT_IN_DEPOT);
			_cursor.vehchain = _ctrl_pressed;

			this->selected_train = v->index;
			this->SetDirty();
		}
	}
};

void ShowTemplateCreateWindow(TemplateVehicle *to_edit, bool *noticeParent, bool *createWindowOpen, int step_h)
{
	if ( BringWindowToFrontById(WC_CREATE_TEMPLATE, VEH_TRAIN) != NULL ) return;
	new TemplateCreateWindow(&_template_create_window_desc, to_edit, noticeParent, createWindowOpen, step_h);
}

