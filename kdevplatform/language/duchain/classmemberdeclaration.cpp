/*
    SPDX-FileCopyrightText: 2002-2005 Roberto Raggi <roberto@kdevelop.org>
    SPDX-FileCopyrightText: 2006 Adam Treat <treat@kde.org>
    SPDX-FileCopyrightText: 2006 Hamish Rodda <rodda@kde.org>
    SPDX-FileCopyrightText: 2007-2008 David Nolden <david.nolden.kdevelop@art-master.de>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "classmemberdeclaration.h"
#include "classmemberdeclarationdata.h"
#include "duchainregister.h"
#include <editor/rangeinrevision.h>

namespace KDevelop {
ClassMemberDeclarationData::ClassMemberDeclarationData()
    : m_accessPolicy(Declaration::Public)
    , m_isStatic(false)
    , m_isAuto(false)
    , m_isFriend(false)
    , m_isRegister(false)
    , m_isExtern(false)
    , m_isMutable(false)
    , m_bitWidth(-1)
    , m_bitOffsetOf(-1)
{
}

ClassMemberDeclaration::ClassMemberDeclaration(const ClassMemberDeclaration& rhs) : Declaration(*new ClassMemberDeclarationData(
            *rhs.d_func()))
{
}

REGISTER_DUCHAIN_ITEM(ClassMemberDeclaration);

Declaration* ClassMemberDeclaration::clonePrivate() const
{
    return new ClassMemberDeclaration(*this);
}

ClassMemberDeclaration::ClassMemberDeclaration(const RangeInRevision& range, DUContext* context)
    : Declaration(*new ClassMemberDeclarationData, range)
{
    d_func_dynamic()->setClassId(this);
    if (context)
        setContext(context);
}

ClassMemberDeclaration::ClassMemberDeclaration(ClassMemberDeclarationData& dd, const RangeInRevision& range)
    : Declaration(dd, range)
{
}

ClassMemberDeclaration::ClassMemberDeclaration(ClassMemberDeclarationData& dd)
    : Declaration(dd)
{
}

ClassMemberDeclaration::~ClassMemberDeclaration()
{
}

bool ClassMemberDeclaration::isStatic() const
{
    return d_func()->m_isStatic;
}

void ClassMemberDeclaration::setStatic(bool isStatic)
{
    d_func_dynamic()->m_isStatic = isStatic;
}

bool ClassMemberDeclaration::isAuto() const
{
    return d_func()->m_isAuto;
}

void ClassMemberDeclaration::setAuto(bool isAuto)
{
    d_func_dynamic()->m_isAuto = isAuto;
}

bool ClassMemberDeclaration::isFriend() const
{
    return d_func()->m_isFriend;
}

void ClassMemberDeclaration::setFriend(bool isFriend)
{
    d_func_dynamic()->m_isFriend = isFriend;
}

bool ClassMemberDeclaration::isRegister() const
{
    return d_func()->m_isRegister;
}

void ClassMemberDeclaration::setRegister(bool isRegister)
{
    d_func_dynamic()->m_isRegister = isRegister;
}

bool ClassMemberDeclaration::isExtern() const
{
    return d_func()->m_isExtern;
}

void ClassMemberDeclaration::setExtern(bool isExtern)
{
    d_func_dynamic()->m_isExtern = isExtern;
}

bool ClassMemberDeclaration::isMutable() const
{
    return d_func()->m_isMutable;
}

void ClassMemberDeclaration::setMutable(bool isMutable)
{
    d_func_dynamic()->m_isMutable = isMutable;
}

Declaration::AccessPolicy ClassMemberDeclaration::accessPolicy() const
{
    return d_func()->m_accessPolicy;
}

void ClassMemberDeclaration::setAccessPolicy(Declaration::AccessPolicy accessPolicy)
{
    d_func_dynamic()->m_accessPolicy = accessPolicy;
}

void ClassMemberDeclaration::setStorageSpecifiers(StorageSpecifiers specifiers)
{
    DUCHAIN_D_DYNAMIC(ClassMemberDeclaration);
    d->m_isStatic = specifiers & StaticSpecifier;
    d->m_isAuto = specifiers & AutoSpecifier;
    d->m_isFriend = specifiers & FriendSpecifier;
    d->m_isRegister = specifiers & RegisterSpecifier;
    d->m_isExtern = specifiers & ExternSpecifier;
    d->m_isMutable = specifiers & MutableSpecifier;
}

int64_t ClassMemberDeclaration::bitOffsetOf() const
{
    return d_func()->m_bitOffsetOf;
}

void ClassMemberDeclaration::setBitOffsetOf(int64_t bitOffsetOf)
{
    d_func_dynamic()->m_bitOffsetOf = bitOffsetOf;
}

int ClassMemberDeclaration::bitWidth() const
{
    return d_func()->m_bitWidth;
}

void ClassMemberDeclaration::setBitWidth(int bitWidth)
{
    d_func_dynamic()->m_bitWidth = bitWidth;
}
}
