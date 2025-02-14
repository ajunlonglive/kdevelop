/*
    SPDX-FileCopyrightText: 2007 Hamish Rodda <rodda@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDEVPLATFORM_ISTATUS_H
#define KDEVPLATFORM_ISTATUS_H

#include "interfacesexport.h"
#include <QObject>

namespace KDevelop {

/**
 * An interface for plugins, languages etc. to provide status updates.
 * Plugins only have to implement this extension interface, registration
 * happens automagically.
 * Regular QObjects can register themselves through IUiController like so:
 * @code
 *    Foo* f; // some QObject that implements IStatus
 *    ICore::self()->uiController()->registerStatus(f);
 * @endcode
 */
class KDEVPLATFORMINTERFACES_EXPORT IStatus
{
public:
    virtual ~IStatus();

    /**
     * Return a name for the status object
     */
    virtual QString statusName() const = 0;

Q_SIGNALS:
    /**
     * Request the current message for this plugin to be cleared.
     */
    virtual void clearMessage( IStatus* ) = 0;

    /**
     * Request a status \a message to be shown for this plugin, with a given \a timeout.
     *
     * \param message Message to display
     * \param timeout Timeout in milliseconds, or pass 0 for no timeout.
     */
    virtual void showMessage( IStatus*, const QString & message, int timeout = 0) = 0;

    /**
     * Request an error \a message to be shown for this plugin, with a given \a timeout.
     *
     * \param message Message to display
     * \param timeout Timeout in milliseconds, or pass 0 for no timeout.
     */
    virtual void showErrorMessage(const QString & message, int timeout = 0) = 0;

    /**
     * Hide the progress bar.
     */
    virtual void hideProgress( IStatus* ) = 0;

    /**
     * Show a progress bar, with the given \a percentage.
     */
    virtual void showProgress( IStatus*, int minimum, int maximum, int value) = 0;
};

}

Q_DECLARE_METATYPE(KDevelop::IStatus*)
Q_DECLARE_INTERFACE( KDevelop::IStatus, "org.kdevelop.IStatus" )

#endif

