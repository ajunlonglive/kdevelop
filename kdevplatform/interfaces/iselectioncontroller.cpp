/*
    SPDX-FileCopyrightText: 2009 Andreas Pakulat <apaku@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "iselectioncontroller.h"

namespace KDevelop
{

ISelectionController::ISelectionController( QObject* parent )
    : QObject( parent )
{
}

ISelectionController::~ISelectionController()
{
}

}

