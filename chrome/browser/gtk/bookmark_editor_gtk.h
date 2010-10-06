// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_GTK_BOOKMARK_EDITOR_GTK_H_
#define CHROME_BROWSER_GTK_BOOKMARK_EDITOR_GTK_H_
#pragma once

#include "app/gtk_integers.h"
#include "app/gtk_signal.h"
#include "base/gtest_prod_util.h"
#include "base/string16.h"
#include "chrome/browser/bookmarks/bookmark_editor.h"
#include "chrome/browser/bookmarks/bookmark_model_observer.h"

class GURL;

typedef union  _GdkEvent GdkEvent;
typedef struct _GtkTreeIter GtkTreeIter;
typedef struct _GtkTreeSelection GtkTreeSelection;
typedef struct _GtkTreeStore GtkTreeStore;
typedef struct _GtkWidget GtkWidget;

// GTK version of the bookmark editor dialog.
class BookmarkEditorGtk : public BookmarkEditor,
                          public BookmarkModelObserver {
  FRIEND_TEST_ALL_PREFIXES(BookmarkEditorGtkTest, ChangeParent);
  FRIEND_TEST_ALL_PREFIXES(BookmarkEditorGtkTest, ChangeParentAndURL);
  FRIEND_TEST_ALL_PREFIXES(BookmarkEditorGtkTest, ChangeURLToExistingURL);
  FRIEND_TEST_ALL_PREFIXES(BookmarkEditorGtkTest, EditTitleKeepsPosition);
  FRIEND_TEST_ALL_PREFIXES(BookmarkEditorGtkTest, EditURLKeepsPosition);
  FRIEND_TEST_ALL_PREFIXES(BookmarkEditorGtkTest, ModelsMatch);
  FRIEND_TEST_ALL_PREFIXES(BookmarkEditorGtkTest, MoveToNewParent);
  FRIEND_TEST_ALL_PREFIXES(BookmarkEditorGtkTest, NewURL);
  FRIEND_TEST_ALL_PREFIXES(BookmarkEditorGtkTest, ChangeURLNoTree);
  FRIEND_TEST_ALL_PREFIXES(BookmarkEditorGtkTest, ChangeTitleNoTree);
 public:
  BookmarkEditorGtk(GtkWindow* window,
                    Profile* profile,
                    const BookmarkNode* parent,
                    const EditDetails& details,
                    BookmarkEditor::Configuration configuration);

  virtual ~BookmarkEditorGtk();

  void Show();
  void Close();

 private:
  void Init(GtkWindow* parent_window);

  // BookmarkModel observer methods. Any structural change results in
  // resetting the tree model.
  virtual void Loaded(BookmarkModel* model) { }
  virtual void BookmarkNodeMoved(BookmarkModel* model,
                                 const BookmarkNode* old_parent,
                                 int old_index,
                                 const BookmarkNode* new_parent,
                                 int new_index);
  virtual void BookmarkNodeAdded(BookmarkModel* model,
                                 const BookmarkNode* parent,
                                 int index);
  virtual void BookmarkNodeRemoved(BookmarkModel* model,
                                   const BookmarkNode* parent,
                                   int old_index,
                                   const BookmarkNode* node);
  virtual void BookmarkNodeChanged(BookmarkModel* model,
                                   const BookmarkNode* node) {}
  virtual void BookmarkNodeChildrenReordered(BookmarkModel* model,
                                             const BookmarkNode* node);
  virtual void BookmarkNodeFavIconLoaded(BookmarkModel* model,
                                         const BookmarkNode* node) {}

  // Resets the model of the tree and updates the various buttons appropriately.
  void Reset();

  // Returns the current url the user has input.
  GURL GetInputURL() const;

  // Returns the title the user has input.
  string16 GetInputTitle() const;

  // Invokes ApplyEdits with the selected node.
  //
  // TODO(erg): This was copied from the windows version. Both should be
  // cleaned up so that we don't overload ApplyEdits.
  void ApplyEdits();

  // Applies the edits done by the user. |selected_parent| gives the parent of
  // the URL being edited.
  void ApplyEdits(GtkTreeIter* selected_parent);

  // Adds a new group parented on |parent| and sets |child| to point to this
  // new group.
  void AddNewGroup(GtkTreeIter* parent, GtkTreeIter* child);

  CHROMEGTK_CALLBACK_0(BookmarkEditorGtk, void, OnSelectionChanged);
  CHROMEGTK_CALLBACK_1(BookmarkEditorGtk, void, OnResponse, int);
  CHROMEGTK_CALLBACK_1(BookmarkEditorGtk, gboolean, OnWindowDeleteEvent,
                       GdkEvent*);

  CHROMEGTK_CALLBACK_0(BookmarkEditorGtk, void, OnWindowDestroy);
  CHROMEGTK_CALLBACK_0(BookmarkEditorGtk, void, OnEntryChanged);

  CHROMEGTK_CALLBACK_0(BookmarkEditorGtk, void, OnNewFolderClicked);

  // Profile the entry is from.
  Profile* profile_;

  // The dialog to display on screen.
  GtkWidget* dialog_;
  GtkWidget* name_entry_;
  GtkWidget* url_entry_;  // This is NULL if IsEditingFolder.
  GtkWidget* tree_view_;
  GtkWidget* new_folder_button_;

  // Helper object that manages the currently selected item in |tree_view_|.
  GtkTreeSelection* tree_selection_;

  // Our local copy of the bookmark data that we make from the BookmarkModel
  // that we can modify as much as we want and still discard when the user
  // clicks Cancel.
  GtkTreeStore* tree_store_;

  // TODO(erg): BookmarkEditorView has an EditorTreeModel object here; convert
  // that into a GObject that implements the interface GtkTreeModel.

  // Initial parent to select. Is only used if node_ is NULL.
  const BookmarkNode* parent_;

  // Details about the node we're editing.
  const EditDetails details_;

  // Mode used to create nodes from.
  BookmarkModel* bb_model_;

  // If true, we're running the menu for the bookmark bar or other bookmarks
  // nodes.
  bool running_menu_for_root_;

  // Is the tree shown?
  bool show_tree_;

  DISALLOW_COPY_AND_ASSIGN(BookmarkEditorGtk);
};

#endif  // CHROME_BROWSER_GTK_BOOKMARK_EDITOR_GTK_H_