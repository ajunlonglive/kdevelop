/*
    SPDX-FileCopyrightText: 1999-2001 Bernd Gehrmann <bernd@kdevelop.org>
    SPDX-FileCopyrightText: 1999-2001 the KDevelop Team
    SPDX-FileCopyrightText: 2007 Dukju Ahn <dukjuahn@gmail.com>
    SPDX-FileCopyrightText: 2010 Julien Desgats <julien.desgats@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDEVPLATFORM_PLUGIN_GREPDIALOG_H
#define KDEVPLATFORM_PLUGIN_GREPDIALOG_H

#include <QDialog>
#include <QUrl>

#include "grepjob.h"
#include "ui_grepwidget.h"

class GrepViewPlugin;

class GrepDialog : public QDialog, private Ui::GrepWidget
{
    Q_OBJECT

public:
    explicit GrepDialog(GrepViewPlugin *plugin, QWidget *parent = nullptr, bool show = true);
    ~GrepDialog() override;

    void setSettings(const GrepJobSettings &settings);
    GrepJobSettings settings() const;
    ///Rerun all grep jobs from a list of settings, called by GrepOutputView
    void historySearch(QVector<GrepJobSettings> &settingsHistory);

public Q_SLOTS:
    ///Start a new search
    void startSearch();

    ///Sets directory(ies)/files to search in. Also it can be semicolon separated list of directories/files or one of special strings: allOpenFilesString, allOpenProjectsString
    void setSearchLocations(const QString &dir);

private Q_SLOTS:
    void templateTypeComboActivated(int);
    void patternComboEditTextChanged( const QString& );
    QMenu* createSyncButtonMenu();
    void addUrlToMenu(QMenu* ret, const QUrl& url);
    void addStringToMenu(QMenu* ret, const QString& string);
    void synchronizeDirActionTriggered(bool);

    ///Check if all projects have been loaded
    bool checkProjectsOpened();
    ///Call the next element in m_jobs_history or close the dialog if all jobs are done
    ///\param[in] next if false, skip pending jobs
    void nextHistory(bool next);

    ///Opens the dialog to select a directory to search in, and inserts it into Location(s) field.
    void selectDirectoryDialog();

protected:
    ///Prevent showing the dialog if m_show is false
    void setVisible(bool visible) override;
    void closeEvent(QCloseEvent* closeEvent) override;

private:
    ///Returns whether the given url is a subfile/subdirectory of one of the chosen directories/files
    ///
    ///This is slow, so don't call it too often
    bool isPartOfChoice(const QUrl& url) const;
    ///Checks what a user has entered into the dialog and saves the data in m_settings
    void updateSettings();

    GrepViewPlugin * m_plugin;
    ///Allow to show a dialog
    const bool m_show;
    ///Current setting
    GrepJobSettings m_settings;
    ///List of remaining grep job settings to be done
    QVector<GrepJobSettings> m_historyJobSettings;
};


#endif

