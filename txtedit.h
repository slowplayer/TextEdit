#ifndef TXTEDIT_H
#define TXTEDIT_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>

class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;

class TxtEdit : public QMainWindow
{
    Q_OBJECT

public:
    TxtEdit(QWidget *parent = 0);
protected:
    virtual void closeEvent(QCloseEvent *e);
private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    bool load(const QString &f);
    bool maybesave();
    void setupCurrentFileName(const QString &filename);
private slots:
    void fileNew();
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamliy(const QString &f);
    void textSize(const QString &p);
    void textColor();
    void textAlign(QAction *a);

    void currengCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

    QAction *actionSave;
    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
    QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionAlignJustify;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    QToolBar *tb;
    QString fileName;
    QTextEdit *textEdit;


};

#endif // TXTEDIT_H
