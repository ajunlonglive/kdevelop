#ifndef __DOCUMENT_IMPL_H__
#define __DOCUMENT_IMPL_H__


#include <keditor/editor.h>


class KWriteDoc;
class KWrite;


class DocumentImpl : public KEditor::Document
{
  Q_OBJECT

public:

  DocumentImpl(KEditor::Editor *parent);
  virtual ~DocumentImpl();
  
  virtual bool saveFile();


protected:

  virtual bool openFile();


private:

  KWriteDoc *m_document;
  KWrite *m_view;

};


#endif
