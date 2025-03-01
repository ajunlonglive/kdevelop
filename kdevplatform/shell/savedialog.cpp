/*
    SPDX-FileCopyrightText: 2002 Harald Fernengel <harry@kdevelop.org>
    SPDX-FileCopyrightText: 2008 Hamish Rodda <rodda@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "savedialog.h"

#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

#include <KLocalizedString>

#include <interfaces/idocument.h>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace KDevelop;

class DocumentItem : public QListWidgetItem
{
public:
    DocumentItem( IDocument* doc, QListWidget* parent )
        : QListWidgetItem(parent)
        , m_doc( doc )
    {
        setFlags(Qt::ItemIsUserCheckable | flags());
        setData(Qt::CheckStateRole, Qt::Checked);
        setText(m_doc->url().toDisplayString(QUrl::PreferLocalFile));
    }

    IDocument* doc() const { return m_doc; }

private:
    IDocument* const m_doc;
};

KSaveSelectDialog::KSaveSelectDialog( const QList<IDocument*>& files, QWidget * parent )
    : QDialog( parent )
{
    setWindowTitle( i18nc("@title:window", "Save Modified Files?") );

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel( i18n("The following files have been modified. Save them?"), this ));

    m_listWidget = new QListWidget(this);
    mainLayout->addWidget(m_listWidget);
//     m_listWidget->addColumn( "" );
//     m_listWidget->header()->hide();
//     m_listWidget->setSectionResizeMode( QListView::LastColumn );

    for (IDocument* doc : files) {
        new DocumentItem( doc, m_listWidget );
    }

    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Save);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KSaveSelectDialog::save);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KSaveSelectDialog::reject);
    auto user1Button = buttonBox->addButton(i18nc("@action:button", "Save &None" ), QDialogButtonBox::ActionRole);
    user1Button->setToolTip(i18nc("@info:tooltip", "Discard all modifications" ));
    connect(user1Button, &QPushButton::clicked, this, &KSaveSelectDialog::accept);
    mainLayout->addWidget(buttonBox);
}

KSaveSelectDialog::~KSaveSelectDialog()
{
}

void KSaveSelectDialog::save( )
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        auto* item = static_cast<DocumentItem*>(m_listWidget->item(i));
        if (item->data(Qt::CheckStateRole).toBool())
            item->doc()->save(IDocument::Silent);
    }

    accept();
}

