// SPDX-FileCopyrightText: © 2019-2022 Alexandros Theodotou <alex@zrythm.org>
// SPDX-License-Identifier: LicenseRef-ZrythmLicense

#include <string.h>

#include "audio/track.h"
#include "audio/tracklist.h"
#include "gui/backend/event.h"
#include "gui/backend/event_manager.h"
#include "gui/backend/wrapped_object_with_change_signal.h"
#include "gui/widgets/item_factory.h"
#include "gui/widgets/multi_selection.h"
#include "gui/widgets/track_filter_popover.h"
#include "project.h"
#include "settings/settings.h"
#include "utils/flags.h"
#include "utils/string.h"
#include "zrythm_app.h"

#include <adwaita.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

G_DEFINE_TYPE (
  TrackFilterPopoverWidget,
  track_filter_popover_widget,
  GTK_TYPE_POPOVER)

static void
on_track_name_changed (
  GtkEditable *              editable,
  TrackFilterPopoverWidget * self)
{
  const char * str = gtk_editable_get_text (editable);
  g_settings_set_string (S_UI, "track-filter-name", str);
  gtk_filter_changed (
    GTK_FILTER (self->custom_filter),
    GTK_FILTER_CHANGE_DIFFERENT);
}

static int
filter_func (void * gobj, void * user_data)
{
#if 0
  TrackFilterPopoverWidget * self =
    Z_TRACK_FILTER_POPOVER_WIDGET (user_data);
#endif

  char * name =
    g_settings_get_string (S_UI, "track-filter-name");

  WrappedObjectWithChangeSignal * wrapped_track =
    Z_WRAPPED_OBJECT_WITH_CHANGE_SIGNAL (gobj);
  Track * track = (Track *) wrapped_track->obj;
  g_return_val_if_fail (IS_TRACK_AND_NONNULL (track), false);

  bool ret = string_contains_substr_case_insensitive (
    track->name, name);

  g_free (name);

  if (track->filtered == ret)
    {
      track->filtered = !ret;
      EVENTS_PUSH (ET_TRACK_VISIBILITY_CHANGED, track);
    }

  return ret;
}

static void
refresh_track_col_view_items (TrackFilterPopoverWidget * self)
{
  for (int i = 0; i < TRACKLIST->num_tracks; i++)
    {
      Track * track = TRACKLIST->tracks[i];

      WrappedObjectWithChangeSignal * wrapped_track =
        wrapped_object_with_change_signal_new (
          track, WRAPPED_OBJECT_TYPE_TRACK);

      g_list_store_append (
        self->track_list_store, wrapped_track);
    }
}

static void
setup_col_view (
  TrackFilterPopoverWidget * self,
  GtkColumnView *            col_view)
{
  self->track_col_view = col_view;

  GListStore * store =
    g_list_store_new (WRAPPED_OBJECT_WITH_CHANGE_SIGNAL_TYPE);
  self->track_list_store = store;

  /* make sortable */
  GtkSorter * sorter = gtk_column_view_get_sorter (col_view);
  sorter = g_object_ref (sorter);
  GtkSortListModel * sort_list_model =
    gtk_sort_list_model_new (G_LIST_MODEL (store), sorter);

  /* make filterable */
  self->custom_filter =
    gtk_custom_filter_new (filter_func, self, NULL);
  GtkFilterListModel * filter_model = gtk_filter_list_model_new (
    G_LIST_MODEL (sort_list_model),
    GTK_FILTER (self->custom_filter));

  /* only allow single selection */
  GtkSingleSelection * sel = GTK_SINGLE_SELECTION (
    gtk_single_selection_new (G_LIST_MODEL (filter_model)));

  /* set model */
  gtk_column_view_set_model (
    col_view, GTK_SELECTION_MODEL (sel));

  /* add columns */
  item_factory_generate_and_append_column (
    self->track_col_view, self->item_factories,
    ITEM_FACTORY_TEXT, Z_F_NOT_EDITABLE, Z_F_RESIZABLE, NULL,
    _ ("Name"));
  item_factory_generate_and_append_column (
    self->track_col_view, self->item_factories,
    ITEM_FACTORY_TOGGLE, Z_F_EDITABLE, Z_F_RESIZABLE, NULL,
    _ ("Visibility"));

  /* refresh to add items */
  refresh_track_col_view_items (self);
}

TrackFilterPopoverWidget *
track_filter_popover_widget_new (void)
{
  TrackFilterPopoverWidget * self =
    g_object_new (TRACK_FILTER_POPOVER_WIDGET_TYPE, NULL);

  return self;
}

static void
track_filter_popover_finalize (TrackFilterPopoverWidget * self)
{
  g_ptr_array_unref (self->item_factories);

  G_OBJECT_CLASS (track_filter_popover_widget_parent_class)
    ->finalize (G_OBJECT (self));
}

static void
track_filter_popover_widget_class_init (
  TrackFilterPopoverWidgetClass * _klass)
{
  /*GtkWidgetClass * wklass = GTK_WIDGET_CLASS (_klass);*/

  GObjectClass * oklass = G_OBJECT_CLASS (_klass);
  oklass->finalize =
    (GObjectFinalizeFunc) track_filter_popover_finalize;
}

static void
track_filter_popover_widget_init (
  TrackFilterPopoverWidget * self)
{
  self->item_factories =
    g_ptr_array_new_with_free_func (item_factory_free_func);

  AdwPreferencesPage * ppage =
    ADW_PREFERENCES_PAGE (adw_preferences_page_new ());
  /*adw_preferences_page_set_title (ppage, _("Track Filtering & Visibility"));*/

  AdwPreferencesGroup * pgroup;
  AdwEntryRow *         entry_row;

  pgroup =
    ADW_PREFERENCES_GROUP (adw_preferences_group_new ());
  adw_preferences_group_set_title (pgroup, _ ("Filter"));
  adw_preferences_page_add (ppage, pgroup);

  entry_row = ADW_ENTRY_ROW (adw_entry_row_new ());
  {
    char * track_name =
      g_settings_get_string (S_UI, "track-filter-name");
    gtk_editable_set_text (
      GTK_EDITABLE (entry_row), track_name);
    g_free (track_name);
  }
  g_signal_connect (
    entry_row, "changed", G_CALLBACK (on_track_name_changed),
    self);
  adw_preferences_row_set_title (
    ADW_PREFERENCES_ROW (entry_row), _ ("Track name"));
  adw_preferences_group_add (pgroup, GTK_WIDGET (entry_row));

  AdwExpanderRow * exp_row =
    ADW_EXPANDER_ROW (adw_expander_row_new ());
  adw_preferences_row_set_title (
    ADW_PREFERENCES_ROW (exp_row), _ ("Track types"));
  MultiSelectionWidget * multi_select =
    multi_selection_widget_new ();
  multi_selection_widget_setup (
    multi_select, NULL, track_type_strings,
    TRACK_TYPE_FOLDER + 1, NULL, NULL, 0, NULL);
  GtkListBoxRow * list_box_row =
    GTK_LIST_BOX_ROW (gtk_list_box_row_new ());
  gtk_list_box_row_set_child (
    list_box_row, GTK_WIDGET (multi_select));
  adw_expander_row_add_row (
    exp_row, GTK_WIDGET (list_box_row));
  adw_expander_row_set_expanded (exp_row, true);
  adw_preferences_group_add (pgroup, GTK_WIDGET (exp_row));

  pgroup =
    ADW_PREFERENCES_GROUP (adw_preferences_group_new ());
  adw_preferences_group_set_title (pgroup, _ ("Visibility"));
  adw_preferences_page_add (ppage, pgroup);

  list_box_row = GTK_LIST_BOX_ROW (gtk_list_box_row_new ());
  GtkColumnView * col_view =
    GTK_COLUMN_VIEW (gtk_column_view_new (NULL));
  setup_col_view (self, col_view);
  gtk_list_box_row_set_child (
    list_box_row, GTK_WIDGET (col_view));
  adw_preferences_group_add (
    pgroup, GTK_WIDGET (list_box_row));

  gtk_popover_set_child (
    GTK_POPOVER (self), GTK_WIDGET (ppage));

  gtk_widget_set_size_request (GTK_WIDGET (self), -1, 600);
}
