#include "txtedit.h"
#include <QTextCodec>
#include <QComboBox>
#include <QFontComboBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QTextEdit>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QWidgetData>
#include <QCloseevent>
#include <QTextDocumentWriter>
const QString rsrcPath=":/images/win";

TxtEdit::TxtEdit(QWidget *parent)
    :QMainWindow(parent)
{
    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    setupFileActions();
    setupEditActions();
    setupTextActions();

    textEdit=new QTextEdit(this);
    connect(textEdit,SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this,SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(textEdit,SIGNAL(cursorPositionChanged()),
            this,SLOT(cursorPositionChanged()));

    setCentralWidget(textEdit);
    textEdit->setFocus();
    setupCurrentFileName(QString());

    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());

    connect(textEdit->document(),SIGNAL(modificationChanged(bool)),
            actionSave,SLOT(setEnabled(bool)));
    connect(textEdit->document(),SIGNAL(modificationChanged(bool)),
            this,SLOT(setWindowModified(bool)));
    connect(textEdit->document(),SIGNAL(undoAvailable(bool)),
            actionUndo,SLOT(setEnabled(bool)));
    connect(textEdit->document(),SIGNAL(redoAvailable(bool)),
            actionRedo,SLOT(setEnabled(bool)));

    setWindowModified(textEdit->document()->isModified());
    actionSave->setEnabled(textEdit->document()->isModified());
    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

    connect(actionUndo,SIGNAL(triggered()),textEdit,SLOT(undo()));
    connect(actionRedo,SIGNAL(triggered()),textEdit,SLOT(redo()));

    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);
    connect(actionCut,SIGNAL(triggered()),textEdit,SLOT(cut()));
    connect(actionCopy,SIGNAL(triggered()),textEdit,SLOT(copy()));
    connect(actionPaste,SIGNAL(triggered()),textEdit,SLOT(paste()));

    connect(textEdit,SIGNAL(copyAvailable(bool)),actionCut,SLOT(setEnabled(bool)));
    connect(textEdit,SIGNAL(copyAvailable(bool)),actionCopy,SLOT(setEnabled(bool)));

    fileNew();

}
void TxtEdit::closeEvent(QCloseEvent *e)
{
    if(maybesave())
        e->accept();
    else
        e->ignore();
}
void TxtEdit::setupFileActions()
{
    QToolBar *tb=new QToolBar(this);
    tb->setWindowTitle(tr("文件工具栏"));
    addToolBar(tb);
    QMenu *menu=new QMenu(tr("文件"),this);
    menuBar()->addMenu(menu);

    QAction *a;
    QIcon newIcon=QIcon::fromTheme("document-new",QIcon(rsrcPath+"/filenew.png"));
    a=new QAction(newIcon,tr("新建"),this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::New);
    connect(a,SIGNAL(triggered()),this,SLOT(fileNew()));
    tb->addAction(a);
    menu->addAction(a);

    a=new QAction(QIcon::fromTheme("document-open",QIcon(rsrcPath+"/fileopen.png")),tr("打开..."),this);
    a->setShortcut(QKeySequence::Open);
    connect(a,SIGNAL(triggered()),this,SLOT(fileOpen()));
    tb->addAction(a);
    menu->addAction(a);
    menu->addSeparator();

    actionSave=a=new QAction(QIcon::fromTheme("document-save",QIcon(rsrcPath+"/filesave.png")),tr("保存"),this);
    a->setShortcut(QKeySequence::Save);
    connect(a,SIGNAL(triggered()),this,SLOT(fileSave()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);

    a=new QAction(tr("另存为..."),this);
    a->setPriority(QAction::LowPriority);
    connect(a,SIGNAL(triggered()),this,SLOT(fileSaveAs()));
    menu->addAction(a);
    menu->addSeparator();
    a=new QAction(tr("退出"),this);
    a->setShortcut(Qt::CTRL+Qt::Key_Q);
    connect(a,SIGNAL(triggered()),this,SLOT(close()));
    menu->addAction(a);

}
void TxtEdit::setupEditActions()
{
    QToolBar *tb=new QToolBar(this);
    tb->setWindowTitle(tr("编辑工具栏"));
    addToolBar(tb);
    QMenu *menu=new QMenu(tr("编辑"),this);
    menuBar()->addMenu(menu);
    QAction *a;
    a=actionUndo=new QAction(QIcon::fromTheme("edit-undo",QIcon(rsrcPath+"/editundo.png")),
                                              tr("&撤销"),this);
    a->setShortcut(QKeySequence::Undo);
    tb->addAction(a);
    menu->addAction(a);

    a=actionRedo=new QAction(QIcon::fromTheme("edit-redo",QIcon(rsrcPath+"/editredo.png")),
                                              tr("&重做"),this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Redo);
    tb->addAction(a);
    menu->addAction(a);

    a=actionCut=new QAction(QIcon::fromTheme("edit-cut",QIcon(rsrcPath+"/editcut.png")),
                                              tr("剪切"),this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Cut);
    tb->addAction(a);
    menu->addAction(a);

    a=actionCopy=new QAction(QIcon::fromTheme("edit-copy",QIcon(rsrcPath+"/editcopy.png")),
                                              tr("复制"),this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Copy);
    tb->addAction(a);
    menu->addAction(a);
    a=actionPaste=new QAction(QIcon::fromTheme("edit-paste",QIcon(rsrcPath+"/editpaste.png")),
                                              tr("粘贴"),this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Paste);
    tb->addAction(a);
    menu->addAction(a);



}
void TxtEdit::setupTextActions()
{
    QToolBar *tb=new QToolBar(this);
    tb->setWindowTitle(tr("格式工具栏1"));
    addToolBar(tb);
    QMenu *menu=new QMenu(tr("格式"),this);
    menuBar()->addMenu(menu);
    actionTextBold=new QAction(QIcon::fromTheme("format-text-bold",QIcon(rsrcPath+"/textbold.png")),
                               tr("加粗"),this);
    actionTextBold->setShortcut(Qt::CTRL+Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    connect(actionTextBold,SIGNAL(triggered()),this,SLOT(textBold()));
    tb->addAction(actionTextBold);
    menu->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    actionTextItalic=new QAction(QIcon::fromTheme("format-text-italic",QIcon(rsrcPath+"/textitalic.png")),
                               tr("倾斜"),this);
    actionTextItalic->setShortcut(Qt::CTRL+Qt::Key_I);
    actionTextItalic->setPriority(QAction::LowPriority);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    connect(actionTextItalic,SIGNAL(triggered()),this,SLOT(textItalic()));
    tb->addAction(actionTextItalic);
    menu->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    actionTextUnderline=new QAction(QIcon::fromTheme("format-text-underline",QIcon(rsrcPath+"/textunder.png")),
                               tr("下划线"),this);
    actionTextUnderline->setShortcut(Qt::CTRL+Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextItalic->setFont(underline);
    connect(actionTextUnderline,SIGNAL(triggered()),this,SLOT(textUnderline()));
    tb->addAction(actionTextUnderline);
    menu->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);
    menu->addSeparator();

    QActionGroup *grp=new QActionGroup(this);
    connect(grp,SIGNAL(triggered(QAction*)),this,SLOT(textAlign(QAction*)));
    actionAlignLeft=new QAction(QIcon::fromTheme("format-justify-left",QIcon(rsrcPath+"/textleft.png")),
                                tr("向左对齐"),grp);
    actionAlignCenter=new QAction(QIcon::fromTheme("format-justify-center",QIcon(rsrcPath+"/textcenter.png")),
                                        tr("中心对齐"),grp);
    actionAlignRight=new QAction(QIcon::fromTheme("format-justify-right",QIcon(rsrcPath+"/textright.png")),
                                tr("向右对齐"),grp);
    actionAlignJustify=new QAction(QIcon::fromTheme("format-justify-fill",QIcon(rsrcPath+"/textjustify.png")),
                                tr("两端对齐"),grp);
    actionAlignLeft->setShortcut(Qt::CTRL+Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    actionAlignCenter->setShortcut(Qt::CTRL+Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    actionAlignRight->setShortcut(Qt::CTRL+Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    actionAlignJustify->setShortcut(Qt::CTRL+Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);
    tb->addActions(grp->actions());
    menu->addActions(grp->actions());
    menu->addSeparator();

    QPixmap pix(16,16);
    pix.fill(Qt::black);
    actionTextColor=new QAction(pix,tr("字体颜色..."),this);
    connect(actionTextColor,SIGNAL(triggered()),this,SLOT(textColor()));
    tb->addAction(actionTextColor);
    menu->addAction(actionTextColor);

    tb=new QToolBar(this);
    tb->setAllowedAreas(Qt::TopToolBarArea|Qt::BottomToolBarArea);
    tb->setWindowTitle(tr("格式工具栏2"));
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);


    comboFont=new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont,SIGNAL(activated(QString)),this,SLOT(textFamliy(QString)));
    comboSize=new QComboBox(tb);
    tb->addWidget(comboSize);
    comboSize->setEditable(true);
    QFontDatabase db;
    foreach(int size,db.standardSizes())
        comboSize->addItem(QString::number(size));
    connect(comboSize,SIGNAL(activated(QString)),this,SLOT(textSize(QString)));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font()
                                                                   .pointSize())));

}
bool TxtEdit::load(const QString &f)
{
    if (!QFile::exists(f))
        return false;
    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString str = codec->toUnicode(data);
    textEdit->setPlainText(str);

    setupCurrentFileName(f);
    return true;
}
bool TxtEdit::maybesave()
{
    if(!textEdit->document()->isModified())
        return true;

    int ret=QMessageBox::warning(this,tr("记事本"),
                             tr("文档已经被修改，\n"
                                "是否保存改动？"),
                             tr("保存"),tr("不保存"),tr("取消"));
    if(ret==0)
        return fileSave();
    else if(ret==2)
        return false;
    return true;
}
void TxtEdit::setupCurrentFileName(const QString &filename)
{
    this->fileName=filename;
    textEdit->document()->setModified(false);
    QString shownName;
    if(fileName.isEmpty())
        shownName="未命名.txt";
    else
        shownName=QFileInfo(fileName).fileName();
    setWindowTitle(tr("%1[*]-%2").arg(shownName).arg(tr("记事本")));
    setWindowModified(false);
}
void TxtEdit::fileNew()
{
    if(maybesave())
    {
        textEdit->clear();
        setupCurrentFileName(QString());
    }
}
void TxtEdit::fileOpen()
{
    QString fn=QFileDialog::getOpenFileName(this,tr("打开文件..."),
                                            QString(),tr("文本文档(*.txt);;所有文件(*)"));
    if(!fn.isEmpty())
        load(fn);
}
bool TxtEdit::fileSave()
{
    if(fileName.isEmpty())
        return fileSaveAs();

    QTextDocumentWriter writer(fileName);
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    writer.setCodec(codec);
    bool success=writer.write(textEdit->document());
    if(success)
        textEdit->document()->setModified(false);
    return success;
}
bool TxtEdit::fileSaveAs()
{
    QString fn=QFileDialog::getSaveFileName(this,tr("另存为..."),
                                            QString(),tr("文本文档(*.txt);;所有文件(*)"));
    if(fn.isEmpty())
        return false;
    if(!(fn.endsWith(".txt",Qt::CaseInsensitive)))
        fn+=".txt";
    setupCurrentFileName(fn);
    return fileSave();
}
void TxtEdit::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold?QFont::Bold:QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}
void TxtEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}
void TxtEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}
void TxtEdit::textFamliy(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}
void TxtEdit::textSize(const QString &p)
{
    qreal pointSize=p.toFloat();
    if(p.toFloat()>0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}
void TxtEdit::textColor()
{
    QColor col=QColorDialog::getColor(textEdit->textColor(),this);
    if(!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
}
void TxtEdit::textAlign(QAction *a)
{
    if(a==actionAlignLeft)
        textEdit->setAlignment(Qt::AlignLeft|Qt::AlignAbsolute);
    else if(a==actionAlignCenter)
        textEdit->setAlignment(Qt::AlignCenter);
    else if (a==actionAlignRight)
        textEdit->setAlignment(Qt::AlignRight|Qt::AlignAbsolute);
    else if(a==actionAlignJustify)
        textEdit->setAlignment(Qt::AlignJustify);
}
void TxtEdit::currengCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}
void TxtEdit::cursorPositionChanged()
{
    alignmentChanged(textEdit->alignment());
}
void TxtEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor=textEdit->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);

}
void TxtEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboFont->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}
void TxtEdit::colorChanged(const QColor &c)
{
    QPixmap pix(16,16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}
void TxtEdit::alignmentChanged(Qt::Alignment a)
{
    if(a&Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if(a&Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if(a&Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if(a&Qt::AlignJustify)
        actionAlignJustify->setChecked(true);

}
